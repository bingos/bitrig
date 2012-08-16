.PATH: ${.CURDIR}/src
RTDIR= ${.CURDIR}/src
CXXINCLUDEDIR=	c++

LIB=		cxxrt

SRCS+=		libelftc_dem_gnu3.c\
		terminate.cc\
		dynamic_cast.cc\
		memory.cc\
		auxhelper.cc\
		exception.cc\
		stdexcept.cc\
		typeinfo.cc\
		guard.cc

CFLAGS+=	-I${.CURDIR}/src

RT_HEADERS=	cxxabi.h\
		unwind.h\
		unwind-arm.h\
		unwind-itanium.h

includes:
	mkdir -p ${DESTDIR}/usr/include/${CXXINCLUDEDIR}
	cd ${RTDIR}; for i in ${RT_HEADERS}; do \
		cmp -s $$i ${DESTDIR}/usr/include/$$i || \
		${INSTALL} ${INSTALL_COPY} -m 444 -o $(BINOWN) -g $(BINGRP) $$i \
		${DESTDIR}/usr/include/${CXXINCLUDEDIR}; done

.include <bsd.lib.mk>
