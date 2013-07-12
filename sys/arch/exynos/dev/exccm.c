/* $OpenBSD: omdog.c,v 1.5 2011/11/15 23:01:11 drahn Exp $ */
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
#include <sys/queue.h>
#include <sys/malloc.h>
#include <sys/sysctl.h>
#include <sys/device.h>
#include <sys/evcount.h>
#include <sys/socket.h>
#include <sys/timeout.h>
#include <machine/intr.h>
#include <machine/bus.h>
#include <exynos/dev/exvar.h>

/* registers */
#define CCM_CCR		0x00
#define CCM_CCDR	0x04
#define CCM_CSR		0x08
#define CCM_CCSR	0x0c
#define CCM_CACRR	0x10
#define CCM_CBCDR	0x14
#define CCM_CBCMR	0x18
#define CCM_CSCMR1	0x1c
#define CCM_CSCMR2	0x20
#define CCM_CSCDR1	0x24
#define CCM_CS1CDR	0x28
#define CCM_CS2CDR	0x2c
#define CCM_CDCDR	0x30
#define CCM_CHSCCDR	0x34
#define CCM_CSCDR2	0x38
#define CCM_CSCDR3	0x3c
#define CCM_CSCDR4	0x40
#define CCM_CDHIPR	0x48
#define CCM_CDCR	0x4c
#define CCM_CTOR	0x50
#define CCM_CLPCR	0x54
#define CCM_CISR	0x58
#define CCM_CIMR	0x5c
#define CCM_CCOSR	0x60
#define CCM_CGPR	0x64
#define CCM_CCGR0	0x68
#define CCM_CCGR1	0x6c
#define CCM_CCGR2	0x70
#define CCM_CCGR3	0x74
#define CCM_CCGR4	0x78
#define CCM_CCGR5	0x7c
#define CCM_CCGR6	0x80
#define CCM_CCGR7	0x84
#define CCM_CMEOR	0x88

/* ANALOG */
#define CCM_ANALOG_PLL_ARM			0x4000
#define CCM_ANALOG_PLL_ARM_SET			0x4004
#define CCM_ANALOG_PLL_ARM_CLR			0x4008
#define CCM_ANALOG_PLL_USB1			0x4010
#define CCM_ANALOG_PLL_USB1_SET			0x4014
#define CCM_ANALOG_PLL_USB1_CLR			0x4018
#define CCM_ANALOG_PLL_USB2			0x4020
#define CCM_ANALOG_PLL_USB2_SET			0x4024
#define CCM_ANALOG_PLL_USB2_CLR			0x4028
#define CCM_ANALOG_PLL_SYS			0x4030
#define CCM_ANALOG_USB2_CHRG_DETECT		0x4210
#define CCM_ANALOG_USB2_CHRG_DETECT_SET		0x4214
#define CCM_ANALOG_USB2_CHRG_DETECT_CLR		0x4218
#define CCM_ANALOG_DIGPROG			0x4260
#define CCM_ANALOG_PLL_ENET			0x40e0
#define CCM_ANALOG_PLL_ENET_SET			0x40e4
#define CCM_ANALOG_PLL_ENET_CLR			0x40e8
#define CCM_ANALOG_PFD_480			0x40f0
#define CCM_ANALOG_PFD_480_SET			0x40f4
#define CCM_ANALOG_PFD_480_CLR			0x40f8
#define CCM_ANALOG_PFD_528			0x4100
#define CCM_ANALOG_PFD_528_SET			0x4104
#define CCM_ANALOG_PFD_528_CLR			0x4108
#define CCM_PMU_MISC1				0x4160

