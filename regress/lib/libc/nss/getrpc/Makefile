PROG=	test-getrpc
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=getrpcbyname getrpcbynumber getrpcent getrpcent-2pass \
	build-snapshot getrpcbyname-snapshot getrpcbynumber-snapshot \
	getrpcent-snapshot

getrpcbyname: ${PROG}
	./${PROG} -n

getrpcbynumber: ${PROG}
	./${PROG} -v

getrpcent: ${PROG}
	./${PROG} -e

getrpcent-2pass: ${PROG}
	./${PROG} -2

build-snapshot: ${PROG}
	./${PROG} -s snapshot_rpc

getrpcbyname-snapshot: ${PROG}
	./${PROG} -n -s snapshot_rpc

getrpcbynumber-snapshot: ${PROG}
	./${PROG} -v -s snapshot_rpc

getrpcent-snapshot: ${PROG}
	./${PROG} -e -s snapshot_rpc

.include <bsd.regress.mk>
