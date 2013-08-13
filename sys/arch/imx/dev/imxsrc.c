/* $OpenBSD: omdog.c,v 1.5 2011/11/15 23:01:11 drahn Exp $ */
/*
 * Copyright (c) 2013 Patrick Wildt <webmaster@patrick-wildt.de>
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
#include <sys/socket.h>
#include <sys/timeout.h>
#include <machine/intr.h>
#include <machine/bus.h>
#include <arm/cpufunc.h>
#include <imx/dev/imxvar.h>
#include <uvm/uvm_extern.h>
#include <machine/pmap.h>

/* registers */
#define SCR		0x00
#define SBMR1		0x04
#define SRSR		0x08
#define SISR		0x14
#define SIMR		0x18
#define SBMR2		0x1C
#define GPR1		0x20
#define GPR2		0x24
#define GPR3		0x28
#define GPR4		0x2C
#define GPR5		0x30
#define GPR6		0x34
#define GPR7		0x38
#define GPR8		0x3C
#define GPR9		0x40
#define GPR10		0x44

#define SCR_CORE_ENABLE(x)	(1 << (21 + x))
#define SCR_CORE_RESET(x)	(1 << (13 + x))

struct imxsrc_softc {
	struct device		sc_dev;
	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;
};

struct imxsrc_softc *imxsrc_sc;

void imxsrc_attach(struct device *, struct device *, void *);
void imxsrc_reset_cpu(int);
void imxsrc_wait_for_cpu(int);

struct cfattach	imxsrc_ca = {
	sizeof (struct imxsrc_softc), NULL, imxsrc_attach
};

struct cfdriver imxsrc_cd = {
	NULL, "imxsrc", DV_DULL
};

void
imxsrc_attach(struct device *parent, struct device *self, void *args)
{
	struct imx_attach_args *ia = args;
	struct imxsrc_softc *sc = (struct imxsrc_softc *) self;
	int i;

	sc->sc_iot = ia->ia_iot;
	if (bus_space_map(sc->sc_iot, ia->ia_dev->mem[0].addr,
	    ia->ia_dev->mem[0].size, 0, &sc->sc_ioh))
		panic("imxsrc_attach: bus_space_map failed!");

	printf("\n");
	imxsrc_sc = sc;

	for (i = 1; i < ncpusfound; i++) {
		config_found(self, NULL, NULL);
		imxsrc_reset_cpu(i);
		imxsrc_wait_for_cpu(i);
		delay(1000);
	}
}

void
imxsrc_reset_cpu(int id)
{
	if (imxsrc_sc == NULL)
		return;

	paddr_t pa = 0;
	if (!pmap_extract(pmap_kernel(), (vaddr_t)armv7_mpstart, &pa))
		panic("%s: No physical address for armv7_mpstart!", __func__);

	/* this is where I want you to start */
	bus_space_write_4(imxsrc_sc->sc_iot, imxsrc_sc->sc_ioh,
	    GPR1 + id * 2 * sizeof(uint32_t), (uint32_t)pa);

	/* we don't need no argument */
	bus_space_write_4(imxsrc_sc->sc_iot, imxsrc_sc->sc_ioh,
	    GPR2 + id * 2 * sizeof(uint32_t), 0);

	/* enable ALL THE cores! */
	bus_space_write_4(imxsrc_sc->sc_iot, imxsrc_sc->sc_ioh, SCR,
	    bus_space_read_4(imxsrc_sc->sc_iot, imxsrc_sc->sc_ioh, SCR)
	    | SCR_CORE_ENABLE(id) | SCR_CORE_RESET(id));

	delay(100);

	/* Reset settings, as they persist over reboots. */
	bus_space_write_4(imxsrc_sc->sc_iot, imxsrc_sc->sc_ioh,
	    GPR1 + id * 2 * sizeof(uint32_t), 0);
	bus_space_write_4(imxsrc_sc->sc_iot, imxsrc_sc->sc_ioh,
	    GPR2 + id * 2 * sizeof(uint32_t), 0);
}

void
imxsrc_wait_for_cpu(int id)
{
	struct cpu_info *ci;
	int i;

	/* On this system, `id' should be the one in cpu_info. */
	ci = cpu_info[id];
	if (ci == NULL)
		panic("%s: cpu_info for core %d is not allocated!",
		    __func__, id);

	/*
	 * wait for it to become ready
	 */
	for (i = 100000; (!(ci->ci_flags & CPUF_PRESENT)) && i>0;i--) {
		delay(10);
	}
	if (! (ci->ci_flags & CPUF_PRESENT)) {
		printf("%s: failed to become ready\n", ci->ci_dev->dv_xname);
#if defined(MPDEBUG) && defined(DDB)
		printf("dropping into debugger; continue from here to resume boot\n");
		Debugger();
#endif
	}

	if ((ci->ci_flags & CPUF_IDENTIFIED) == 0) {
		atomic_setbits_int(&ci->ci_flags, CPUF_IDENTIFY);

		/* wait for it to identify */
		for (i = 100000; (ci->ci_flags & CPUF_IDENTIFY) && i > 0; i--)
			delay(10);

		if (ci->ci_flags & CPUF_IDENTIFY)
			printf("%s: failed to identify\n",
			    ci->ci_dev->dv_xname);
	}
}

static int cpu_imxsrc_match(struct device *, void *, void *);
static void cpu_imxsrc_attach (struct device *, struct device *, void *);

static int
cpu_imxsrc_match(struct device *parent, void *vcf, void *aux)
{
	return (1);
}

static void
cpu_imxsrc_attach(struct device *parent, struct device *self, void *aux)
{
	cpu_attach(self);
}

struct cfattach cpu_imxsrc_ca = {
	sizeof(struct device), cpu_imxsrc_match, cpu_imxsrc_attach
};