/* bits and bytes */
#define CCM_CCSR_PLL3_SW_CLK_SEL		(1 << 0)
#define CCM_CCSR_PLL2_SW_CLK_SEL		(1 << 1)
#define CCM_CCSR_PLL1_SW_CLK_SEL		(1 << 2)
#define CCM_CCSR_STEP_SEL			(1 << 8)
#define CCM_CBCDR_IPG_PODF_SHIFT		8
#define CCM_CBCDR_IPG_PODF_MASK			0x3
#define CCM_CBCDR_AHB_PODF_SHIFT		10
#define CCM_CBCDR_AHB_PODF_MASK			0x7
#define CCM_CBCDR_PERIPH_CLK_SEL_SHIFT		25
#define CCM_CBCDR_PERIPH_CLK_SEL_MASK		0x1
#define CCM_CBCMR_PERIPH_CLK2_SEL_SHIFT		12
#define CCM_CBCMR_PERIPH_CLK2_SEL_MASK		0x3
#define CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT	18
#define CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK	0x3
#define CCM_CSCDR1_USDHCx_CLK_SEL_SHIFT(x)	((x) + 15)
#define CCM_CSCDR1_USDHCx_CLK_SEL_MASK		0x1
#define CCM_CSCDR1_USDHCx_PODF_MASK		0x7
#define CCM_CSCDR1_UART_PODF_MASK		0x7
#define CCM_CCGR1_ENET				(3 << 10)
#define CCM_CCGR2_I2C(x)			(3 << (6 + 2*x))
#define CCM_CCGR4_125M_PCIE			(3 << 0)
#define CCM_CCGR5_100M_SATA			(3 << 4)
#define CCM_CCGR6_USBOH3			(3 << 0)
#define CCM_CSCMR1_PERCLK_CLK_SEL_MASK		0x1f
#define CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK	0x7f
#define CCM_ANALOG_PLL_ARM_BYPASS		(1 << 16)
#define CCM_ANALOG_PLL_USB2_DIV_SELECT_MASK	0x1
#define CCM_ANALOG_PLL_USB2_EN_USB_CLKS		(1 << 6)
#define CCM_ANALOG_PLL_USB2_POWER		(1 << 12)
#define CCM_ANALOG_PLL_USB2_ENABLE		(1 << 13)
#define CCM_ANALOG_PLL_USB2_BYPASS		(1 << 16)
#define CCM_ANALOG_PLL_USB2_LOCK		(1 << 31)
#define CCM_ANALOG_PLL_SYS_DIV_SELECT_MASK	0x1
#define CCM_ANALOG_USB2_CHRG_DETECT_CHK_CHRG_B	(1 << 19)
#define CCM_ANALOG_USB2_CHRG_DETECT_EN_B	(1 << 20)
#define CCM_ANALOG_DIGPROG_MINOR_MASK		0xff
#define CCM_ANALOG_PLL_ENET_DIV_125M		(1 << 11)
#define CCM_ANALOG_PLL_ENET_POWERDOWN		(1 << 12)
#define CCM_ANALOG_PLL_ENET_ENABLE		(1 << 13)
#define CCM_ANALOG_PLL_ENET_BYPASS		(1 << 16)
#define CCM_ANALOG_PLL_ENET_125M_PCIE		(1 << 19)
#define CCM_ANALOG_PLL_ENET_100M_SATA		(1 << 20)
#define CCM_ANALOG_PLL_ENET_LOCK		(1 << 31)
#define CCM_ANALOG_PFD_480_PFDx_FRAC(x, y)	(((x) >> ((y) << 3)) & 0x3f)
#define CCM_ANALOG_PFD_528_PFDx_FRAC(x, y)	(((x) >> ((y) << 3)) & 0x3f)
#define CCM_PMU_MISC1_LVDSCLK1_CLK_SEL_SATA	(0xB << 0)
#define CCM_PMU_MISC1_LVDSCLK1_CLK_SEL_MASK	(0x1f << 0)
#define CCM_PMU_MISC1_LVDSCLK1_OBEN		(1 << 10)
#define CCM_PMU_MISC1_LVDSCLK1_IBEN		(1 << 12)

#define HCLK_FREQ				24000
#define PLL3_80M				80000

#define HREAD4(sc, reg)							\
	(bus_space_read_4((sc)->sc_iot, (sc)->sc_ioh, (reg)))
#define HWRITE4(sc, reg, val)						\
	bus_space_write_4((sc)->sc_iot, (sc)->sc_ioh, (reg), (val))
#define HSET4(sc, reg, bits)						\
	HWRITE4((sc), (reg), HREAD4((sc), (reg)) | (bits))
#define HCLR4(sc, reg, bits)						\
	HWRITE4((sc), (reg), HREAD4((sc), (reg)) & ~(bits))

