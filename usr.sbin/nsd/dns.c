/*
 * dns.c -- DNS definitions.
 *
 * Copyright (c) 2001-2011, NLnet Labs. All rights reserved.
 *
 * See LICENSE for the license.
 *
 */

#include "config.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "dns.h"
#include "zonec.h"
#include "zparser.h"

/* Taken from RFC 1035, section 3.2.4.  */
static lookup_table_type dns_rrclasses[] = {
	{ CLASS_IN, "IN" },	/* the Internet */
	{ CLASS_CS, "CS" },	/* the CSNET class (Obsolete) */
	{ CLASS_CH, "CH" },	/* the CHAOS class */
	{ CLASS_HS, "HS" },	/* Hesiod */
	{ 0, NULL }
};

static rrtype_descriptor_type rrtype_descriptors[(RRTYPE_DESCRIPTORS_LENGTH+1)] = {
	/* 0 */
	{ 0, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 1 */
	{ TYPE_A, "A", T_A, 1, 1,
	  { RDATA_WF_A }, { RDATA_ZF_A } },
	/* 2 */
	{ TYPE_NS, "NS", T_NS, 1, 1,
	  { RDATA_WF_COMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 3 */
	{ TYPE_MD, "MD", T_MD, 1, 1,
	  { RDATA_WF_UNCOMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 4 */
	{ TYPE_MF, "MF", T_MF, 1, 1,
	  { RDATA_WF_UNCOMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 5 */
	{ TYPE_CNAME, "CNAME", T_CNAME, 1, 1,
	  { RDATA_WF_COMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 6 */
	{ TYPE_SOA, "SOA", T_SOA, 7, 7,
	  { RDATA_WF_COMPRESSED_DNAME, RDATA_WF_COMPRESSED_DNAME, RDATA_WF_LONG,
	    RDATA_WF_LONG, RDATA_WF_LONG, RDATA_WF_LONG, RDATA_WF_LONG },
	  { RDATA_ZF_DNAME, RDATA_ZF_DNAME, RDATA_ZF_PERIOD, RDATA_ZF_PERIOD,
	    RDATA_ZF_PERIOD, RDATA_ZF_PERIOD, RDATA_ZF_PERIOD } },
	/* 7 */
	{ TYPE_MB, "MB", T_MB, 1, 1,
	  { RDATA_WF_COMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 8 */
	{ TYPE_MG, "MG", T_MG, 1, 1,
	  { RDATA_WF_COMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 9 */
	{ TYPE_MR, "MR", T_MR, 1, 1,
	  { RDATA_WF_COMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 10 */
	{ TYPE_NULL, "NULL", T_UTYPE, 1, 1,
	  { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 11 */
	{ TYPE_WKS, "WKS", T_WKS, 2, 2,
	  { RDATA_WF_A, RDATA_WF_BINARY },
	  { RDATA_ZF_A, RDATA_ZF_SERVICES } },
	/* 12 */
	{ TYPE_PTR, "PTR", T_PTR, 1, 1,
	  { RDATA_WF_COMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 13 */
	{ TYPE_HINFO, "HINFO", T_HINFO, 2, 2,
	  { RDATA_WF_TEXT, RDATA_WF_TEXT }, { RDATA_ZF_TEXT, RDATA_ZF_TEXT } },
	/* 14 */
	{ TYPE_MINFO, "MINFO", T_MINFO, 2, 2,
	  { RDATA_WF_COMPRESSED_DNAME, RDATA_WF_COMPRESSED_DNAME },
	  { RDATA_ZF_DNAME, RDATA_ZF_DNAME } },
	/* 15 */
	{ TYPE_MX, "MX", T_MX, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_COMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_DNAME } },
	/* 16 */
	{ TYPE_TXT, "TXT", T_TXT, 1, 1,
	  { RDATA_WF_TEXTS },
	  { RDATA_ZF_TEXTS } },
	/* 17 */
	{ TYPE_RP, "RP", T_RP, 2, 2,
	  { RDATA_WF_COMPRESSED_DNAME, RDATA_WF_COMPRESSED_DNAME },
	  { RDATA_ZF_DNAME, RDATA_ZF_DNAME } },
	/* 18 */
	{ TYPE_AFSDB, "AFSDB", T_AFSDB, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_COMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_DNAME } },
	/* 19 */
	{ TYPE_X25, "X25", T_X25, 1, 1,
	  { RDATA_WF_TEXT },
	  { RDATA_ZF_TEXT } },
	/* 20 */
	{ TYPE_ISDN, "ISDN", T_ISDN, 1, 2,
	  { RDATA_WF_TEXT, RDATA_WF_TEXT },
	  { RDATA_ZF_TEXT, RDATA_ZF_TEXT } },
	/* 21 */
	{ TYPE_RT, "RT", T_RT, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_COMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_DNAME } },
	/* 22 */
	{ TYPE_NSAP, "NSAP", T_NSAP, 1, 1,
	  { RDATA_WF_BINARY },
	  { RDATA_ZF_NSAP } },
	/* 23 */
	{ 23, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 24 */
	{ TYPE_SIG, "SIG", T_SIG, 9, 9,
	  { RDATA_WF_SHORT, RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_LONG,
	    RDATA_WF_LONG, RDATA_WF_LONG, RDATA_WF_SHORT,
	    RDATA_WF_UNCOMPRESSED_DNAME, RDATA_WF_BINARY },
	  { RDATA_ZF_RRTYPE, RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_PERIOD,
	    RDATA_ZF_TIME, RDATA_ZF_TIME, RDATA_ZF_SHORT, RDATA_ZF_DNAME,
	    RDATA_ZF_BASE64 } },
	/* 25 */
	{ TYPE_KEY, "KEY", T_KEY, 4, 4,
	  { RDATA_WF_SHORT, RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_BINARY },
	  { RDATA_ZF_SHORT, RDATA_ZF_BYTE, RDATA_ZF_ALGORITHM,
	    RDATA_ZF_BASE64 } },
	/* 26 */
	{ TYPE_PX, "PX", T_PX, 3, 3,
	  { RDATA_WF_SHORT, RDATA_WF_UNCOMPRESSED_DNAME,
	    RDATA_WF_UNCOMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_DNAME, RDATA_ZF_DNAME } },
	/* 27 */
	{ 27, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 28 */
	{ TYPE_AAAA, "AAAA", T_AAAA, 1, 1,
	  { RDATA_WF_AAAA },
	  { RDATA_ZF_AAAA } },
	/* 29 */
	{ TYPE_LOC, "LOC", T_LOC, 1, 1,
	  { RDATA_WF_BINARY },
	  { RDATA_ZF_LOC } },
	/* 30 */
	{ TYPE_NXT, "NXT", T_NXT, 2, 2,
	  { RDATA_WF_UNCOMPRESSED_DNAME, RDATA_WF_BINARY },
	  { RDATA_ZF_DNAME, RDATA_ZF_NXT } },
	/* 31 */
	{ 31, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 32 */
	{ 32, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 33 */
	{ TYPE_SRV, "SRV", T_SRV, 4, 4,
	  { RDATA_WF_SHORT, RDATA_WF_SHORT, RDATA_WF_SHORT,
	    RDATA_WF_UNCOMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_SHORT, RDATA_ZF_SHORT, RDATA_ZF_DNAME } },
	/* 34 */
	{ 34, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 35 */
	{ TYPE_NAPTR, "NAPTR", T_NAPTR, 6, 6,
	  { RDATA_WF_SHORT, RDATA_WF_SHORT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_UNCOMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_SHORT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_DNAME } },
	/* 36 */
	{ TYPE_KX, "KX", T_KX, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_UNCOMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_DNAME } },
	/* 37 */
	{ TYPE_CERT, "CERT", T_CERT, 4, 4,
	  { RDATA_WF_SHORT, RDATA_WF_SHORT, RDATA_WF_BYTE, RDATA_WF_BINARY },
	  { RDATA_ZF_CERTIFICATE_TYPE, RDATA_ZF_SHORT, RDATA_ZF_ALGORITHM,
	    RDATA_ZF_BASE64 } },
	/* 38 */
	{ TYPE_A6, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 39 */
	{ TYPE_DNAME, "DNAME", T_DNAME, 1, 1,
	  { RDATA_WF_UNCOMPRESSED_DNAME }, { RDATA_ZF_DNAME } },
	/* 40 */
	{ 40, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 41 */
	{ TYPE_OPT, "OPT", T_UTYPE, 1, 1,
	  { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 42 */
	{ TYPE_APL, "APL", T_APL, 0, MAXRDATALEN,
	  { RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL,
	    RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL, RDATA_WF_APL },
	  { RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL,
	    RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL, RDATA_ZF_APL } },
	/* 43 */
	{ TYPE_DS, "DS", T_DS, 4, 4,
	  { RDATA_WF_SHORT, RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_BINARY },
	  { RDATA_ZF_SHORT, RDATA_ZF_ALGORITHM, RDATA_ZF_BYTE, RDATA_ZF_HEX } },
	/* 44 */
	{ TYPE_SSHFP, "SSHFP", T_SSHFP, 3, 3,
	  { RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_BINARY },
	  { RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_HEX } },
	/* 45 */
	{ TYPE_IPSECKEY, "IPSECKEY", T_IPSECKEY, 4, 5,
	  { RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_IPSECGATEWAY,
	    RDATA_WF_BINARY },
	  { RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_IPSECGATEWAY,
	    RDATA_ZF_BASE64 } },
	/* 46 */
	{ TYPE_RRSIG, "RRSIG", T_RRSIG, 9, 9,
	  { RDATA_WF_SHORT, RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_LONG,
	    RDATA_WF_LONG, RDATA_WF_LONG, RDATA_WF_SHORT,
	    RDATA_WF_LITERAL_DNAME, RDATA_WF_BINARY },
	  { RDATA_ZF_RRTYPE, RDATA_ZF_ALGORITHM, RDATA_ZF_BYTE, RDATA_ZF_PERIOD,
	    RDATA_ZF_TIME, RDATA_ZF_TIME, RDATA_ZF_SHORT,
		RDATA_ZF_LITERAL_DNAME, RDATA_ZF_BASE64 } },
	/* 47 */
	{ TYPE_NSEC, "NSEC", T_NSEC, 2, 2,
	  { RDATA_WF_LITERAL_DNAME, RDATA_WF_BINARY },
	  { RDATA_ZF_LITERAL_DNAME, RDATA_ZF_NSEC } },
	/* 48 */
	{ TYPE_DNSKEY, "DNSKEY", T_DNSKEY, 4, 4,
	  { RDATA_WF_SHORT, RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_BINARY },
	  { RDATA_ZF_SHORT, RDATA_ZF_BYTE, RDATA_ZF_ALGORITHM,
	    RDATA_ZF_BASE64 } },
	/* 49 */
	{ TYPE_DHCID, "DHCID", T_DHCID, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_BASE64 } },
	/* 50 */
	{ TYPE_NSEC3, "NSEC3", T_NSEC3, 6, 6,
	  { RDATA_WF_BYTE, /* hash type */
	    RDATA_WF_BYTE, /* flags */
	    RDATA_WF_SHORT, /* iterations */
	    RDATA_WF_BINARYWITHLENGTH, /* salt */
	    RDATA_WF_BINARYWITHLENGTH, /* next hashed name */
	    RDATA_WF_BINARY /* type bitmap */ },
	  { RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_SHORT, RDATA_ZF_HEX_LEN,
	    RDATA_ZF_BASE32, RDATA_ZF_NSEC } },
	/* 51 */
	{ TYPE_NSEC3PARAM, "NSEC3PARAM", T_NSEC3PARAM, 4, 4,
	  { RDATA_WF_BYTE, /* hash type */
	    RDATA_WF_BYTE, /* flags */
	    RDATA_WF_SHORT, /* iterations */
	    RDATA_WF_BINARYWITHLENGTH /* salt */ },
	  { RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_SHORT, RDATA_ZF_HEX_LEN } },
	/* 52 */
	{ TYPE_TLSA, "TLSA", T_TLSA, 4, 4,
	  { RDATA_WF_BYTE, /* usage */
	    RDATA_WF_BYTE, /* selector */
	    RDATA_WF_BYTE, /* matching type */
	    RDATA_WF_BINARY }, /* certificate association data */
	  { RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_BYTE, RDATA_ZF_HEX } },
	/* 53 */
	{ 53, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 54 */
	{ 54, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 55 */
	{ 55, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 56 */
	{ 56, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 57 */
	{ 57, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 58 */
	{ 58, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 59 */
	{ 59, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 60 */
	{ 60, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 61 */
	{ 61, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 62 */
	{ 62, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 63 */
	{ 63, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 64 */
	{ 64, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 65 */
	{ 65, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 66 */
	{ 66, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 67 */
	{ 67, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 68 */
	{ 68, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 69 */
	{ 69, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 70 */
	{ 70, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 71 */
	{ 71, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 72 */
	{ 72, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 73 */
	{ 73, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 74 */
	{ 74, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 75 */
	{ 75, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 76 */
	{ 76, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 77 */
	{ 77, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 78 */
	{ 78, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 79 */
	{ 79, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 80 */
	{ 80, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 81 */
	{ 81, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 82 */
	{ 82, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 83 */
	{ 83, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 84 */
	{ 84, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 85 */
	{ 85, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 86 */
	{ 86, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 87 */
	{ 87, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 88 */
	{ 88, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 89 */
	{ 89, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 90 */
	{ 90, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 91 */
	{ 91, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 92 */
	{ 92, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 93 */
	{ 93, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 94 */
	{ 94, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 95 */
	{ 95, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 96 */
	{ 96, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 97 */
	{ 97, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 98 */
	{ 98, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 99 */
	{ TYPE_SPF, "SPF", T_SPF, 1, MAXRDATALEN,
	  { RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT,
	    RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT, RDATA_WF_TEXT },
	  { RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT,
	    RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT, RDATA_ZF_TEXT } },
	/* 100 */
	{ 100, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 101 */
	{ 101, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 102 */
	{ 102, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 103 */
	{ 103, NULL, T_UTYPE, 1, 1, { RDATA_WF_BINARY }, { RDATA_ZF_UNKNOWN } },
	/* 104 */
	{ TYPE_NID, "NID", T_NID, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_ILNP64 },
	  { RDATA_ZF_SHORT, RDATA_ZF_ILNP64 } },
	/* 105 */
	{ TYPE_L32, "L32", T_L32, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_A },
	  { RDATA_ZF_SHORT, RDATA_ZF_A } },
	/* 106 */
	{ TYPE_L64, "L64", T_L64, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_ILNP64 },
	  { RDATA_ZF_SHORT, RDATA_ZF_ILNP64 } },
	/* 107 */
	{ TYPE_LP, "LP", T_LP, 2, 2,
	  { RDATA_WF_SHORT, RDATA_WF_UNCOMPRESSED_DNAME },
	  { RDATA_ZF_SHORT, RDATA_ZF_DNAME } },
	/* 32769 */
	{ TYPE_DLV, "DLV", T_DLV, 4, 4,
	  { RDATA_WF_SHORT, RDATA_WF_BYTE, RDATA_WF_BYTE, RDATA_WF_BINARY },
	  { RDATA_ZF_SHORT, RDATA_ZF_ALGORITHM, RDATA_ZF_BYTE, RDATA_ZF_HEX } },
};

rrtype_descriptor_type *
rrtype_descriptor_by_type(uint16_t type)
{
	if (type < RRTYPE_DESCRIPTORS_LENGTH)
		return &rrtype_descriptors[type];
	else if (type == TYPE_DLV)
		return &rrtype_descriptors[PSEUDO_TYPE_DLV];
	return &rrtype_descriptors[0];
}

rrtype_descriptor_type *
rrtype_descriptor_by_name(const char *name)
{
	int i;

	for (i = 0; i < RRTYPE_DESCRIPTORS_LENGTH; ++i) {
		if (rrtype_descriptors[i].name
		    && strcasecmp(rrtype_descriptors[i].name, name) == 0)
		{
			return &rrtype_descriptors[i];
		}
	}

	if (rrtype_descriptors[PSEUDO_TYPE_DLV].name
	    && strcasecmp(rrtype_descriptors[PSEUDO_TYPE_DLV].name, name) == 0)
	{
		return &rrtype_descriptors[PSEUDO_TYPE_DLV];
	}

	return NULL;
}

const char *
rrtype_to_string(uint16_t rrtype)
{
	static char buf[20];
	rrtype_descriptor_type *descriptor = rrtype_descriptor_by_type(rrtype);
	if (descriptor->name) {
		return descriptor->name;
	} else {
		snprintf(buf, sizeof(buf), "TYPE%d", (int) rrtype);
		return buf;
	}
}

/*
 * Lookup the type in the ztypes lookup table.  If not found, check if
 * the type uses the "TYPExxx" notation for unknown types.
 *
 * Return 0 if no type matches.
 */
uint16_t
rrtype_from_string(const char *name)
{
        char *end;
        long rrtype;
	rrtype_descriptor_type *entry;

	entry = rrtype_descriptor_by_name(name);
	if (entry) {
		return entry->type;
	}

	if (strlen(name) < 5)
		return 0;

	if (strncasecmp(name, "TYPE", 4) != 0)
		return 0;

	if (!isdigit((int)name[4]))
		return 0;

	/* The rest from the string must be a number.  */
	rrtype = strtol(name + 4, &end, 10);
	if (*end != '\0')
		return 0;
	if (rrtype < 0 || rrtype > 65535L)
		return 0;

        return (uint16_t) rrtype;
}

const char *
rrclass_to_string(uint16_t rrclass)
{
	static char buf[20];
	lookup_table_type *entry = lookup_by_id(dns_rrclasses, rrclass);
	if (entry) {
		assert(strlen(entry->name) < sizeof(buf));
		strlcpy(buf, entry->name, sizeof(buf));
	} else {
		snprintf(buf, sizeof(buf), "CLASS%d", (int) rrclass);
	}
	return buf;
}

uint16_t
rrclass_from_string(const char *name)
{
        char *end;
        long rrclass;
	lookup_table_type *entry;

	entry = lookup_by_name(dns_rrclasses, name);
	if (entry) {
		return (uint16_t) entry->id;
	}

	if (strlen(name) < 6)
		return 0;

	if (strncasecmp(name, "CLASS", 5) != 0)
		return 0;

	if (!isdigit((int)name[5]))
		return 0;

	/* The rest from the string must be a number.  */
	rrclass = strtol(name + 5, &end, 10);
	if (*end != '\0')
		return 0;
	if (rrclass < 0 || rrclass > 65535L)
		return 0;

	return (uint16_t) rrclass;
}
