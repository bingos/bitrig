PROG=	test-getpw
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=getpwnam getpwuid getpwent getpwent-2pass build-snapshot \
	getpwnam-snapshot getpwuid-snapshot getpwent-snapshot

getpwnam: ${PROG}
	./${PROG} -n

getpwuid: ${PROG}
	./${PROG} -u

getpwent: ${PROG}
	./${PROG} -e

getpwent-2pass: ${PROG}
	./${PROG} -2

build-snapshot: ${PROG}
	./${PROG} -s snapshot_pwd

getpwnam-snapshot: ${PROG}
	./${PROG} -n -s snapshot_pwd

getpwuid-snapshot: ${PROG}
	./${PROG} -u -s snapshot_pwd

getpwent-snapshot: ${PROG}
	./${PROG} -e -s snapshot_pwd

.include <bsd.regress.mk>
