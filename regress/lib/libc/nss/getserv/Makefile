PROG=	test-getserv
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=getservbyname getservbyport getservent getservent-2pass \
	build-snapshot getservbyname-snapshot getservbyport-snapshot \
	getservent-snapshot

getservbyname: ${PROG}
	./${PROG} -n

getservbyport: ${PROG}
	./${PROG} -p

getservent: ${PROG}
	./${PROG} -e

getservent-2pass: ${PROG}
	./${PROG} -2

build-snapshot: ${PROG}
	./${PROG} -s snapshot_serv

getservbyname-snapshot: ${PROG}
	./${PROG} -n -s snapshot_serv

getservbyport-snapshot: ${PROG}
	./${PROG} -p -s snapshot_serv

getservent-snapshot: ${PROG}
	./${PROG} -e -s snapshot_serv

.include <bsd.regress.mk>
