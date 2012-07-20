/*	$OpenBSD: _types.h,v 1.2 2008/03/16 19:42:57 otto Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.3 (Berkeley) 1/5/94
 */

#ifndef _SYS__TYPES_H_
#define	_SYS__TYPES_H_

#include <machine/_types.h>

typedef	unsigned long	__cpuid_t;	/* CPU id */
typedef	__int32_t	__dev_t;	/* device number */
typedef	__uint32_t	__fixpt_t;	/* fixed point number */
typedef	__uint32_t	__gid_t;	/* group id */
typedef	__uint32_t	__id_t;		/* may contain pid, uid or gid */
typedef __uint32_t	__in_addr_t;	/* base type for internet address */
typedef __uint16_t	__in_port_t;	/* IP port type */
typedef	__uint32_t	__ino_t;	/* inode number */
typedef	long		__key_t;	/* IPC key (for Sys V IPC) */
typedef	__uint32_t	__mode_t;	/* permissions */
typedef	__uint32_t	__nlink_t;	/* link count */
typedef	__int32_t	__pid_t;	/* process id */
typedef __uint64_t	__rlim_t;	/* resource limit */
typedef __uint8_t	__sa_family_t;	/* sockaddr address family type */
typedef	__int32_t	__segsz_t;	/* segment size */
typedef __uint32_t	__socklen_t;	/* length type for network syscalls */
typedef	__int32_t	__swblk_t;	/* swap offset */
typedef	__uint32_t	__uid_t;	/* user id */
typedef	__uint32_t	__useconds_t;	/* microseconds */
typedef	__int32_t	__suseconds_t;	/* microseconds (signed) */
typedef __uint64_t	__fsblkcnt_t;	/* file system block count */
typedef __uint64_t	__fsfilcnt_t;	/* file system file count */

/*
 * Unusual type definitions.
 */
/*
 * rune_t is declared to be an ``int'' instead of the more natural
 * ``unsigned long'' or ``long''.  Two things are happening here.  It is not
 * unsigned so that EOF (-1) can be naturally assigned to it and used.  Also,
 * it looks like 10646 will be a 31 bit standard.  This means that if your
 * ints cannot hold 32 bits, you will be in trouble.  The reason an int was
 * chosen over a long is that the is*() and to*() routines take ints (says
 * ANSI C), but they use __ct_rune_t instead of int.
 *
 * NOTE: rune_t is not covered by ANSI nor other standards, and should not
 * be instantiated outside of lib/libc/locale.  Use wchar_t.  wint_t and
 * rune_t must be the same type.  Also, wint_t should be able to hold all
 * members of the largest character set plus one extra value (WEOF), and
 * must be at least 16 bits.
 */
typedef int		__ct_rune_t;    /* arg type for ctype funcs */
typedef __ct_rune_t	__rune_t;       /* rune_t (see above) */
typedef __ct_rune_t	__wint_t;       /* wint_t (see above) */
typedef __uint32_t	__fixpt_t;      /* fixed point number */

/*
 * mbstate_t is an opaque object to keep conversion state, during multibyte
 * stream conversions. The content must not be referenced by user programs.
 */
typedef union {
	char __mbstate8[128];
	__int64_t __mbstateL;			/* for alignment */
} __mbstate_t;

#endif /* !_SYS__TYPES_H_ */
