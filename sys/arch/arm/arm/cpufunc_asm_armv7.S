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

#define	DSB	.long	0xf57ff040
#define	ISB	.long	0xf57ff060
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

/*
 * CPWAIT -- Canonical method to wait for CP15 update.
 * NOTE: Clobbers the specified temp reg.
 * copied from arm/arm/cpufunc_asm_xscale.S
 * XXX: better be in a common header file.
 */
#define CPWAIT_BRANCH							\
	sub	pc, pc, #4
#define CPWAIT(tmp)							\
	mrc	p15, 0, tmp, c2, c0, 0	/* arbitrary read of CP15 */	;\
	mov	tmp, tmp		/* wait for it to complete */	;\
	CPWAIT_BRANCH			/* branch to next insn */

#define SCU_CTL 0x0
#define SCU_CTL_SCU_ENA 0x1
#define CORTEXA9_AUXCTL_SMP (1 << 6)
#define CORTEXA9_AUXCTL_FW (1 << 0)
ENTRY(armv7_mpstart)
	mov r2, #0x02000000
	add r2, r2, #0x001e0000
	add r2, r2, #0x00008000
	add r2, r2, #0x00000040

	/* print "Core " */
	mov	r3, #0x43
	strb	r3, [r2]
	mov	r3, #0x6F
	strb	r3, [r2]
	mov	r3, #0x72
	strb	r3, [r2]
	mov	r3, #0x65
	strb	r3, [r2]
	mov	r3, #0x20
	strb	r3, [r2]

	/*
	 * Step 1, invalidate the caches
	 */
	/*
	bl	_C_LABEL(armv7_idcache_wbinv_all)	@ toss d-cache
	*/

	/*
	 * Step 2, wait for the SCU to be enabled
	 */
	mrc	p15, 4, r3, c15, c0, 0		@ read cbar
1:	ldr	r0, [r3, #SCU_CTL]		@ read scu control
	and	r0, r0, #SCU_CTL_SCU_ENA
	cmp	r0, #SCU_CTL_SCU_ENA
	bne	1b				@ try again

	/*
	 * Step 3, set ACTLR.SMP=1 (and ACTRL.FX=1)
	 */
	mrc	p15, 0, r0, c1, c0, 1		@ read aux ctl
	orr	r0, #CORTEXA9_AUXCTL_SMP	@ enable SMP
	mcr	p15, 0, r0, c1, c0, 1		@ write aux ctl
	mov	r0, r0
	orr	r0, #CORTEXA9_AUXCTL_FW		@ enable cache/tlb/coherency
	mcr	p15, 0, r0, c1, c0, 1		@ write aux ctl
	mov	r0, r0

	/*
	 * We should be in SMP mode now.
	 */
	mrc	p15, 0, r4, c0, c0, 5		@ get MPIDR
	and	r4, r4, #7			@ get our cpu number

	/* print number and newline */
	add	r3, r4, #0x30
	strb	r3, [r2]
	mov	r3, #0x0D
	strb	r3, [r2]
	mov	r3, #0x0A
	strb	r3, [r2]

	ldr	r0, Lstartup_pagetable

	mcr	p15, 0, r0, c2, c0, 0		/* Set TTB */
	mcr	p15, 0, r0, c8, c7, 0		/* Flush TLB */

	/* Set the Domain Access register.  Very important! */
	mov	r0, #((DOMAIN_CLIENT << (PMAP_DOMAIN_KERNEL*2)) | DOMAIN_CLIENT)
	mcr	p15, 0, r0, c3, c0, 0

	/* Enable MMU */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #CPU_CONTROL_MMU_ENABLE
	mcr	p15, 0, r0, c1, c0, 0
	CPWAIT(r0)

	mov	r0, #0xB0000000			/* XXX: SDRAM_START is 0x10000000 */
	add	pc, pc, r0			/* jump into our space */
	mov	r0, r0				/* nop, not used */

	ldr	r0, .Ll1pagetable		/* get address of l1pt pvaddr */
	ldr	r0, [r0]

	bl	armv7_setttb

	/* print number and newline, this time to the mapped console */
	mov	r2, #0xfd000000
	add	r2, r2, #0x000e0000
	add	r2, r2, #0x00008000
	add	r2, r2, #0x00000040
	add	r3, r4, #0x30
	strb	r3, [r2]
	mov	r3, #0x0D
	strb	r3, [r2]
	mov	r3, #0x0A
	strb	r3, [r2]

	ldr	r0, .Lcpu_hatched		@ now show we've hatched
	mov	r5, #1
	lsl	r1, r5, r4
	ldr	r4, [r0]
	orr	r1, r1, r4
	str	r1, [r0]			@ make this atomic

	b	loop				@ XXX: not there yet

loop:
	WFI
	b	loop

ENTRY(aux_control)
	mrc	p15, 0, r3, c1, c0, 1	/* Read the control register */
	bic	r2, r3, r0		/* Clear bits */
	eor	r2, r2, r1		/* XOR bits */

	teq	r2, r3			/* Only write if there is a change */
	mcrne	p15, 0, r2, c1, c0, 1	/* Write new control register */
	mov	r0, r3			/* Return old value */
	mov	pc, lr

/* XXX The following macros should probably be moved to asm.h */
#define _DATA_OBJECT(x) .globl x; .type x,_ASM_TYPE_OBJECT; x:
#define C_OBJECT(x)	_DATA_OBJECT(_C_LABEL(x))

Lstartup_pagetable:     .word   STARTUP_PAGETABLE_ADDR
.Ll1pagetable:
	.word	_C_LABEL(l1pagetable)
.Lcpu_hatched:
	.word	_C_LABEL(arm_cpu_hatched)

	.align 2
C_OBJECT(armv7_dcache_sets_max)
	.word	0
C_OBJECT(armv7_dcache_index_max)
	.word	0
C_OBJECT(armv7_dcache_sets_inc)
	.word	0
C_OBJECT(armv7_dcache_index_inc)
	.word	0
