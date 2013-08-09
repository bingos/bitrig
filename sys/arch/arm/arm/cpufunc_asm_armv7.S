/* $OpenBSD: cpufunc_asm_armv7.S,v 1.6 2013/03/30 01:30:30 patrick Exp $ */
/*
 * Copyright (c) 2008 Dale Rahn <drahn@openbsd.org>
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

#include <arm/armreg.h>
#include <machine/asm.h>

#define	DSB	.long	0xf57ff04f
#define	ISB	.long	0xf57ff06f
#define	WFI	.long	0xe320f003

ENTRY(armv7_cpu_sleep)
	WFI
	mov	pc, lr

ENTRY(armv7_drain_writebuf)
	DSB
	ISB
	mov	pc, lr

/*
 * Function to read the MPCore base address
 */
ENTRY(armv7_periphbase)
	mrc	p15, 4, r0, c15, c0, 0
	mov	pc, lr

/*
 * Functions to set the MMU Translation Table Base register
 */
ENTRY(armv7_setttb)
	mcr	p15, 0, r0, c7, c5, 0	/* Flush I cache */
	mcr	p15, 0, r0, c7, c5, 6	/* Flush BP cache */
	DSB
	ISB

	mcr	p15, 0, r0, c2, c0, 0	/* load new TTB */
	mcr	p15, 0, r0, c8, c7, 0	/* invalidate I+D TLBs */
	DSB
	ISB

	mov	pc, lr

/*
 * TLB functions
 */
ENTRY(armv7_tlb_flushID_SE)
	mcr	p15, 0, r0, c8, c6, 1	/* flush D tlb single entry */
	mcr	p15, 0, r0, c8, c5, 1	/* flush I tlb single entry */
	mcr	p15, 0, r0, c7, c5, 7	/* flush va from BP */
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_tlb_flushI_SE)
	mcr	p15, 0, r0, c8, c5, 1	/* flush I tlb single entry */
	mcr	p15, 0, r0, c7, c5, 7	/* flush va from BP */
	DSB
	ISB
	mov	pc, lr

/*
 * TLB functions
 */
ENTRY(armv7_tlb_flushID)
	mcr	p15, 0, r0, c8, c7, 0	/* flush I+D tlb */
	mcr	p15, 0, r0, c7, c5, 6	/* Flush BP cache */
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_tlb_flushI)
	mcr	p15, 0, r0, c8, c5, 0	/* flush I tlb */
	mcr	p15, 0, r0, c7, c5, 6	/* Flush BP cache */
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_tlb_flushD)
	mcr	p15, 0, r0, c8, c6, 0	/* flush D tlb */
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_tlb_flushD_SE)
	mcr	p15, 0, r0, c8, c6, 1	/* flush D tlb single entry */
	DSB
	ISB
	mov	pc, lr


/*
 * Cache operations.  For the entire cache we use the set/index
 * operations.
 */
	s_max	.req r0
	i_max	.req r1
	s_inc	.req r2
	i_inc	.req r3
ENTRY(armv7_icache_sync_range)
	ldr	ip, .Larmv7_line_size
	cmp	r1, #0x8000
	movcs	r1, #0x8000	/* XXX needs to match cache size... */
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c11, 1	/* Clean D cache SE with VA to PoU */
	mcr	p15, 0, r0, c7, c5, 1	/* Invalidate I cache SE with VA */
	add	r0, r0, ip
	subs	r1, r1, ip
	bhi	1b
	mcr	p15, 0, r0, c7, c5, 6	/* Flush BP cache */
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_icache_sync_all)
.Larmv7_icache_sync_all:
	/*
	 * We assume that the code here can never be out of sync with the
	 * dcache, so that we can safely flush the Icache and fall through
	 * into the Dcache cleaning code.
	 */
	mcr	p15, 0, r0, c7, c5, 0	/* Flush I cache */
	mcr	p15, 0, r0, c7, c5, 6	/* Flush BP cache */
	ISB
	mov	pc, lr

.Larmv7_line_size:
	.word	_C_LABEL(arm_pdcache_line_size)

