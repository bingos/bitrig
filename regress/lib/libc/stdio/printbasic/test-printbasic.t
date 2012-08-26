#!/bin/sh
# $FreeBSD: src/tools/regression/lib/libc/stdio/test-printbasic.t,v 1.1 2012/01/14 21:38:31 das Exp $

cd `dirname $0`

executable=`basename $0 .t`

make $executable 2>&1 > /dev/null

exec ./$executable
