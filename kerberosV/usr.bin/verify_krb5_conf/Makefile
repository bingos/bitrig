# $OpenBSD: Makefile,v 1.11 2013/07/10 11:14:40 ajacoutot Exp $

PROG=	verify_krb5_conf
MPAGES=	verify_krb5_conf.8
MSRC=	${KRB5DIR}/lib/krb5
CPPFLAGS=	-I${KRB5DIR}/lib/asn1 -I${KRB5DIR}/lib/krb5 -I${KRB5DIR}/lib/roken
DPADD=	${LIBKAFS5} ${LIBKRB5} ${LIBROKEN} ${LIBVERS} \
	${LIBASN1} ${LIBCRYPTO} ${LIBCOM_ERR}
LDADD=	-lkafs5 -lkrb5 -lroken ${LIBVERS} \
	-lasn1 -lcrypto -lcom_err -lwind -lheimbase

.include <bsd.prog.mk>

.PATH: ${KRB5DIR}/lib/krb5
