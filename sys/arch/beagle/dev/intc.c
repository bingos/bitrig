/* $OpenBSD: intc.c,v 1.11 2013/05/10 00:18:42 patrick Exp $ */
/*
 * Copyright (c) 2007,2009 Dale Rahn <drahn@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/queue.h>
#include <sys/malloc.h>
#include <sys/device.h>
#include <sys/evcount.h>
#include <machine/bus.h>
#include <beagle/dev/omapvar.h>
#include "intc.h"

/* registers */
#define	INTC_REVISION		0x00	/* R */
#define	INTC_SYSCONFIG		0x10	/* RW */
#define		INTC_SYSCONFIG_AUTOIDLE		0x1
#define		INTC_SYSCONFIG_SOFTRESET	0x2
#define	INTC_SYSSTATUS		0x14	/* R */
#define		INTC_SYSSYSTATUS_RESETDONE	0x1
#define	INTC_SIR_IRQ		0x40	/* R */	
#define	INTC_SIR_FIQ		0x44	/* R */
#define	INTC_CONTROL		0x48	/* RW */
#define		INTC_CONTROL_NEWIRQ	0x1
#define		INTC_CONTROL_NEWFIQ	0x1
#define		INTC_CONTROL_GLOBALMASK	0x1
#define	INTC_PROTECTION		0x4c	/* RW */
#define		INTC_PROTECTION_PROT 1	/* only privileged mode */
#define	INTC_IDLE		0x50	/* RW */

#define INTC_IRQ_TO_REG(i)	(((i) >> 5) & 0x3)
#define INTC_IRQ_TO_REGi(i)	((i) & 0x1f)
#define	INTC_ITRn(i)		0x80+(0x20*i)+0x00	/* R */
#define	INTC_MIRn(i)		0x80+(0x20*i)+0x04	/* RW */
#define	INTC_CLEARn(i)		0x80+(0x20*i)+0x08	/* RW */
#define	INTC_SETn(i)		0x80+(0x20*i)+0x0c	/* RW */
#define	INTC_ISR_SETn(i)	0x80+(0x20*i)+0x10	/* RW */
#define	INTC_ISR_CLEARn(i)	0x80+(0x20*i)+0x14	/* RW */
#define INTC_PENDING_IRQn(i)	0x80+(0x20*i)+0x18	/* R */
#define INTC_PENDING_FIQn(i)	0x80+(0x20*i)+0x1c	/* R */

#define	INTC_ITR0		0x80	/* R */
#define	INTC_MIR0		0x84	/* RW */
#define	INTC_CLEAR0		0x88	/* RW */
#define	INTC_SET0		0x8c	/* RW */
#define	INTC_ISR_SET0		0x90	/* RW */
#define	INTC_ISR_CLEAR0		0x94	/* RW */
#define INTC_PENDING_IRQ0	0x98 	/* R */
#define INTC_PENDING_FIQ0	0x9c 	/* R */

#define	INTC_ITR1		0xa0	/* R */
#define	INTC_MIR1		0xa4	/* RW */
#define	INTC_CLEAR1		0xa8	/* RW */
#define	INTC_SET1		0xac	/* RW */
#define	INTC_ISR_SET1		0xb0	/* RW */
#define	INTC_ISR_CLEAR1		0xb4	/* RW */
#define INTC_PENDING_IRQ1	0xb8 	/* R */
#define INTC_PENDING_FIQ1	0xbc 	/* R */

#define	INTC_ITR2		0xc0	/* R */
#define	INTC_MIR2		0xc4	/* RW */
#define	INTC_CLEAR2		0xc8	/* RW */
#define	INTC_SET2		0xcc	/* RW */
#define	INTC_ISR_SET2		0xd0	/* RW */
#define	INTC_ISR_CLEAR2		0xd4	/* RW */
#define INTC_PENDING_IRQ2	0xd8 	/* R */
#define INTC_PENDING_FIQ2	0xdc 	/* R */

