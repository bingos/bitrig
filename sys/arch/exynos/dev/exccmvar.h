/* $OpenBSD: omgpiovar.h,v 1.1 2009/05/08 03:13:26 drahn Exp $ */
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

#ifndef EXCCMVAR_H
#define EXCCMVAR_H

unsigned int exccm_get_usdhx(int x);
unsigned int exccm_get_fecclk(void);
unsigned int exccm_get_uartclk(void);
unsigned int exccm_get_ipg_perclk(void);
unsigned int exccm_get_ahbclk(void);
void exccm_enable_i2c(int x);
void exccm_enable_usboh3(void);
void exccm_disable_usb2_chrg_detect(void);
void exccm_enable_pll_usb2(void);

#endif /* EXCCMVAR_H */