struct exccm_softc {
	struct device		sc_dev;
	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;
};

enum clocks {
	/* OSC */
	OSC,		/* 24 MHz OSC */

	/* PLLs */
	ARM_PLL1,	/* ARM core PLL */
	SYS_PLL2,	/* System PLL: 528 MHz */
	USB1_PLL3,	/* OTG USB PLL: 480 MHz */
	USB2_PLL,	/* Host USB PLL: 480 MHz */
	AUD_PLL4,	/* Audio PLL */
	VID_PLL5,	/* Video PLL */
	ENET_PLL6,	/* ENET PLL */
	MLB_PLL,	/* MLB PLL */

	/* SYS_PLL2 PFDs */
	SYS_PLL2_PFD0,	/* 352 MHz */
	SYS_PLL2_PFD1,	/* 594 MHz */
	SYS_PLL2_PFD2,	/* 396 MHz */

	/* USB1_PLL3 PFDs */
	USB1_PLL3_PFD0,	/* 720 MHz */
	USB1_PLL3_PFD1,	/* 540 MHz */
	USB1_PLL3_PFD2,	/* 508.2 MHz */
	USB1_PLL3_PFD3,	/* 454.7 MHz */
};

struct exccm_softc *exccm_sc;

void exccm_attach(struct device *parent, struct device *self, void *args);
int exccm_cpuspeed(int *);
unsigned int exccm_decode_pll(enum clocks, unsigned int);
unsigned int exccm_get_pll2_pfd(unsigned int);
unsigned int exccm_get_pll3_pfd(unsigned int);
unsigned int exccm_get_armclk(void);
void exccm_armclk_set_parent(enum clocks);
void exccm_armclk_set_freq(unsigned int);
unsigned int exccm_get_usdhx(int x);
unsigned int exccm_get_periphclk(void);
unsigned int exccm_get_fecclk(void);
unsigned int exccm_get_ahbclk(void);
unsigned int exccm_get_ipgclk(void);
unsigned int exccm_get_ipg_perclk(void);
unsigned int exccm_get_uartclk(void);
void exccm_enable_i2c(int x);
void exccm_enable_usboh3(void);
void exccm_disable_usb2_chrg_detect(void);
void exccm_enable_pll_usb2(void);

struct cfattach	exccm_ca = {
	sizeof (struct exccm_softc), NULL, exccm_attach
};

struct cfdriver exccm_cd = {
	NULL, "exccm", DV_DULL
};

void
exccm_attach(struct device *parent, struct device *self, void *args)
{
	struct ex_attach_args *ea = args;
	struct exccm_softc *sc = (struct exccm_softc *) self;

	exccm_sc = sc;
	sc->sc_iot = ea->ea_iot;
	if (bus_space_map(sc->sc_iot, ea->ea_dev->mem[0].addr,
	    ea->ea_dev->mem[0].size, 0, &sc->sc_ioh))
		panic("exccm_attach: bus_space_map failed!");

	printf(": Exynos 5 rev 1.%d CPU freq: %d MHz",
	    HREAD4(sc, CCM_ANALOG_DIGPROG) & CCM_ANALOG_DIGPROG_MINOR_MASK,
	    exccm_get_armclk() / 1000);

	printf("\n");

	cpu_cpuspeed = exccm_cpuspeed;
}

int
exccm_cpuspeed(int *freq)
{
	*freq = exccm_get_armclk() / 1000;
	return (0);
}

unsigned int
exccm_decode_pll(enum clocks pll, unsigned int freq)
{
	struct exccm_softc *sc = exccm_sc;
	uint32_t div;

	switch (pll) {
	case ARM_PLL1:
		if (HREAD4(sc, CCM_ANALOG_PLL_ARM)
		    & CCM_ANALOG_PLL_ARM_BYPASS)
			return freq;
		div = HREAD4(sc, CCM_ANALOG_PLL_ARM)
		    & CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK;
		return (freq * div) / 2;
	case SYS_PLL2:
		div = HREAD4(sc, CCM_ANALOG_PLL_SYS)
		    & CCM_ANALOG_PLL_SYS_DIV_SELECT_MASK;
		return freq * (20 + (div << 1));
	case USB1_PLL3:
		div = HREAD4(sc, CCM_ANALOG_PLL_USB2)
		    & CCM_ANALOG_PLL_USB2_DIV_SELECT_MASK;
		return freq * (20 + (div << 1));
	default:
		return 0;
	}
}