ENTRY(armv7_dcache_wb_range)
	ldr	ip, .Larmv7_line_size
	cmp	r1, #0x8000
	movcs	r1, #0x8000	/* XXX needs to match cache size... */
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c10, 1	/* Clean D cache SE with VA */
	add	r0, r0, ip
	subs	r1, r1, ip
	bhi	1b
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_idcache_wbinv_range)
	ldr	ip, .Larmv7_line_size
	cmp	r1, #0x8000
	movcs	r1, #0x8000	/* XXX needs to match cache size... */
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c11, 1	/* Clean D cache SE with VA to PoU */
	mcr	p15, 0, r0, c7, c5, 1	/* Invalidate I cache SE with VA */
	mcr	p15, 0, r0, c7, c14, 1	/* Purge D cache SE with VA */
	add	r0, r0, ip
	subs	r1, r1, ip
	bhi	1b
	mcr	p15, 0, r0, c7, c5, 6	/* Flush BP cache */
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_dcache_wbinv_range)
	ldr	ip, .Larmv7_line_size
	cmp	r1, #0x8000
	movcs	r1, #0x8000	/* XXX needs to match cache size... */
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c11, 1	/* Clean D cache SE with VA to PoU */
	mcr	p15, 0, r0, c7, c5, 1	/* Invalidate I cache SE with VA */
	mcr	p15, 0, r0, c7, c14, 1	/* Purge D cache SE with VA */
	add	r0, r0, ip
	subs	r1, r1, ip
	bhi	1b
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_dcache_inv_range)
	ldr	ip, .Larmv7_line_size
	cmp	r1, #0x8000
	movcs	r1, #0x8000	/* XXX needs to match cache size... */
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c11, 1	/* Clean D cache SE with VA to PoU */
	mcr	p15, 0, r0, c7, c5, 1	/* Invalidate I cache SE with VA */
	mcr	p15, 0, r0, c7, c6, 1	/* Invalidate D cache SE with VA */
	add	r0, r0, ip
	subs	r1, r1, ip
	bhi	1b
	DSB
	ISB
	mov	pc, lr

ENTRY(armv7_icache_inv_all)
	mov	r0, #0
	mcr	p15, 2, r0, c0, c0, 0	@ set cache level to L1
	mrc	p15, 1, r0, c0, c0, 0	@ read CCSIDR

	ubfx	r2, r0, #13, #15	@ get num sets - 1 from CCSIDR
	ubfx	r3, r0, #3, #10		@ get numways - 1 from CCSIDR
	clz	r1, r3			@ number of bits to MSB of way
	lsl	r3, r3, r1		@ shift into position
	mov	ip, #1			@
	lsl	ip, ip, r1		@ ip now contains the way decr

	ubfx	r0, r0, #0, #3		@ get linesize from CCSIDR
	add	r0, r0, #4		@ apply bias
	lsl	r2, r2, r0		@ shift sets by log2(linesize)
	add	r3, r3, r2		@ merge numsets - 1 with numways - 1
	sub	ip, ip, r2		@ subtract numsets - 1 from way decr
	mov	r1, #1
	lsl	r1, r1, r0		@ r1 now contains the set decr
	mov	r2, ip			@ r2 now contains set way decr

	/* r3 = ways/sets, r2 = way decr, r1 = set decr, r0 and ip are free */
1:	mcr	p15, 0, r3, c7, c6, 2	@ invalidate line
	movs	r0, r3			@ get current way/set
	beq	2f			@ at 0 means we are done.
	movs	r0, r0, lsl #10		@ clear way bits leaving only set bits
	subne	r3, r3, r1		@ non-zero?, decrement set #
	subeq	r3, r3, r2		@ zero?, decrement way # and restore set count
	b	1b

2:	DSB				@ wait for stores to finish
	mov	r0, #0			@ and ...
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate L1 cache
	ISB				@ instruction sync barrier
	bx	lr			@ return

ENTRY(armv7_dcache_inv_all)
	mrc	p15, 1, r0, c0, c0, 1	@ read CLIDR
	ands	r3, r0, #0x07000000
	beq	.Ldone_inv
	lsr	r3, r3, #23		@ left align loc (low 4 bits)

	mov	r1, #0