#define INTC_ILRn(i)		0x100+(4*i)
#define		INTC_ILR_IRQ	0x0		/* not of FIQ */
#define		INTC_ILR_FIQ	0x1
#define		INTC_ILR_PRIs(pri)	((pri) << 2)
#define		INTC_ILR_PRI(reg)	(((reg) >> 2) & 0x2f)
#define		INTC_MIN_PRI	63
#define		INTC_STD_PRI	32
#define		INTC_MAX_PRI	0

#define NIRQ	INTC_NUM_IRQ

volatile int softint_pending;

struct intrsource intc_handler[NIRQ];
u_int32_t intc_smask[NIPL];
u_int32_t intc_imask[3][NIPL];

bus_space_tag_t		intc_iot;
bus_space_handle_t	intc_ioh;

void	intc_attach(struct device *, struct device *, void *);
int	intc_spllower(int new);
int	intc_splraise(int new);
void	intc_setipl(int new);
void	intc_calc_mask(void);
void	intc_pic_hwunmask(struct pic *, int);

struct cfattach	intc_ca = {
	sizeof (struct device), NULL, intc_attach
};

struct cfdriver intc_cd = {
	NULL, "intc", DV_DULL
};

int intc_attached = 0;

void
intc_attach(struct device *parent, struct device *self, void *args)
{
	struct omap_attach_args *oa = args;
	int i;
	u_int32_t rev;

	intc_iot = oa->oa_iot;
	if (bus_space_map(intc_iot, oa->oa_dev->mem[0].addr,
	    oa->oa_dev->mem[0].size, 0, &intc_ioh))
		panic("intc_attach: bus_space_map failed!");

	rev = bus_space_read_4(intc_iot, intc_ioh, INTC_REVISION);

	printf(" rev %d.%d\n", rev >> 4 & 0xf, rev & 0xf);

	/* software reset of the part? */
	/* set protection bit (kernel only)? */
#if 0
	bus_space_write_4(intc_iot, intc_ioh, INTC_PROTECTION,
	     INTC_PROTECTION_PROT);
#endif

	/* XXX - check power saving bit */

	/* mask all interrupts */
	bus_space_write_4(intc_iot, intc_ioh, INTC_MIR0, 0xffffffff);
	bus_space_write_4(intc_iot, intc_ioh, INTC_MIR1, 0xffffffff);
	bus_space_write_4(intc_iot, intc_ioh, INTC_MIR2, 0xffffffff);

	struct pic *pic = malloc(sizeof(struct pic),
	    M_DEVBUF, M_ZERO | M_NOWAIT);
	pic->pic_hwunmask = intc_pic_hwunmask;

	for (i = 0; i < NIRQ; i++) {
		bus_space_write_4(intc_iot, intc_ioh, INTC_ILRn(i),
		    INTC_ILR_PRIs(INTC_MIN_PRI)|INTC_ILR_IRQ);

		intc_handler[i].is_pin = i;
		intc_handler[i].is_pic = pic;
	}

	intc_calc_mask();
	bus_space_write_4(intc_iot, intc_ioh, INTC_CONTROL,
	    INTC_CONTROL_NEWIRQ);

	intc_attached = 1;

	/* insert self as interrupt handler */
	arm_set_intr_handler(intc_splraise, intc_spllower, intc_splx,
	    intc_setipl,
	    intc_intr_establish, intc_intr_disestablish, intc_intr_string,
	    intc_irq_handler);

	intc_setipl(IPL_HIGH);  /* XXX ??? */
	enable_interrupts(I32_bit);
}

