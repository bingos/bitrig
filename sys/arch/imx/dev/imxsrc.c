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

	sc->sc_iot = ia->ia_iot;
	if (bus_space_map(sc->sc_iot, ia->ia_dev->mem[0].addr,
	    ia->ia_dev->mem[0].size, 0, &sc->sc_ioh))
		panic("imxsrc_attach: bus_space_map failed!");

	printf("\n");
	imxsrc_sc = sc;

	imxsrc_reset_cpu(1);
	delay(1000);
	imxsrc_reset_cpu(2);
	delay(1000);
	imxsrc_reset_cpu(3);
	delay(1000);
}

volatile u_int arm_cpu_hatched = 0;

#include <uvm/uvm_extern.h>
#include <machine/pmap.h>

void
imxsrc_reset_cpu(int id)
{
	if (imxsrc_sc == NULL)
		return;

	paddr_t pa = 0;
	if (!pmap_extract(pmap_kernel(), (vaddr_t)armv7_mpstart, &pa))
		panic("No physical address for armv7_mpstart!");

	printf("%s: powering up core %i\n", imxsrc_sc->sc_dev.dv_xname, id);

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

	/* wait for hatch */
	/*
	while(!arm_cpu_hatched)
		cpu_drain_writebuf();
	printf("cpu hatched!\n");
	*/
}