.Lstart_inv:
	add	r2, r3, r3, lsr #1	@ r2 = level * 3 / 2
	mov	r1, r0, lsr r2		@ r1 = cache type
	and	r1, r1, #7
	cmp	r1, #2			@ is it data or i&d?
	blt	.Lnext_level_inv	@ nope, skip level

	mcr	p15, 2, r3, c0, c0, 0	@ select cache level
	ISB
	mrc	p15, 1, r0, c0, c0, 0	@ read CCSIDR

	ubfx	ip, r0, #0, #3		@ get linesize from CCSIDR
	add	ip, ip, #4		@ apply bias
	ubfx	r2, r0, #13, #15	@ get numsets - 1 from CCSIDR
	lsl	r2, r2, ip		@ shift to set position
	orr	r3, r3, r2		@ merge set into way/set/level
	mov	r1, #1
	lsl	r1, r1, ip		@ r1 = set decr

	ubfx	ip, r0, #3, #10		@ get numways - 1 from [to be discarded] CCSIDR
	clz	r2, ip			@ number of bits to MSB of way
	lsl	ip, ip, r2		@ shift by that into way position
	mov	r0, #1			@
	lsl	r2, r0, r2		@ r2 now contains the way decr
	mov	r0, r3			@ get sets/level (no way yet)
	orr	r3, r3, ip		@ merge way into way/set/level
	bfc	r0, #0, #4		@ clear low 4 bits (level) to get numset - 1
	sub	r2, r2, r0		@ subtract from way decr

	/* r3 = ways/sets/level, r2 = way decr, r1 = set decr, r0 and ip are free */
1:	mcr	p15, 0, r3, c7, c6, 2	@ invalidate line
	cmp	r3, #15			@ are we done with this level (way/set == 0)
	bls	.Lnext_level_inv	@ yes, go to next level
	lsl	r0, r3, #10		@ clear way bits leaving only set/level bits
	lsr	r0, r0, #4		@ clear level bits leaving only set bits
	subne	r3, r3, r1		@ non-zero?, decrement set #
	subeq	r3, r3, r2		@ zero?, decrement way # and restore set count
	b	1b

.Lnext_level_inv:
	mrc	p15, 1, r0, c0, c0, 1	@ read CLIDR
	and	ip, r0, #0x07000000	@ narrow to LoC
	lsr	ip, ip, #23		@ left align LoC (low 4 bits)
	add	r3, r3, #2		@ go to next level
	cmp	r3, ip			@ compare
	blt	.Lstart_inv		@ not done, next level (r0 == CLIDR)

.Ldone_inv:
	mov	r0, #0			@ default back to cache level 0
	mcr	p15, 2, r0, c0, c0, 0	@ select cache level
	DSB
	ISB
	bx	lr

/*
 * Context switch.
 *
 * These is the CPU-specific parts of the context switcher cpu_switch()
 * These functions actually perform the TTB reload.
 *
 * NOTE: Special calling convention
 *	r1, r4-r13 must be preserved
 */
ENTRY(armv7_context_switch)
	/*
	 * We can assume that the caches will only contain kernel addresses
	 * at this point.  So no need to flush them again.
	 */
	mcr	p15, 0, r0, c7, c5, 0	/* Flush I cache */
	mcr	p15, 0, r0, c7, c5, 6	/* Flush BP cache */
	DSB
	ISB

	mcr	p15, 0, r0, c2, c0, 0	/* set the new TTB */
	mcr	p15, 0, r0, c8, c7, 0	/* and flush the I+D tlbs */
	DSB
	ISB
	mov	pc, lr

/* XXX The following macros should probably be moved to asm.h */
#define _DATA_OBJECT(x) .globl x; .type x,_ASM_TYPE_OBJECT; x:
#define C_OBJECT(x)	_DATA_OBJECT(_C_LABEL(x))

	.align 2
C_OBJECT(armv7_dcache_sets_max)
	.word	0
C_OBJECT(armv7_dcache_index_max)
	.word	0
C_OBJECT(armv7_dcache_sets_inc)
	.word	0
C_OBJECT(armv7_dcache_index_inc)
	.word	0
