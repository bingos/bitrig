/*
 * Copyright (c) 2012 Sylvestre Gallon <ccna.syl@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/mount.h>

#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mntopts.h"

const struct mntopt mopts[] = {
	MOPT_STDOPTS,
	{ "subtype",	0, MFLAG_SET | MFLAG_STRVAL | MFLAG_OPT },
	{ "fsname",	0, MFLAG_SET | MFLAG_STRVAL | MFLAG_OPT },
	{ NULL }
};

void	usage(void);

int
main(int argc, char *argv[])
{
	int ch, mntflags, altflags;
	struct fusefs_args args;
	char path[MAXPATHLEN];
	
	mntflags = altflags = 0;
	while ((ch = getopt(argc, argv, "o:")) != -1)
		switch (ch) {
		case 'o':
			altflags |= getmntopts(optarg, mopts, &mntflags);
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;
	
	if (argc != 2)
		usage();
	
	args.flags = altflags;
	args.fd = atoi(argv[0]);
	
	if (realpath(argv[1], path) == NULL)
		err(1, "realpath %s", argv[1]);

	if (mount(MOUNT_FUSEFS, path, mntflags, &args)) {
		if (errno == EOPNOTSUPP)
			errx(1, "%s: Filesystem not supported by kernel",
			    argv[1]);
		else
			err(1, "%s", argv[1]);
	}
	exit(0);
}

void
usage(void)
{
	(void)fprintf(stderr,
		"usage: mount_procfs [-o options] fd mount_point\n");
	exit(1);
}
