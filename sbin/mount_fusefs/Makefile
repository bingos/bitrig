#	$OpenBSD: src/sbin/mount_procfs/Makefile,v 1.7 2004/06/22 21:12:00 otto Exp $

PROG=	mount_fusefs
SRCS=	mount_fusefs.c getmntopts.c
NOMAN=

MOUNT=	${.CURDIR}/../mount
CFLAGS+= -I${MOUNT}
.PATH:	${MOUNT}

.include <bsd.prog.mk>
