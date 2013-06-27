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

#ifndef _GPT_H
#define _GPT_H

#include "part.h"

/* Various constants */
#define GPT_SIGNATURE	0x5452415020494645
#define GPT_REVISION	0x10000
#define GPT_PARTITIONS	128
#define GPT_DOSACTIVE	0x2

/* GPT header */
typedef struct __attribute__((packed)) _gpt_header_t {
	u_int64_t signature;			/* "EFI PART" */
	u_int32_t revision;			/* GPT Version 1.0: 0x00000100 */
	u_int32_t header_size;			/* Little-Endian */
	u_int32_t header_checksum;		/* CRC32: with this field as 0 */
	u_int32_t reserved;			/* always zero */
	u_int64_t current_lba;			/* location of this header */
	u_int64_t backup_lba;			/* location of backup header */
	u_int64_t start_lba;			/* first usable LBA (after this header) */
	u_int64_t end_lba;			/* last usable LBA (before backup header) */
	uuid_t    guid;				/* disk GUID / UUID */
	u_int64_t partitions_lba;		/* location of partition entries */
	u_int32_t partitions_num;		/* # of reserved entry space, usually 128 */
	u_int32_t partitions_size;		/* size per entry */
	u_int32_t partitions_checksum;		/* checksum of all entries */
} gpt_header_t;

/* GPT data struct */
typedef struct _gpt_t {
	gpt_header_t *header;
	gpt_partition_t *part;
} gpt_t;

/* Prototypes */
void GPT_print_disk(char *);
void GPT_print(gpt_t *, char *);
void GPT_parse(disk_t *, char *, off_t, off_t, gpt_t *);
void GPT_make(gpt_t *, char *);
int GPT_init(disk_t *, gpt_t *);
int GPT_load(int, disk_t *, gpt_t *, int);
int GPT_store(int, disk_t *, gpt_t *);
int GPT_read(int, off_t, char *);
int GPT_write(int, off_t, char *);
void GPT_pcopy(disk_t *, gpt_t *);
void GPT_free(gpt_t *);

/* Sanity check */
#include <sys/param.h>
#if (DEV_BSIZE != 512)
#error "DEV_BSIZE != 512, somebody better fix me!"
#endif

#endif /* _GPT_H */