unsigned int
exccm_get_pll2_pfd(unsigned int pfd)
{
	struct exccm_softc *sc = exccm_sc;

	return exccm_decode_pll(SYS_PLL2, HCLK_FREQ) * 18
	    / CCM_ANALOG_PFD_528_PFDx_FRAC(HREAD4(sc, CCM_ANALOG_PFD_528), pfd);
}

unsigned int
exccm_get_pll3_pfd(unsigned int pfd)
{
	struct exccm_softc *sc = exccm_sc;

	return exccm_decode_pll(USB1_PLL3, HCLK_FREQ) * 18
	    / CCM_ANALOG_PFD_480_PFDx_FRAC(HREAD4(sc, CCM_ANALOG_PFD_480), pfd);
}

unsigned int
exccm_get_armclk()
{
	struct exccm_softc *sc = exccm_sc;

	uint32_t ccsr = HREAD4(sc, CCM_CCSR);

	if (!(ccsr & CCM_CCSR_PLL1_SW_CLK_SEL))
		return exccm_decode_pll(ARM_PLL1, HCLK_FREQ);
	else if (ccsr & CCM_CCSR_STEP_SEL)
		return exccm_get_pll2_pfd(2);
	else
		return HCLK_FREQ;
}

void
exccm_armclk_set_parent(enum clocks clock)
{
	struct exccm_softc *sc = exccm_sc;

	switch (clock)
	{
	case ARM_PLL1:
		/* jump onto pll1 */
		HCLR4(sc, CCM_CCSR, CCM_CCSR_PLL1_SW_CLK_SEL);
		/* put step clk on OSC, power saving */
		HCLR4(sc, CCM_CCSR, CCM_CCSR_STEP_SEL);
		break;
	case OSC:
		/* put step clk on OSC */
		HCLR4(sc, CCM_CCSR, CCM_CCSR_STEP_SEL);
		/* jump onto step clk */
		HSET4(sc, CCM_CCSR, CCM_CCSR_PLL1_SW_CLK_SEL);
		break;
	case SYS_PLL2_PFD2:
		/* put step clk on pll2-pfd2 400 MHz */
		HSET4(sc, CCM_CCSR, CCM_CCSR_STEP_SEL);
		/* jump onto step clk */
		HSET4(sc, CCM_CCSR, CCM_CCSR_PLL1_SW_CLK_SEL);
		break;
	default:
		panic("%s: parent not possible for arm clk", __func__);
	}
}

void
exccm_armclk_set_freq(unsigned int freq)
{
	if (freq > 1296000 || freq < 648000)
		panic("%s: frequency must be between 648MHz and 1296MHz!",
		    __func__);
}

unsigned int
exccm_get_usdhx(int x)
{
	struct exccm_softc *sc = exccm_sc;

	uint32_t cscmr1 = HREAD4(sc, CCM_CSCMR1);
	uint32_t cscdr1 = HREAD4(sc, CCM_CSCDR1);
	uint32_t podf, clkroot;

	// Odd bitsetting. Damn you.
	if (x == 1)
		podf = ((cscdr1 >> 11) & CCM_CSCDR1_USDHCx_PODF_MASK);
	else
		podf = ((cscdr1 >> (10 + 3*x)) & CCM_CSCDR1_USDHCx_PODF_MASK);

	if (cscmr1 & (1 << CCM_CSCDR1_USDHCx_CLK_SEL_SHIFT(x)))
		clkroot = exccm_get_pll2_pfd(0); // 352 MHz
	else
		clkroot = exccm_get_pll2_pfd(2); // 396 MHz

	return clkroot / (podf + 1);
}

unsigned int
exccm_get_uartclk()
{
	struct exccm_softc *sc = exccm_sc;

	uint32_t clkroot = PLL3_80M;
	uint32_t podf = HREAD4(sc, CCM_CSCDR1) & CCM_CSCDR1_UART_PODF_MASK;

	return clkroot / (podf + 1);
}

