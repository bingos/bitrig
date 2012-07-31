LIB=cxx

CXXFLAGS+= -std=c++0x -fstrict-aliasing -Wall -Wextra -Wshadow -Wconversion
CXXFLAGS+= -Wnewline-eof -Wpadded -Wmissing-prototypes -Wstrict-aliasing=2
CXXFLAGS+= -Wstrict-overflow=4 -D_LIBCPP_HAS_NO_CONSTEXPR -D__XSI_VISIBLE
CXXFLAGS+= -I${.CURDIR}/libcxx/include
CXXFLAGS+= -g

LIBCSRCDIR=	${.CURDIR}/../libc

.PATH: ${.CURDIR}/libcxx/src

SRCS=	algorithm.cpp \
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

.include <bsd.lib.mk>
