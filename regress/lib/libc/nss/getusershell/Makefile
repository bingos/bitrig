PROG=	test-getusershell
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=getusershell

getusershell:	${PROG}
	./${PROG} -s snapshot_usershell

.include <bsd.regress.mk>
