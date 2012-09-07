PROG=	test-gethostby
CFLAGS+=-D__FBSDID\(x\)=""

REGRESS_TARGETS=gethostbyname2-IPv4 \
	gethostbyaddr-IPv4 \
	gethostbyname2-getaddrinfo-IPv4 \
	gethostbyaddr-getnameinfo-IPv4 \
	gethostbyname2-snapshot-IPv4 \
	gethostbyaddr-snapshot-IPv4 \
	gethostbyname2-IPv6 \
	gethostbyaddr-IPv6 \
	gethostbyname2-getaddrinfo-IPv6 \
	gethostbyaddr-getnameinfo-IPv6 \
	gethostbyname2-snapshot-IPv6 \
	gethostbyaddr-snapshot-IPv6 \
	gethostbyname2-IPv6-mapped \
	gethostbyaddr-IPv6-mapped \
	gethostbyname2-snapshot-IPv6-mapped \
	gethostbyaddr-snapshot-IPv6-mapped \
	getipnodebyname-IPv4 \
	getipnodebyaddr-IPv4 \
	getipnodebyname-getaddrinfo-IPv4 \
	getipnodebyaddr-getnameinfo-IPv4 \
	getipnodebyname-snapshot-IPv4 \
	getipnodebyname-snapshot-IPv4 \
	getipnodebyname-IPv6 \
	getipnodebyaddr-IPv6 \
	getipnodebyname-getaddrinfo-IPv6 \
	getipnodebyaddr-getnameinfo-IPv6 \
	getipnodebyname-snapshot-IPv6 \
	getipnodebyaddr-snapshot-IPv6 \
	getipnodebyname-IPv6-AI_V4MAPPED \
	getipnodebyaddr-IPv6-AI_V4MAPPED \
	getipnodebyname-snapshot-IPv6-AI_V4MAPPED \
	getipnodebyaddr-snapshot-IPv6-AI_V4MAPPED \
	getipnodebyname-IPv6-AI_V4MAPPED_CFG \
	getipnodebyaddr-IPv6-AI_V4MAPPED_CFG \
	getipnodebyname-snapshot-IPv6-AI_V4MAPPED_CFG \
	getipnodebyaddr-snapshot-IPv6-AI_V4MAPPED_CFG \
	getipnodebyname-IPv6-AI_V4MAPPED_CFG-|-AI_ALL \
	getipnodebyaddr-IPv6-AI_V4MAPPED_CFG-|-AI_ALL \
	getipnodebyname-snapshot-IPv6-AI_V4MAPPED_CFG-|-AI_ALL \
	getipnodebyaddr-snapshot-IPv6-AI_V4MAPPED_CFG-|-AI_ALL \
	getipnodebyname-IPv6-AI_V4MAPPED_CFG-|-AI_ADDRCONFIG \
	getipnodebyname-snapshot-IPv6-AI_V4MAPPED_CFG-|-AI_ADDRCONFIG \
	getipnodebyname-IPv4-AI_ADDRCONFIG \
	getipnodebyname-snapshot-IPv4-AI_ADDRCONFIG \
	getipnodebyname-IPv6-AI_ADDRCONFIG \
	getipnodebyname-snapshot-IPv6-AI_ADDRCONFIG


gethostbyname2-IPv4: ${PROG}
	./${PROG} -4 -n -f ${.CURDIR}/../mach

gethostbyaddr-IPv4: ${PROG}
	./${PROG} -4 -a -f ${.CURDIR}/../mach

gethostbyname2-getaddrinfo-IPv4: ${PROG}
	./${PROG} -4 -2 -f ${.CURDIR}/../mach

gethostbyaddr-getnameinfo-IPv4: ${PROG}
	./${PROG} -4 -i -f ${.CURDIR}/../mach

gethostbyname2-snapshot-IPv4: ${PROG}
	./${PROG} -4 -n -s snapshot_htname4 -f ${.CURDIR}/../mach

gethostbyaddr-snapshot-IPv4: ${PROG}
	./${PROG} -4 -a -s snapshot_htaddr4 -f ${.CURDIR}/../mach

gethostbyname2-IPv6: ${PROG}
	./${PROG} -6 -n -f ${.CURDIR}/../mach

gethostbyaddr-IPv6: ${PROG}
	./${PROG} -6 -a -f ${.CURDIR}/../mach

gethostbyname2-getaddrinfo-IPv6: ${PROG}
	./${PROG} -6 -2 -f ${.CURDIR}/../mach

gethostbyaddr-getnameinfo-IPv6: ${PROG}
	./${PROG} -6 -i -f ${.CURDIR}/../mach

gethostbyname2-snapshot-IPv6: ${PROG}
	./${PROG} -6 -n -s snapshot_htname6 -f ${.CURDIR}/../mach

gethostbyaddr-snapshot-IPv6: ${PROG}
	./${PROG} -6 -a -s snapshot_htaddr6 -f ${.CURDIR}/../mach

gethostbyname2-IPv6-mapped: ${PROG}
	./${PROG} -m -n -f ${.CURDIR}/../mach

gethostbyaddr-IPv6-mapped: ${PROG}
	./${PROG} -m -a -f ${.CURDIR}/../mach

gethostbyname2-snapshot-IPv6-mapped: ${PROG}
	./${PROG} -m -n -s snapshot_htname6map -f ${.CURDIR}/../mach

