__FBSDID("$FreeBSD: src/lib/libc/gen/setprogname.c,v 1.8 2002/03/29 22:43:41 markm Exp $");

#include <string.h>

extern char *__progname;		/* Program name, from crt0. */

void
setprogname(const char *progname)
{
	const char *p;

	p = strrchr(progname, '/');
	if (p != NULL)
		__progname = (char *)p + 1;
	else
		__progname = (char *)progname;
}
