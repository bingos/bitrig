PROG=	test-getgr
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=getgrnam getgrgid getgrent getgrent-2pass build-snapshot \
	getgrnam-snapshot getgrgid-snapshot getgrent-snapshot 

getgrnam: ${PROG}
	./${PROG} -n

getgrgid: ${PROG}
	./${PROG} -g

getgrent: ${PROG}
	./${PROG} -e

getgrent-2pass: ${PROG}
	./${PROG} -2

building-snapshot-if-needed: ${PROG}
	./${PROG} -s snapshot_grp

getgrnam-snapshot: ${PROG}
	./${PROG} -n -s snapshot_grp

getgrgid-snapshot: ${PROG}
	./${PROG} -g -s snapshot_grp

getgrent-snapshot: ${PROG}
	./${PROG} -e -s snapshot_grp

.include <bsd.regress.mk>
