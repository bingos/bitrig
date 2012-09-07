PROG=	test-getproto
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=getprotobyname getprotobynumber getprotoent \
	getproto-2pass build-snapshot getprotobyname-snapshot \
	getprotobynumber-snapshot getprotoent-snapshot

getprotobyname: ${PROG}
	./${PROG} -n

getprotobynumber: ${PROG}
	./${PROG} -v

getprotoent: ${PROG}
	./${PROG} -e

getproto-2pass: ${PROG}
	./${PROG} -2

build-snapshot: ${PROG}
	./${PROG} -s snapshot_proto

getprotobyname-snapshot: ${PROG}
	./${PROG} -n -s snapshot_proto

getprotobynumber-snapshot: ${PROG}
	./${PROG} -v -s snapshot_proto

getprotoent-snapshot: ${PROG}
	./${PROG} -e -s snapshot_proto

.include <bsd.regress.mk>
