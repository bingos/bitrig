/*
 * Copyright (c) 1997 Tobias Weingartner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/disklabel.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <paths.h>
#include "disk.h"
#include "user.h"

#define _PATH_GPT _PATH_BOOTDIR "gpt"
static unsigned char builtin_gpt[] = {
#include "gptcode.h"
};

int	y_flag;

static void
usage(void)
{
	extern char * __progname;

	fprintf(stderr, "usage: %s "
	    "[-eiuy] [-f gptfile] disk\n"
	    "\t-i: initialize disk with virgin GPT\n"
	    "\t-e: edit GPTs on disk interactively\n"
	    "\t-f: specify non-standard GPT template\n"
	    "\t-y: do not ask questions\n"
	    "`disk' may be of the forms: sd0 or /dev/rsd0c.\n",
	    __progname);
	exit(1);
}


int
main(int argc, char *argv[])
{
	int ch, fd, error;
	int i_flag = 0, m_flag = 0;
	disk_t disk;
	DISK_metrics *usermetrics;
#ifdef HAS_GPT
	char *gptfile = _PATH_GPT;
#else
	char *gptfile = NULL;
#endif
	gpt_t gpt;
	char gpt_buf[DEV_BSIZE];

	while ((ch = getopt(argc, argv, "ieuf:y")) != -1) {
		const char *errstr;

		switch(ch) {
		case 'i':
			i_flag = 1;
			break;
		case 'e':
			m_flag = 1;
			break;
		case 'f':
			gptfile = optarg;
			break;
		case 'y':
			y_flag = 1;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	/* Argument checking */
	if (argc != 1)
		usage();
	else
		disk.name = argv[0];

	/* No supplied geometry */
	usermetrics = NULL;

	/* Get the geometry */
	disk.real = NULL;
	if (DISK_getmetrics(&disk, usermetrics))
		errx(1, "Can't get disk geometry.");

	/* Print out current GPTs on disk */
	if ((i_flag + m_flag) == 0)
		exit(USER_print_disk(&disk));

	/* Parse gpt template, to pass on later */
	if (gptfile != NULL && (fd = open(gptfile, O_RDONLY)) == -1) {
		warn("%s", gptfile);
		warnx("using builtin GPT");
		gptfile = NULL;
	}
	if (gptfile == NULL) {
		memcpy(gpt_buf, builtin_gpt, sizeof(gpt_buf));
	} else {
		error = GPT_read(fd, 0, gpt_buf);
		if (error == -1)
			err(1, "Unable to read GPT");
		close(fd);
	}
	GPT_parse(&disk, gpt_buf, 0, 0, &gpt);

	/* Now do what we are supposed to */
	if (i_flag)
		if (USER_init(&disk, &gpt) == -1)
			err(1, "error initializing GPT");

	if (m_flag)
		USER_modify(&disk, &gpt, 0, 0);

	return (0);
}

