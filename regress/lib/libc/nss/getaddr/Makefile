PROG=	test-getaddr
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=getaddrinfo-PF_UNSPEC getaddrinfo-snapshot-PF_UNSPEC \
	getaddrinfo-PF_INET getaddrinfo-snapshot-PF_INET \
	getaddrinfo-PF_INET6 getaddrinfo-snapshot-PF_INET6 

getaddrinfo-PF_UNSPEC: ${PROG}
	./${PROG} -f ${.CURDIR}/../mach

getaddrinfo-snapshot-PF_UNSPEC: ${PROG}
	./${PROG} -f ${.CURDIR}/../mach -s snapshot_ai

getaddrinfo-PF_INET: ${PROG}
	./${PROG} -f ${.CURDIR}/../mach

getaddrinfo-snapshot-PF_INET: ${PROG}
	./${PROG} -4 -f ${.CURDIR}/../mach -s snapshot_ai4

getaddrinfo-PF_INET6: ${PROG}
	./${PROG} -f ${.CURDIR}/../mach

getaddrinfo-snapshot-PF_INET6: ${PROG}
	./${PROG} -6 -f ${.CURDIR}/../mach -s snapshot_ai6

.include <bsd.regress.mk>