unsigned int
exccm_get_periphclk()
{
	struct exccm_softc *sc = exccm_sc;

	if ((HREAD4(sc, CCM_CBCDR) >> CCM_CBCDR_PERIPH_CLK_SEL_SHIFT)
		    & CCM_CBCDR_PERIPH_CLK_SEL_MASK) {
		switch((HREAD4(sc, CCM_CBCMR)
		    >> CCM_CBCMR_PERIPH_CLK2_SEL_SHIFT) & CCM_CBCMR_PERIPH_CLK2_SEL_MASK) {
		case 0:
			return exccm_decode_pll(USB1_PLL3, HCLK_FREQ);
		case 1:
		case 2:
			return HCLK_FREQ;
		default:
			return 0;
		}
	
	} else {
		switch((HREAD4(sc, CCM_CBCMR)
		    >> CCM_CBCMR_PRE_PERIPH_CLK_SEL_SHIFT) & CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK) {
		default:
		case 0:
			return exccm_decode_pll(SYS_PLL2, HCLK_FREQ);
		case 1:
			return exccm_get_pll2_pfd(2); // 396 MHz
		case 2:
			return exccm_get_pll2_pfd(0); // 352 MHz
		case 3:
			return exccm_get_pll2_pfd(2) / 2; // 198 MHz
		}
	}
}

unsigned int
exccm_get_fecclk()
{
	struct exccm_softc *sc = exccm_sc;
	uint32_t div = 0;

	switch (HREAD4(sc, CCM_ANALOG_PLL_ENET) & 0x3)
	{
		case 0:
			div = 20;
		case 1:
			div = 10;
		case 2:
			div = 5;
		case 3:
			div = 4;
	}

	return 500000 / div ;
}

unsigned int
exccm_get_ahbclk()
{
	struct exccm_softc *sc = exccm_sc;
	uint32_t ahb_podf;

	ahb_podf = (HREAD4(sc, CCM_CBCDR) >> CCM_CBCDR_AHB_PODF_SHIFT)
	    & CCM_CBCDR_AHB_PODF_MASK;
	return exccm_get_periphclk() / (ahb_podf + 1);
}

unsigned int
exccm_get_ipgclk()
{
	struct exccm_softc *sc = exccm_sc;
	uint32_t ipg_podf;

	ipg_podf = (HREAD4(sc, CCM_CBCDR) >> CCM_CBCDR_IPG_PODF_SHIFT)
	    & CCM_CBCDR_IPG_PODF_MASK;
	return exccm_get_ahbclk() / (ipg_podf + 1);
}

unsigned int
exccm_get_ipg_perclk()
{
	struct exccm_softc *sc = exccm_sc;
	uint32_t ipg_podf;

	ipg_podf = HREAD4(sc, CCM_CSCMR1) & CCM_CSCMR1_PERCLK_CLK_SEL_MASK;

	return exccm_get_ipgclk() / (ipg_podf + 1);
}

void
exccm_enable_i2c(int x)
{
	struct exccm_softc *sc = exccm_sc;

	HSET4(sc, CCM_CCGR2, CCM_CCGR2_I2C(x));
}

void
exccm_enable_usboh3(void)
{
	struct exccm_softc *sc = exccm_sc;

	HSET4(sc, CCM_CCGR6, CCM_CCGR6_USBOH3);
}

void
exccm_disable_usb2_chrg_detect(void)
{
	struct exccm_softc *sc = exccm_sc;

	HWRITE4(sc, CCM_ANALOG_USB2_CHRG_DETECT_SET,
	      CCM_ANALOG_USB2_CHRG_DETECT_CHK_CHRG_B
	    | CCM_ANALOG_USB2_CHRG_DETECT_EN_B);
}

void
exccm_enable_pll_usb2(void)
{
	struct exccm_softc *sc = exccm_sc;

	HWRITE4(sc, CCM_ANALOG_PLL_USB2_CLR, CCM_ANALOG_PLL_USB2_BYPASS);

	HWRITE4(sc, CCM_ANALOG_PLL_USB2_SET,
	      CCM_ANALOG_PLL_USB2_ENABLE
	    | CCM_ANALOG_PLL_USB2_POWER
	    | CCM_ANALOG_PLL_USB2_EN_USB_CLKS);
}
