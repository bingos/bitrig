__FBSDID("$FreeBSD: src/lib/libc/gen/getprogname.c,v 1.4 2002/03/29 22:43:41 markm Exp $");

extern char *__progname;		/* Program name, from crt0. */

const char *
getprogname(void)
{

	return (__progname);
}
