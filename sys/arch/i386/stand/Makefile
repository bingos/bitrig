#	$OpenBSD: Makefile,v 1.19 2013/01/18 04:10:16 jsing Exp $

.if ${MACHINE} == "i386"
SUBDIR=	mbr cdbr
.endif
SUBDIR+=biosboot boot cdboot installboot pxeboot

test-fd0:
	sudo mount /dev/fd0a /mnt
	sudo cp /usr/mdec/boot /mnt
	sudo /usr/mdec/installboot -v /mnt/boot /usr/mdec/biosboot fd0
	sudo umount /mnt

zero-fd0:
	sudo dd if=/dev/zero of=/dev/rfd0a count=144 bs=10240

prep-fd0:
	sudo disklabel -w fd0 floppy
	sudo newfs -O 0 -m 0 -o space -i 3000 -c 80 /dev/rfd0a

.include <bsd.subdir.mk>
