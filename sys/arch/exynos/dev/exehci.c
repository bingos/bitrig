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
#include <exynos/dev/exsysregvar.h>
#include <exynos/dev/expowervar.h>
#include <exynos/dev/exgpiovar.h>

#include <dev/usb/ehcireg.h>
#include <dev/usb/ehcivar.h>

/* registers */
#define USBPHY_CTRL0			0x00
#define USBPHY_TUNE0			0x04
#define HSICPHY_CTRL1			0x10
#define HSICPHY_TUNE1			0x14
#define HSICPHY_CTRL2			0x20
#define HSICPHY_TUNE2			0x24
#define EHCI_CTRL			0x30
#define OHCI_CTRL			0x34
#define USBOTG_SYS			0x38
#define USBOTG_TUNE			0x40

/* bits and bytes */
#define CLK_24MHZ			5

#define HOST_CTRL0_PHYSWRSTALL		(1 << 31)
#define HOST_CTRL0_COMMONON_N		(1 << 9)
#define HOST_CTRL0_SIDDQ		(1 << 6)
#define HOST_CTRL0_FORCESLEEP		(1 << 5)
#define HOST_CTRL0_FORCESUSPEND		(1 << 4)
#define HOST_CTRL0_WORDINTERFACE	(1 << 3)
#define HOST_CTRL0_UTMISWRST		(1 << 2)
#define HOST_CTRL0_LINKSWRST		(1 << 1)
#define HOST_CTRL0_PHYSWRST		(1 << 0)

#define HOST_CTRL0_FSEL_MASK		(7 << 16)

#define EHCI_CTRL_ENAINCRXALIGN		(1 << 29)
#define EHCI_CTRL_ENAINCR4		(1 << 28)
#define EHCI_CTRL_ENAINCR8		(1 << 27)
#define EHCI_CTRL_ENAINCR16		(1 << 26)

void	exehci_attach(struct device *, struct device *, void *);
int	exehci_detach(struct device *, int);

struct exehci_softc {
	struct ehci_softc	sc;
	void			*sc_ih;
	bus_space_handle_t	ph_ioh;
};

struct cfattach exehci_ca = {
	sizeof (struct exehci_softc), NULL, exehci_attach,
	exehci_detach, NULL
};

void	exehci_setup(struct exehci_softc *);

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
		ea->ea_dev->mem[1].size, 0, &sc->ph_ioh)) {
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

	exehci_setup(sc);

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
	bus_space_unmap(sc->sc.iot, sc->ph_ioh, ea->ea_dev->mem[1].addr);
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

void
exehci_setup(struct exehci_softc *sc)
{
	uint32_t val;

	exsysreg_usbhost_mode(1);
	expower_usbhost_phy_ctrl(1);

	delay(10000);

	/* Setting up host and device simultaneously */
	val = bus_space_read_4(sc->sc.iot, sc->ph_ioh, USBPHY_CTRL0);
	val &= ~(HOST_CTRL0_FSEL_MASK |
		 HOST_CTRL0_COMMONON_N |
		 /* HOST Phy setting */
		 HOST_CTRL0_PHYSWRST |
		 HOST_CTRL0_PHYSWRSTALL |
		 HOST_CTRL0_SIDDQ |
		 HOST_CTRL0_FORCESUSPEND |
		 HOST_CTRL0_FORCESLEEP);
	val |= (/* Setting up the ref freq */
		 CLK_24MHZ << 16 |
		 /* HOST Phy setting */
		 HOST_CTRL0_LINKSWRST |
		 HOST_CTRL0_UTMISWRST);
	bus_space_write_4(sc->sc.iot, sc->ph_ioh, USBPHY_CTRL0, val);
	delay(10000);
	bus_space_write_4(sc->sc.iot, sc->ph_ioh, USBPHY_CTRL0,
	    bus_space_read_4(sc->sc.iot, sc->ph_ioh, USBPHY_CTRL0) &
		~(HOST_CTRL0_LINKSWRST | HOST_CTRL0_UTMISWRST));
	delay(20000);

	/* EHCI Ctrl setting */
	bus_space_write_4(sc->sc.iot, sc->ph_ioh, EHCI_CTRL,
	    bus_space_read_4(sc->sc.iot, sc->ph_ioh, EHCI_CTRL) |
		EHCI_CTRL_ENAINCRXALIGN |
		EHCI_CTRL_ENAINCR4 |
		EHCI_CTRL_ENAINCR8 |
		EHCI_CTRL_ENAINCR16);

	/* HSIC USB Hub initialization. */
	if (1) {
		exgpio_set_dir(0xd0, EXGPIO_DIR_OUT);
		exgpio_clear_bit(0xd0);
		delay(1000);
		exgpio_set_bit(0xd0);
		delay(5000);

		val = bus_space_read_4(sc->sc.iot, sc->ph_ioh, HSICPHY_CTRL1);
		val &= ~(HOST_CTRL0_SIDDQ |
			 HOST_CTRL0_FORCESLEEP |
			 HOST_CTRL0_FORCESUSPEND);
		bus_space_write_4(sc->sc.iot, sc->ph_ioh, HSICPHY_CTRL1, val);
		val |= HOST_CTRL0_PHYSWRST;
		bus_space_write_4(sc->sc.iot, sc->ph_ioh, HSICPHY_CTRL1, val);
		delay(1000);
		val &= ~HOST_CTRL0_PHYSWRST;
		bus_space_write_4(sc->sc.iot, sc->ph_ioh, HSICPHY_CTRL1, val);
	}

	/* PHY clock and power setup time */
	delay(50000);
}