void
intc_calc_mask(void)
{
	struct cpu_info *ci = curcpu();
	int irq;
	struct intrhand *ih;
	int i;

	for (irq = 0; irq < NIRQ; irq++) {
		int max = IPL_NONE;
		int min = IPL_HIGH;
		for (ih = intc_handler[irq].is_handlers; ih != NULL; ih = ih->ih_next) {
			if (ih->ih_level > max)
				max = ih->ih_level;

			if (ih->ih_level < min)
				min = ih->ih_level;
		}

		intc_handler[irq].is_maxlevel = max;

		if (max == IPL_NONE)
			min = IPL_NONE;

#ifdef DEBUG_INTC
		if (min != IPL_NONE) {
			printf("irq %d to block at %d %d reg %d bit %d\n",
			    irq, max, min, INTC_IRQ_TO_REG(irq),
			    INTC_IRQ_TO_REGi(irq));
		}
#endif
		/* Enable interrupts at lower levels, clear -> enable */
		for (i = 0; i < min; i++)
			intc_imask[INTC_IRQ_TO_REG(irq)][i] &=
			    ~(1 << INTC_IRQ_TO_REGi(irq));
		for (; i <= IPL_HIGH; i++)
			intc_imask[INTC_IRQ_TO_REG(irq)][i] |=
			    1 << INTC_IRQ_TO_REGi(irq);
		/* XXX - set enable/disable, priority */ 
		bus_space_write_4(intc_iot, intc_ioh, INTC_ILRn(irq),
		    INTC_ILR_PRIs(NIPL-max)|INTC_ILR_IRQ);
	}
	arm_init_smask();
	intc_setipl(ci->ci_ilevel);
}

void
intc_splx(int new)
{
	struct cpu_info *ci = curcpu();
	intc_setipl(new);

	if (ci->ci_ipending & arm_smask[ci->ci_ilevel])
		arm_do_pending_intr(ci->ci_ilevel);
}

int
intc_spllower(int new)
{
	struct cpu_info *ci = curcpu();
	int old = ci->ci_ilevel;
	intc_splx(new);
	return (old);
}

int
intc_splraise(int new)
{
	struct cpu_info *ci = curcpu();
	int old;
	old = ci->ci_ilevel;

	/*
	 * setipl must always be called because there is a race window
	 * where the variable is updated before the mask is set
	 * an interrupt occurs in that window without the mask always
	 * being set, the hardware might not get updated on the next
	 * splraise completely messing up spl protection.
	 */
	if (old > new)
		new = old;

	intc_setipl(new);
  
	return (old);
}

void
intc_setipl(int new)
{
	struct cpu_info *ci = curcpu();
	int i;
	int psw;
	if (intc_attached == 0)
		return;

	psw = disable_interrupts(I32_bit);
#if 0
	{
		volatile static int recursed = 0;
		if (recursed == 0) {
			recursed = 1;
			if (new != 12) 
				printf("setipl %d\n", new);
			recursed = 0;
		}
	}
#endif
	ci->ci_ilevel = new;
	for (i = 0; i < 3; i++)
		bus_space_write_4(intc_iot, intc_ioh,
		    INTC_MIRn(i), intc_imask[i][new]);
	bus_space_write_4(intc_iot, intc_ioh, INTC_CONTROL,
	    INTC_CONTROL_NEWIRQ);
	restore_interrupts(psw);
}

void
intc_intr_bootstrap(vaddr_t addr)
{
	int i, j;
	extern struct bus_space armv7_bs_tag;
	intc_iot = &armv7_bs_tag;
	intc_ioh = addr;
	for (i = 0; i < 3; i++)
		for (j = 0; j < NIPL; j++)
			intc_imask[i][j] = 0xffffffff;
}

void
intc_irq_handler(void *frame)
{
	int irq, pri, s;
	struct intrhand *ih;
	void *arg;

	irq = bus_space_read_4(intc_iot, intc_ioh, INTC_SIR_IRQ);
#ifdef DEBUG_INTC
	printf("irq %d fired\n", irq);
#endif

	pri = intc_handler[irq].is_maxlevel;
	s = intc_splraise(pri);
	for (ih = intc_handler[irq].is_handlers; ih != NULL; ih = ih->ih_next) {
		if (ih->ih_arg != 0)
			arg = ih->ih_arg;
		else
			arg = frame;

		if (ih->ih_fun(arg)) 
			ih->ih_count.ec_count++;

	}

	bus_space_write_4(intc_iot, intc_ioh, INTC_CONTROL,
	    INTC_CONTROL_NEWIRQ);

	intc_splx(s);
}

