#	$OpenBSD: Makefile,v 1.12 2012/12/21 08:50:27 espie Exp $

PROG=	awk
SRCS=	ytab.c lex.c b.c main.c parse.c proctab.c tran.c lib.c run.c
LDADD=	-lm
DPADD=	${LIBM}
CLEANFILES+=proctab.c maketab ytab.c ytab.h
CFLAGS+=-I. -I${.CURDIR} -DHAS_ISBLANK -DNDEBUG
HOSTCFLAGS+=-I. -I${.CURDIR} -DHAS_ISBLANK -DNDEBUG

ytab.c ytab.h: awkgram.y
	${YACC} -d ${.CURDIR}/awkgram.y
	mv y.tab.c ytab.c
	mv y.tab.h ytab.h

proctab.c: maketab
	./maketab >proctab.c

maketab: ytab.h maketab.c
	${HOSTCC} ${HOSTCFLAGS} ${.CURDIR}/maketab.c -o $@

.include <bsd.prog.mk>