gethostbyaddr-snapshot-IPv6-mapped: ${PROG}
	./${PROG} -m -a -s snapshot_htaddr6map -f ${.CURDIR}/../mach

getipnodebyname-IPv4: ${PROG}
	./${PROG} -o -4 -n -f ${.CURDIR}/../mach

getipnodebyaddr-IPv4: ${PROG}
	./${PROG} -o -4 -a -f ${.CURDIR}/../mach

getipnodebyname-getaddrinfo-IPv4: ${PROG}
	./${PROG} -o -4 -2 -f ${.CURDIR}/../mach

getipnodebyaddr-getnameinfo-IPv4: ${PROG}
	./${PROG} -o -4 -i -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv4: ${PROG}
	./${PROG} -o -4 -n -s snapshot_ipnodename4 -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv4: ${PROG}
	./${PROG} -o -4 -a -s snapshot_ipnodeaddr4 -f ${.CURDIR}/../mach

getipnodebyname-IPv6: ${PROG}
	./${PROG} -o -6 -n -f ${.CURDIR}/../mach

getipnodebyaddr-IPv6: ${PROG}
	./${PROG} -o -6 -a -f ${.CURDIR}/../mach

getipnodebyname-getaddrinfo-IPv6: ${PROG}
	./${PROG} -o -6 -2 -f ${.CURDIR}/../mach

getipnodebyaddr-getnameinfo-IPv6: ${PROG}
	./${PROG} -o -6 -i -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv6: ${PROG}
	./${PROG} -o -6 -n -s snapshot_ipnodename6 -f ${.CURDIR}/../mach

getipnodebyaddr-snapshot-IPv6: ${PROG}
	./${PROG} -o -6 -a -s snapshot_ipnodeaddr6 -f ${.CURDIR}/../mach

getipnodebyname-IPv6-AI_V4MAPPED: ${PROG}
	./${PROG} -o -m -n -f ${.CURDIR}/../mach

getipnodebyaddr-IPv6-AI_V4MAPPED: ${PROG}
	./${PROG} -o -m -a -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv6-AI_V4MAPPED: ${PROG}
	./${PROG} -o -m -n -s snapshot_ipnodename6_AI_V4MAPPED -f ${.CURDIR}/../mach

getipnodebyaddr-snapshot-IPv6-AI_V4MAPPED: ${PROG}
	./${PROG} -o -m -a -s snapshot_ipnodeaddr6_AI_V4MAPPED -f ${.CURDIR}/../mach

getipnodebyname-IPv6-AI_V4MAPPED_CFG: ${PROG}
	./${PROG} -o -M -n -f ${.CURDIR}/../mach

getipnodebyaddr-IPv6-AI_V4MAPPED_CFG: ${PROG}
	./${PROG} -o -M -a -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv6-AI_V4MAPPED_CFG: ${PROG}
	./${PROG} -o -M -n -s snapshot_ipnodename6_AI_V4MAPPED_CFG -f ${.CURDIR}/../mach

getipnodebyaddr-snapshot-IPv6-AI_V4MAPPED_CFG: ${PROG}
	./${PROG} -o -M -a -s snapshot_ipnodeaddr6_AI_V4MAPPED_CFG -f ${.CURDIR}/../mach

getipnodebyname-IPv6-AI_V4MAPPED_CFG-|-AI_ALL: ${PROG}
	./${PROG} -o -MA -n -f ${.CURDIR}/../mach

getipnodebyaddr-IPv6-AI_V4MAPPED_CFG-|-AI_ALL: ${PROG}
	./${PROG} -o -MA -a -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv6-AI_V4MAPPED_CFG-|-AI_ALL: ${PROG}
	./${PROG} -o -MA -n -s snapshot_ipnodename6_AI_V4MAPPED_CFG_AI_ALL -f ${.CURDIR}/../mach

getipnodebyaddr-snapshot-IPv6-AI_V4MAPPED_CFG-|-AI_ALL: ${PROG}
	./${PROG} -o -MA -a -s snapshot_ipnodeaddr6_AI_V4MAPPED_CFG_AI_ALL -f ${.CURDIR}/../mach

getipnodebyname-IPv6-AI_V4MAPPED_CFG-|-AI_ADDRCONFIG: ${PROG}
	./${PROG} -o -Mc -n -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv6-AI_V4MAPPED_CFG-|-AI_ADDRCONFIG: ${PROG}
	./${PROG} -o -Mc -n -s snapshot_ipnodename6_AI_V4MAPPED_CFG_AI_ADDRCONFIG -f ${.CURDIR}/../mach

getipnodebyname-IPv4-AI_ADDRCONFIG: ${PROG}
	./${PROG} -o -4c -n -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv4-AI_ADDRCONFIG: ${PROG}
	./${PROG} -o -4c -n -s snapshot_ipnodename4_AI_ADDRCONFIG -f ${.CURDIR}/../mach

getipnodebyname-IPv6-AI_ADDRCONFIG: ${PROG}
	./${PROG} -o -6c -n -f ${.CURDIR}/../mach

getipnodebyname-snapshot-IPv6-AI_ADDRCONFIG: ${PROG}
	./${PROG} -o -6c -n -s snapshot_ipnodename6_AI_ADDRCONFIG -f ${.CURDIR}/../mach

.include <bsd.regress.mk>
