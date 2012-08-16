.PATH: ${.CURDIR}/src

LIB=c++

HDRDIR= ${.CURDIR}/include
RTDIR= ${.CURDIR}/../libcxxrt/src
CXXINCLUDEDIR=	c++

CXXFLAGS+= -std=c++0x -nostdlib -nostdinc -I/usr/include -v
CXXFLAGS+= -fstrict-aliasing -Wall -Wextra -Wshadow -Wconversion
CXXFLAGS+= -Wnewline-eof -Wpadded -Wmissing-prototypes -Wstrict-aliasing=2
CXXFLAGS+= -Wstrict-overflow=4
CXXFLAGS+= -DLIBCXXRT -D__XSI_VISIBLE -D_LIBCPP_HAS_NO_CONSTEXPR
#-D_LIBCPP_HAS_NO_CONSTEXPR -D__XSI_VISIBLE
CXXFLAGS+= -I${HDRDIR} -I${RTDIR}
CXXFLAGS+= -g

SRCS+=	algorithm.cpp \
	bind.cpp \
	chrono.cpp \
	condition_variable.cpp \
	debug.cpp \
	exception.cpp \
	future.cpp \
	hash.cpp \
	ios.cpp \
	iostream.cpp \
	locale.cpp \
	memory.cpp \
	mutex.cpp \
	new.cpp \
	random.cpp \
	regex.cpp \
	stdexcept.cpp \
	string.cpp \
	strstream.cpp \
	system_error.cpp \
	thread.cpp \
	typeinfo.cpp \
	utility.cpp \
	valarray.cpp

STD_HEADERS=	__bit_reference\
		__config\
		__debug\
		__functional_03\
		__functional_base\
		__functional_base_03\
		__hash_table\
		__locale\
		__mutex_base\
		__split_buffer\
		__sso_allocator\
		__std_stream\
		__tree\
		__tuple\
		__tuple_03\
		__undef_min_max\
		algorithm\
		array\
		atomic\
		bitset\
		cassert\
		ccomplex\
		cctype\
		cerrno\
		cfenv\
		cfloat\
		chrono\
		cinttypes\
		ciso646\
		climits\
		clocale\
		cmath\
		codecvt\
		complex\
		complex.h\
		condition_variable\
		csetjmp\
		csignal\
		cstdarg\
		cstdbool\
		cstddef\
		cstdint\
		cstdio\
		cstdlib\
		cstring\
		ctgmath\
		ctime\
		cwchar\
		cwctype\
		deque\
		exception\
		forward_list\
		fstream\
		functional\
		future\
		initializer_list\
		iomanip\
		ios\
		iosfwd\
		iostream\
		istream\
		iterator\
		limits\
		list\
		locale\
		map\
		memory\
		mutex\
		new\
		numeric\
		ostream\
		queue\
		random\
		ratio\
		regex\
		scoped_allocator\
		set\
		sstream\
		stack\
		stdexcept\
		streambuf\
		string\
		strstream\
		system_error\
		tgmath.h\
		thread\
		tuple\
		type_traits\
		typeindex\
		typeinfo\
		unordered_map\
		unordered_set\
		utility\
		valarray\
		vector

EXT_HEADERS=	__hash\
		hash_map\
		hash_set
EXTDIR=		${CXXINCLUDEDIR}/ext

includes:
	mkdir -p ${DESTDIR}/usr/include/${CXXINCLUDEDIR}
	cd ${HDRDIR}; for i in ${STD_HEADERS}; do \
		cmp -s $$i ${DESTDIR}/usr/include/$$i || \
		${INSTALL} ${INSTALL_COPY} -m 444 -o $(BINOWN) -g $(BINGRP) $$i \
		${DESTDIR}/usr/include/${CXXINCLUDEDIR}; done

	mkdir -p ${DESTDIR}/usr/include/${EXTDIR}
	cd ${HDRDIR}/ext; for i in ${EXT_HEADERS}; do \
		cmp -s $$i ${DESTDIR}/usr/include/$$i || \
		${INSTALL} ${INSTALL_COPY} -m 444 -o $(BINOWN) -g $(BINGRP) $$i \
		${DESTDIR}/usr/include/${EXTDIR}; done

.include <bsd.lib.mk>
