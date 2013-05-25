/* $OpenBSD$ */
/*
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

#include <sys/termios.h>

#define ARM_DEV_NMEM 6				/* number of memory ranges */
#define ARM_DEV_NIRQ 4				/* number of IRQs per device */

struct arm_mem {
	u_int32_t		 addr;		/* physical start address */
	u_int32_t		 size;		/* size of range in bytes */
};

struct board_dev {
	char			*name;
	int			 unit;
};

struct arm_dev {
	char			 *name;		/* driver name or made up name */
	int			 unit;		/* driver instance number or -1 */
	struct arm_mem mem[ARM_DEV_NMEM];	/* memory ranges */
	int irq[ARM_DEV_NIRQ];			/* IRQ number(s) */
};

struct arm_board {
	int			 bd_id;		/* board id */
	char			*bd_name;	/* board name */
	struct board_dev	*bd_dev;	/* board devices */
	struct arm_dev		*bd_soc;	/* soc devices */
	u_int32_t		 bd_console_addr; /* console addr */
	int (*bd_cnattach)(bus_space_tag_t iot, bus_addr_t iobase, int rate,
	    int frequency, tcflag_t cflag);
};

extern struct arm_board imx6_phyflex_board;
extern struct arm_board imx6_sabrelite_board;
extern struct arm_board omap3_beagleboard_board;
extern struct arm_board omap3_overo_board;
extern struct arm_board omap4_pandaboard_board;
extern struct arm_board **boards;

struct arm_board **armv7_devices(void);
