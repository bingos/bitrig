/*	$OpenBSD: exehci.c,v 1.10 2011/11/10 19:37:01 uwe Exp $ */
/*
 * Copyright (c) 2012-2013 Patrick Wildt <patrick@blueri.se>
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
#include <sys/device.h>
#include <sys/kernel.h>
#include <sys/rwlock.h>
#include <sys/timeout.h>

#include <machine/intr.h>
#include <machine/bus.h>

#include <dev/usb/usb.h>
#include <dev/usb/usbdi.h>
#include <dev/usb/usbdivar.h>
#include <dev/usb/usb_mem.h>

#include <exynos/dev/exvar.h>
#include <exynos/dev/exccmvar.h>
#include <exynos/dev/exgpiovar.h>

#include <dev/usb/ehcireg.h>
#include <dev/usb/ehcivar.h>

/* usb phy */
#define USBPHY_PWD			0x00
#define USBPHY_CTRL			0x30
#define USBPHY_CTRL_SET			0x34
#define USBPHY_CTRL_CLR			0x38
#define USBPHY_CTRL_TOG			0x3c

#define USBPHY_CTRL_ENUTMILEVEL2	(1 << 14)
#define USBPHY_CTRL_ENUTMILEVEL3	(1 << 15)
#define USBPHY_CTRL_CLKGATE		(1 << 30)
#define USBPHY_CTRL_SFTRST		(1 << 31)

/* ehci */
#define EHCI_USBMODE			0x68

#define EHCI_USBMODE_HOST		(3 << 0)
#define EHCI_PS_PTS_UTMI_MASK	((1 << 25) | (3 << 30))

/* usb non-core */
#define USBNC_USB_UH1_CTRL		0x04

#define USBNC_USB_UH1_CTRL_OVER_CUR_POL	(1 << 8)
#define USBNC_USB_UH1_CTRL_OVER_CUR_DIS	(1 << 7)

/* board specific */
#define EHCI_PHYFLEX_USB_H1_PWR		0
#define EHCI_PHYFLEX_USB_OTG_PWR	111

void	exehci_attach(struct device *, struct device *, void *);
int	exehci_detach(struct device *, int);

struct exehci_softc {
	struct ehci_softc	sc;
	void			*sc_ih;
	bus_space_handle_t	uh_ioh;
	bus_space_handle_t	ph_ioh;
	bus_space_handle_t	nc_ioh;
};

struct cfattach exehci_ca = {
	sizeof (struct exehci_softc), NULL, exehci_attach,
	exehci_detach, NULL
};

void
exehci_attach(struct device *parent, struct device *self, void *aux)
{
	struct exehci_softc	*sc = (struct exehci_softc *)self;
	struct ex_attach_args	*ea = aux;
	usbd_status		r;
	char *devname = sc->sc.sc_bus.bdev.dv_xname;

	sc->sc.iot = ea->ea_iot;
	sc->sc.sc_bus.dmatag = ea->ea_dmat;
	sc->sc.sc_size = ea->ea_dev->mem[0].size;

	/* Map I/O space */
	if (bus_space_map(sc->sc.iot, ea->ea_dev->mem[0].addr,
		ea->ea_dev->mem[0].size, 0, &sc->sc.ioh)) {
		printf(": cannot map mem space\n");
		goto out;
	}

	if (bus_space_map(sc->sc.iot, ea->ea_dev->mem[1].addr,
		ea->ea_dev->mem[1].size, 0, &sc->uh_ioh)) {
		printf(": cannot map mem space\n");
		goto mem0;
	}

	printf("\n");

	sc->sc_ih = arm_intr_establish(ea->ea_dev->irq[0], IPL_USB,
	    ehci_intr, &sc->sc, devname);
	if (sc->sc_ih == NULL) {
		printf(": unable to establish interrupt\n");
		goto mem1;
	}

	strlcpy(sc->sc.sc_vendor, "Exynos 5", sizeof(sc->sc.sc_vendor));
	r = ehci_init(&sc->sc);
	if (r != USBD_NORMAL_COMPLETION) {
		printf("%s: init failed, error=%d\n", devname, r);
		goto intr;
	}

	sc->sc.sc_child = config_found((void *)sc, &sc->sc.sc_bus,
	    usbctlprint);

	goto out;

intr:
	arm_intr_disestablish(sc->sc_ih);
	sc->sc_ih = NULL;
mem1:
	bus_space_unmap(sc->sc.iot, sc->uh_ioh, ea->ea_dev->mem[1].addr);
mem0:
	bus_space_unmap(sc->sc.iot, sc->sc.ioh, sc->sc.sc_size);
	sc->sc.sc_size = 0;
out:
	return;
}

int
exehci_detach(struct device *self, int flags)
{
	struct exehci_softc		*sc = (struct exehci_softc *)self;
	int				rv;

	rv = ehci_detach(&sc->sc, flags);
	if (rv)
		return (rv);

	if (sc->sc_ih != NULL) {
		arm_intr_disestablish(sc->sc_ih);
		sc->sc_ih = NULL;
	}

	if (sc->sc.sc_size) {
		bus_space_unmap(sc->sc.iot, sc->sc.ioh, sc->sc.sc_size);
		sc->sc.sc_size = 0;
	}

	return (0);
}