void *
intc_intr_establish(int irqno, int level, int (*func)(void *),
    void *arg, char *name)
{
	int psw;
	struct intrhand *ih;

	if (irqno < 0 || irqno >= NIRQ)
		panic("intc_intr_establish: bogus irqnumber %d: %s",
		     irqno, name);
	psw = disable_interrupts(I32_bit);

	/* no point in sleeping unless someone can free memory. */
	ih = (struct intrhand *)malloc (sizeof *ih, M_DEVBUF,
	    cold ? M_NOWAIT : M_WAITOK);
	if (ih == NULL)
		panic("intr_establish: can't malloc handler info");
	ih->ih_fun = func;
	ih->ih_arg = arg;
	ih->ih_level = level & ~IPL_FLAGS;
	ih->ih_flags = level & IPL_FLAGS;
	ih->ih_irq = irqno;
	ih->ih_name = name;

	/* XXX: add flags to interrupt source? */
	intc_handler[irqno].is_flags |= ih->ih_flags;

	if (intc_handler[irqno].is_handlers != NULL) {
		struct intrhand		*tmp;
		for (tmp = intc_handler[irqno].is_handlers; tmp->ih_next != NULL; tmp = tmp->ih_next);
		tmp->ih_next = ih;
	} else {
		intc_handler[irqno].is_handlers = ih;
	}

	if (name != NULL)
		evcount_attach(&ih->ih_count, name, &ih->ih_irq);

#ifdef DEBUG_INTC
	printf("intc_intr_establish irq %d level %d [%s]\n", irqno, level,
	    name);
#endif
	intc_calc_mask();
	
	restore_interrupts(psw);
	return (ih);
}

void
intc_intr_disestablish(void *cookie)
{
	int psw;
	struct intrhand *ih = cookie;
	struct intrhand *tmp = NULL;
	int irqno = ih->ih_irq;
	psw = disable_interrupts(I32_bit);

	if (intc_handler[irqno].is_handlers == ih)
		intc_handler[irqno].is_handlers = NULL;
	else {
		for (tmp = intc_handler[irqno].is_handlers; tmp->ih_next != NULL; tmp++) {
			if (tmp->ih_next == ih) {
				tmp->ih_next = tmp->ih_next->ih_next;
				break;
			}
		}
	}

	if (ih->ih_name != NULL)
		evcount_detach(&ih->ih_count);
	free(ih, M_DEVBUF);
	restore_interrupts(psw);
}

const char *
intc_intr_string(void *cookie)
{
	return "huh?";
}

void
intc_pic_hwunmask(struct pic *pic, int pin)
{
	/* XXX: ??? */
}

#if 0
int intc_tst(void *a);

int
intc_tst(void *a)
{
	printf("inct_tst called\n");
	bus_space_write_4(intc_iot, intc_ioh, INTC_ISR_CLEAR0, 2);
	return 1;
}

void intc_test(void);
void intc_test(void)
{
	void * ih;
	printf("about to register handler\n");
	ih = intc_intr_establish(1, IPL_BIO, intc_tst, NULL, "intctst");

	printf("about to set bit\n");
	bus_space_write_4(intc_iot, intc_ioh, INTC_ISR_SET0, 2);

	printf("about to clear bit\n");
	bus_space_write_4(intc_iot, intc_ioh, INTC_ISR_CLEAR0, 2);

	printf("about to remove handler\n");
	intc_intr_disestablish(ih);

	printf("done\n");
}
#endif
