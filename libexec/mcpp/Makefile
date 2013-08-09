CFLAGS+=-I. -I${.CURDIR} -I${.SRCDIR}/dist-mcpp/src -DHAVE_CONFIG_H

PROG=mcpp

.PATH: ${.CURDIR}/dist-mcpp/src


SRCS= directive.c main.c system.c eval.c mbchar.c
SRCS+= expand.c support.c

.include <bsd.prog.mk>

