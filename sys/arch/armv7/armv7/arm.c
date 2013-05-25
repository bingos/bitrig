/* $OpenBSD: omap.c,v 1.3 2011/11/15 23:01:11 drahn Exp $ */
/*
 * Copyright (c) 2005,2008 Dale Rahn <drahn@openbsd.com>
 * Copyright (c) 2013 Patrick Wildt <patrick@blueri.se>
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
#include <sys/malloc.h>
#include <sys/reboot.h>
#define _ARM32_BUS_DMA_PRIVATE
#include <machine/bus.h>
#include <armv7/armv7/armv7var.h>

struct arm32_bus_dma_tag arm_bus_dma_tag = {
	0,
	0,
	NULL,
	_bus_dmamap_create,
	_bus_dmamap_destroy,
	_bus_dmamap_load,
	_bus_dmamap_load_mbuf,
	_bus_dmamap_load_uio,
	_bus_dmamap_load_raw,
	_bus_dmamap_unload,
	_bus_dmamap_sync,
	_bus_dmamem_alloc,
	_bus_dmamem_free,
	_bus_dmamem_map,
	_bus_dmamem_unmap,
	_bus_dmamem_mmap,
};

struct arm_softc {
	struct device sc_dv;
};

int	arm_match(struct device *, void *, void *);
void	arm_attach(struct device *, struct device *, void *);
int	arm_submatch(struct device *, void *, void *);

struct cfattach arm_ca = {
	sizeof(struct arm_softc), arm_match, arm_attach, NULL,
	config_activate_children
};

struct cfdriver arm_cd = {
	NULL, "arm", DV_DULL
};

int
arm_match(struct device *parent, void *cfdata, void *aux)
{
	return (1);
}

void
arm_attach(struct device *parent, struct device *self, void *aux)
{
	struct board_dev *bd;

	printf(": %s\n", board->bd_name);

	/* Directly configure on-board devices (dev* in config file). */
	for (bd = board->bd_dev; bd->name != NULL; bd++) {
		struct arm_dev *id = arm_find_dev(bd->name, bd->unit);
		struct arm_attach_args aa;

		if (id == NULL)
			printf("%s: device %s unit %d not found\n",
			    self->dv_xname, bd->name, bd->unit);

		memset(&aa, 0, sizeof(aa));
		aa.aa_dev = id;
		aa.aa_iot = &armv7_bs_tag;
		aa.aa_dmat = &arm_bus_dma_tag;

		if (config_found_sm(self, &aa, NULL, arm_submatch) == NULL)
			printf("%s: device %s unit %d not configured\n",
			    self->dv_xname, bd->name, bd->unit);
	}
}

/*
 * We do direct configuration of devices on this SoC "bus", so we
 * never call the child device's match function at all (it can be
 * NULL in the struct cfattach).
 */
int
arm_submatch(struct device *parent, void *child, void *aux)
{
	struct cfdata *cf = child;
	struct arm_attach_args *aa = aux;

	if (strcmp(cf->cf_driver->cd_name, aa->aa_dev->name) == 0)
		return (1);

	/* "These are not the droids you are looking for." */
	return (0);
}

struct arm_dev *
arm_find_dev(const char *name, int unit)
{
	struct arm_dev *id;

	for (id = board->bd_soc; id->name != NULL; id++) {
		if (id->unit == unit && strcmp(id->name, name) == 0)
			return (id);
	}

	return (NULL);
}
