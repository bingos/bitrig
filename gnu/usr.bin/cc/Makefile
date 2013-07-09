# $FreeBSD: src/gnu/usr.bin/cc/Makefile,v 1.41.8.1 2009/04/15 03:14:26 kensmith Exp $
# $OpenBSD: Makefile,v 1.2 2013/05/18 18:18:21 miod Exp $

.include <bsd.own.mk>

.if make(obj) || make(depend) || make(clean) || make(cleandir)
SUBDIR=	cc_tools
.endif

SUBDIR+= libcpp cpp

.include <bsd.subdir.mk>
