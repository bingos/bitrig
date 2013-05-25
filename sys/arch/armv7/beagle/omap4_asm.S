/*	$OpenBSD$	*/
/*-
 * Copyright (c) 2011
 *	Ben Gray <ben.r.gray@gmail.com>.
 * All rights reserved.
 *
 * Based heavily on the Linux OMAP secure API
 *    Copyright (C) 2010 Texas Instruments, Inc.
 *    Written by Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/asm.h>

/**
 *	omap4_smc_call - issues a secure monitor API call
 *	@r0: contains the monitor API number
 *	@r1: contains the value to set
 * 
 *	This function will send a secure monitor call to the internal rom code in
 *	the trust mode. The rom code expects that r0 will contain the value and 
 *	r12 will contain the API number, so internally the function swaps the
 *	register settings around.  The trust mode code may also alter all the cpu
 *	registers so everything (including the lr register) is saved on the stack
 *	before the call.
 *
 *	RETURNS:
 *	nothing
 */
 	.text
#if 0
	.arch	armv6z
#endif
ENTRY(omap4_smc_call)
	stmfd   sp!, {r2-r12, lr}
	mov     r12, r0
	mov     r0, r1
#if 0
	dsb
#else
	.word	0xf57ff040
#endif
#if 0
	smc     #0	/* XXX or #1 ??? */
#else
	.word	0xe1600070
#endif
	ldmfd   sp!, {r2-r12, pc}

