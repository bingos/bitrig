# $OpenBSD$

PROG=	mount_fusefs
SRCS=	mount_fusefs.c getmntopts.c
NOMAN=

MOUNT=	${.CURDIR}/../mount
CFLAGS+= -I${MOUNT}
.PATH:	${MOUNT}

.include <bsd.prog.mk>
