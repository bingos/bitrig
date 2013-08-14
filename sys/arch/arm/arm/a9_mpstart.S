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

#include <arm/armreg.h>
#include <machine/asm.h>
#include "assym.h"

#define	DSB	.long	0xf57ff040
#define	ISB	.long	0xf57ff060
#define	WFI	.long	0xe320f003

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
ENTRY(armv7_mpstart)
	/*
	 * Step 1, invalidate the caches
	 */
	bl	_C_LABEL(armv7_icache_inv_all)	@ toss i-cache
	bl	_C_LABEL(armv7_dcache_inv_all)	@ toss d-cache

	/*
	 * Step 2, wait for the SCU to be enabled
	 */
	mrc	p15, 4, r3, c15, c0, 0		@ read cbar
1:	ldr	r0, [r3, #SCU_CTL]		@ read scu control
	and	r0, r0, #SCU_CTL_SCU_ENA
	cmp	r0, #SCU_CTL_SCU_ENA
	bne	1b				@ try again

	/*
	 * Step 3, set ACTLR.SMP=1 (and ACTRL.FW=1)
	 */
	mrc	p15, 0, r0, c1, c0, 1		@ read aux ctl
	orr	r0, #CORTEX_A9_AUXCTL_SMP	@ enable SMP
	orr	r0, #CORTEX_A9_AUXCTL_FW	@ enable cache/tlb/coherency
	mcr	p15, 0, r0, c1, c0, 1		@ write aux ctl
	CPWAIT(r0)

	/*
	 * We should be in SMP mode now.
	 */
	mrc	p15, 0, r4, c0, c0, 5		@ get MPIDR
	and	r4, r4, #7			@ get our cpu number

	/* Set the Domain Access register.  Very important! */
	mov	r0, #((DOMAIN_CLIENT << (PMAP_DOMAIN_KERNEL*2)) | DOMAIN_CLIENT)
	mcr	p15, 0, r0, c3, c0, 0
	CPWAIT(r0)

	ldr	r0, Lstartup_pagetable

	mcr	p15, 0, r0, c2, c0, 0		@ Set TTB
	mcr	p15, 0, r0, c8, c7, 0		@ Flush TLB
	CPWAIT(r0)

	/* Enable MMU */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #CPU_CONTROL_MMU_ENABLE
	mcr	p15, 0, r0, c1, c0, 0
	CPWAIT(r0)

	mov	r0, #0xB0000000			@ XXX: SDRAM_START is 0x10000000
	add	pc, pc, r0			@ jump into our space
	mov	r0, r0				@ nop, not used

	ldr	r0, .Ll1pagetable		@ get address of l1pt pvaddr
	ldr	r0, [r0]

	bl	armv7_setttb

	/*
	* So, we're now in Kernel VM.
	* Let's set some other basic stuff and jump into C!
	*/

	/* Load svc stack */
	ldr	sp, .Lkernelstack
	ldr	sp, [sp]
	ldr	r2, .Lkernelstacksize
	mla	r3, r4, r2, r2			@ r3 = (core id * stacksize) + stacksize
	add	sp, r3

	/* Enable Caches */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #(CPU_CONTROL_IC_ENABLE)
	orr	r0, r0, #(CPU_CONTROL_DC_ENABLE)
	mcr	p15, 0, r0, c1, c0, 0
	CPWAIT(r0)

	dsb						@ make sure all memory stuff is done

	/* set SVC mode, disable IRQ|FIQ */
	mrs	r2, cpsr
	bic	r2, r2, #(PSR_MODE)
	orr	r2, r2, #(PSR_SVC32_MODE)
	orr	r2, r2, #(I32_bit | F32_bit)
	msr	cpsr, r2

	mov	r0, r4
	bl	_C_LABEL(cpu_hatch)

	b	loop				@ XXX: not there yet

loop:
	WFI
	b	loop

Lstartup_pagetable:     .word   STARTUP_PAGETABLE_ADDR
.Ll1pagetable:
	.word	_C_LABEL(l1pagetable)
.Lkernelstack:
	.word	_C_LABEL(proc0paddr)
.Lkernelstacksize:
	.word	UPAGES * PAGE_SIZE
.Lcpu_info:
	.word	_C_LABEL(cpu_info)
