/*	$OpenBSD: usbdevs.h,v 1.15 2000/04/04 22:52:30 aaron Exp $	*/

/*
 * THIS FILE IS AUTOMATICALLY GENERATED.  DO NOT EDIT.
 *
 * generated from:
 *	OpenBSD:
 */
/* $NetBSD: usbdevs,v 1.88 2000/04/03 12:09:43 augustss Exp $ */

/*
 * Copyright (c) 1998, 1999, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (augustss@carlstedt.se) at
 * Carlstedt Research & Technology.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * List of known USB vendors
 */

#define	USB_VENDOR_AOX	0x03e8		/* AOX, Inc. */
#define	USB_VENDOR_ATMEL	0x03eb		/* Atmel Corp. */
#define	USB_VENDOR_HP	0x03f0		/* Hewlett Packard */
#define	USB_VENDOR_ADAPTEC	0x03f3		/* Adaptec, Inc. */
#define	USB_VENDOR_ACERLABS	0x0402		/* Acer Labs, Inc. */
#define	USB_VENDOR_FUTURE	0x0403		/* Future Technology Devices */
#define	USB_VENDOR_NEC	0x0409		/* NEC */
#define	USB_VENDOR_KODAK	0x040a		/* Eastman Kodak Corp. */
#define	USB_VENDOR_MELCO	0x0411		/* Melco, Inc. */
#define	USB_VENDOR_ADI	0x0422		/* ADI Systems, Inc. */
#define	USB_VENDOR_CATC	0x0423		/* Computer Access Technology Corp. */
#define	USB_VENDOR_GRAVIS	0x0428		/* Advanced Gravis Computer Tech. Ltd. */
#define	USB_VENDOR_SUN	0x0430		/* Sun Microsystems */
#define	USB_VENDOR_AMD	0x0438		/* Advanced Micro Devices */
#define	USB_VENDOR_LEXMARK	0x043d		/* Lexmark International, Inc. */
#define	USB_VENDOR_NANAO	0x0440		/* NANAO Corp. */
#define	USB_VENDOR_ALPS	0x044e		/* Alps Electric Co., Ltd. */
#define	USB_VENDOR_THRUST	0x044f		/* Thrustmaster */
#define	USB_VENDOR_TI	0x0451		/* Texas Instruments */
#define	USB_VENDOR_ANALOGDEVICES	0x0456		/* Analog Devices, Inc. */
#define	USB_VENDOR_KYE	0x0458		/* KYE Systems Corp. */
#define	USB_VENDOR_MICROSOFT	0x045e		/* Microsoft */
#define	USB_VENDOR_PRIMAX	0x0461		/* Primax Electronics */
#define	USB_VENDOR_AMP	0x0464		/* AMP, Inc. */
#define	USB_VENDOR_CHERRY	0x046a		/* Cherry Mikroschalter GmbH */
#define	USB_VENDOR_MEGATRENDS	0x046b		/* American Megatrends */
#define	USB_VENDOR_LOGITECH	0x046d		/* Logitech, Inc. */
#define	USB_VENDOR_BTC	0x046e		/* Behavior Tech. Computer */
#define	USB_VENDOR_PHILIPS	0x0471		/* Philips */
#define	USB_VENDOR_CONNECTIX	0x0478		/* Connectix Corp. */
#define	USB_VENDOR_KENSINGTON	0x047d		/* Kensington */
#define	USB_VENDOR_LUCENT	0x047e		/* Lucent */
#define	USB_VENDOR_STMICRO	0x0483		/* STMicroelectronics */
#define	USB_VENDOR_ACERP	0x04a5		/* Acer Peripherals, Inc. */
#define	USB_VENDOR_CANON	0x04a9		/* Canon, Inc. */
#define	USB_VENDOR_CYPRESS	0x04b4		/* Cypress Semiconductor */
#define	USB_VENDOR_EPSON	0x04b8		/* Seiko Epson Corp. */
#define	USB_VENDOR_IODATA	0x04bb		/* I/O Data */
#define	USB_VENDOR_3COMUSR	0x04c1		/* U.S. Robotics */
#define	USB_VENDOR_KONICA	0x04c8		/* Konica Corp. */
#define	USB_VENDOR_ALTEC	0x04d2		/* Altec Lansing Technologies, Inc. */
#define	USB_VENDOR_SHUTTLE	0x04e6		/* Shuttle Technology */
#define	USB_VENDOR_ANNABOOKS	0x04ed		/* Annabooks */
#define	USB_VENDOR_CHICONY	0x04f2		/* Chicony Electronics Co., Ltd. */
#define	USB_VENDOR_BROTHER	0x04f9		/* Brother Industries, Ltd. */
#define	USB_VENDOR_DALLAS	0x04fa		/* Dallas Semiconductor */
#define	USB_VENDOR_ACER	0x0502		/* Acer, Inc. */
#define	USB_VENDOR_3COM	0x0506		/* 3Com Corp. */
#define	USB_VENDOR_AZTECH	0x0509		/* Aztech Systems Ltd */
#define	USB_VENDOR_BELKIN	0x050d		/* Belkin Components */
#define	USB_VENDOR_KAWATSU	0x050f		/* Kawatsu Semiconductor, Inc. */
#define	USB_VENDOR_APC	0x051d		/* American Power Conversion */
#define	USB_VENDOR_CONNECTEK	0x0522		/* Advanced Connectek USA, Inc. */
#define	USB_VENDOR_NETCHIP	0x0525		/* NetChip Technology */
#define	USB_VENDOR_ALTRA	0x0527		/* ALTRA */
#define	USB_VENDOR_ATI	0x0528		/* ATI Technologies, Inc. */
#define	USB_VENDOR_AKS	0x0529		/* Aladdin Knowledge Systems */
#define	USB_VENDOR_UNIACCESS	0x0540		/* Universal Access */
#define	USB_VENDOR_ANCHOR	0x0547		/* Anchor Chips, Inc. */
#define	USB_VENDOR_VISION	0x0553		/* VLSI Vision Ltd. */
#define	USB_VENDOR_ASAHIKASEI	0x0556		/* Asahi Kasei Microsystems Co., Ltd */
#define	USB_VENDOR_ATEN	0x0557		/* ATEN International Co. Ltd. */
#define	USB_VENDOR_MUSTEK	0x055f		/* Mustek Systems, Inc. */
#define	USB_VENDOR_TELEX	0x0562		/* Telex Communications, Inc. */
#define	USB_VENDOR_PERACOM	0x0565		/* Peracom Networks, Inc. */
#define	USB_VENDOR_WACOM	0x056a		/* WACOM Corp. Ltd. */
#define	USB_VENDOR_ETEK	0x056c		/* e-TEK Labs */
#define	USB_VENDOR_EIZO	0x056d		/* EIZO */
#define	USB_VENDOR_ELECOM	0x056e		/* Elecom Corp. Ltd. */
#define	USB_VENDOR_BAFO	0x0576		/* BAFO/Quality Computer Accessories */
#define	USB_VENDOR_YEDATA	0x057b		/* Y-E Data */
#define	USB_VENDOR_AVM	0x057c		/* AVM GmbH */
#define	USB_VENDOR_QUICKSHOT	0x057f		/* Quickshot */
#define	USB_VENDOR_ROCKFIRE	0x0583		/* Rockfire */
#define	USB_VENDOR_ALCOR	0x058f		/* Alcor Micro, Inc. */
#define	USB_VENDOR_IOMEGA	0x059b		/* Iomega Corp. */
#define	USB_VENDOR_ATREND	0x059c		/* A-Trend Technology Co., Ltd. */
#define	USB_VENDOR_AID	0x059d		/* Advanced Input Devices */
#define	USB_VENDOR_OMNIVISION	0x05a9		/* OmniVision */
#define	USB_VENDOR_INSYSTEM	0x05ab		/* In-System Design */
#define	USB_VENDOR_APPLE	0x05ac		/* Apple Computer */
#define	USB_VENDOR_DIGI	0x05c5		/* Digi International */
#define	USB_VENDOR_QTRONIX	0x05c7		/* Qtronix Corp */
#define	USB_VENDOR_ELSA	0x05cc		/* ELSA Gmbh */
#define	USB_VENDOR_BRAINBOXES	0x05d1		/* Brainboxes Limited */
#define	USB_VENDOR_AXIOHM	0x05d9		/* Axiohm Transaction Solutions */
#define	USB_VENDOR_EIZONANAO	0x05e7		/* EIZO Nanao */
#define	USB_VENDOR_KLSI	0x05e9		/* Kawasaki LSI */
#define	USB_VENDOR_ANKO	0x05ef		/* Anko Electronic Co., Ltd. */
#define	USB_VENDOR_PIENGINEERING	0x05f3		/* P.I. Engineering */
#define	USB_VENDOR_AOC	0x05f6		/* AOC International */
#define	USB_VENDOR_CHIC	0x05fe		/* Chic Technology */
#define	USB_VENDOR_BARCO	0x0600		/* Barco Display Systems */
#define	USB_VENDOR_BRIDGE	0x0607		/* Bridge Information Co., Ltd. */
#define	USB_VENDOR_SOLIDYEAR	0x060b		/* Solid Year */
#define	USB_VENDOR_BIORAD	0x0614		/* Bio-Rad Laboratories */
#define	USB_VENDOR_MACALLY	0x0618		/* Macally */
#define	USB_VENDOR_ACTLABS	0x061c		/* Act Labs, Ltd. */
#define	USB_VENDOR_ALARIS	0x0620		/* Alaris, Inc. */
#define	USB_VENDOR_APEX	0x0624		/* Apex, Inc. */
#define	USB_VENDOR_AVISION	0x0638		/* Avision, Inc. */
#define	USB_VENDOR_LINKSYS	0x066b		/* Linksys, Inc. */
#define	USB_VENDOR_ACERSA	0x066e		/* Acer Semiconductor America, Inc. */
#define	USB_VENDOR_AIWA	0x0677		/* Aiwa Co., Ltd. */
#define	USB_VENDOR_ACARD	0x0678		/* ACARD Technology Corp. */
#define	USB_VENDOR_ADVANCELOGIC	0x0680		/* Avance Logic, Inc. */
#define	USB_VENDOR_ASKEY	0x069a		/* Askey Computer Corp. */
#define	USB_VENDOR_ALCATELT	0x06b9		/* Alcatel Telecom */
#define	USB_VENDOR_AGFA	0x06bd		/* AGFA-Gevaert NV */
#define	USB_VENDOR_ASIAMD	0x06be		/* Asia Microelectronic Development, Inc. */
#define	USB_VENDOR_BIZLINK	0x06c4		/* Bizlink International Corp. */
#define	USB_VENDOR_AASHIMA	0x06d6		/* Aashima Technology B.V. */
#define	USB_VENDOR_MULTITECH	0x06e0		/* MultiTech */
#define	USB_VENDOR_ADS	0x06e1		/* ADS Technologies, Inc. */
#define	USB_VENDOR_ALCATELM	0x06e4		/* Alcatel Microelectronics */
#define	USB_VENDOR_SIRIUS	0x06ea		/* Sirius Technologies */
#define	USB_VENDOR_BOSTON	0x06fd		/* Boston Acoustics */
#define	USB_VENDOR_SMC	0x0707		/* Standard Microsystems Corp */
#define	USB_VENDOR_AUREAL	0x0755		/* Aureal Semiconductor */
#define	USB_VENDOR_MIDIMAN	0x0763		/* Midiman */
#define	USB_VENDOR_SANDISK	0x0781		/* SanDisk Corp */
#define	USB_VENDOR_BRIMAX	0x078e		/* Brimax, Inc. */
#define	USB_VENDOR_AXIS	0x0792		/* Axis Communications AB */
#define	USB_VENDOR_ABL	0x0794		/* ABL Electronics Corp. */
#define	USB_VENDOR_ALFADATA	0x079d		/* Alfadata Computer Corp. */
#define	USB_VENDOR_BE	0x07a4		/* Be, Inc. */
#define	USB_VENDOR_ADMTEK	0x07a6		/* ADMtek, Inc. */
#define	USB_VENDOR_COREGA	0x07aa		/* Corega */
#define	USB_VENDOR_FREECOM	0x07ab		/* Freecom */
#define	USB_VENDOR_ABOCOM	0x07b8		/* AboCom Systems, Inc. */
#define	USB_VENDOR_KEISOKUGIKEN	0x07c1		/* Keisokugiken Corp. */
#define	USB_VENDOR_APG	0x07c5		/* APG Cash Drawer */
#define	USB_VENDOR_BUG	0x07c8		/* B.U.G., Inc. */
#define	USB_VENDOR_ALLIEDTELESYN	0x07c9		/* Allied Telesyn International */
#define	USB_VENDOR_AVERMEDIA	0x07ca		/* AVerMedia Technologies, Inc. */
#define	USB_VENDOR_SIIG	0x07cc		/* SIIG */
#define	USB_VENDOR_APTIO	0x07d2		/* Aptio Products, Inc. */
#define	USB_VENDOR_ARASAN	0x07da		/* Arasan Chip Systems */
#define	USB_VENDOR_ALLIEDCABLE	0x07e6		/* Allied Cable Corp. */
#define	USB_VENDOR_ZOOM	0x0803		/* Zoom Telephonics, Inc. */
#define	USB_VENDOR_BROADLOGIC	0x0827		/* BroadLogic, Inc. */
#define	USB_VENDOR_HANDSPRING	0x082d		/* Handspring, Inc. */
#define	USB_VENDOR_ACTIONSTAR	0x0835		/* Action Star Enterprise Co., Ltd. */
#define	USB_VENDOR_PALM	0x0830		/* Palm Computing, Inc. */
#define	USB_VENDOR_ACCTON	0x083a		/* Accton Technology Corp. */
#define	USB_VENDOR_DIAMOND	0x0841		/* Diamond */
#define	USB_VENDOR_NETGEAR	0x0846		/* BayNETGEAR, Inc. */
#define	USB_VENDOR_ACTIVEWIRE	0x0854		/* ActiveWire, Inc. */
#define	USB_VENDOR_ADESSOKBTEK	0x087c		/* ADESSO/Kbtek America, Inc. */
#define	USB_VENDOR_APT	0x0880		/* APT Technologies, Inc. */
#define	USB_VENDOR_BOCARESEARCH	0x0885		/* Boca Research, Inc. */
#define	USB_VENDOR_ANDREA	0x08a8		/* Andrea Electronics */
#define	USB_VENDOR_BURRBROWN	0x08bb		/* Burr-Brown Japan, Ltd. */
#define	USB_VENDOR_2WIRE	0x08c8		/* 2Wire, Inc */
#define	USB_VENDOR_AIPTEK	0x08ca		/* AIPTEK International, Inc. */
#define	USB_VENDOR_BILLIONTON	0x08dd		/* Billionton Systems, Inc. */
#define	USB_VENDOR_AUTHENTEC	0x08ff		/* AuthenTec, Inc. */
#define	USB_VENDOR_ALATION	0x0910		/* Alation Systems, Inc. */
#define	USB_VENDOR_BIOMETRIC	0x0929		/* American Biometric Company */
#define	USB_VENDOR_BLUEWATER	0x0956		/* BlueWater Systems, Inc. */
#define	USB_VENDOR_AGILENT	0x0957		/* Agilent Technologies, Inc. */
#define	USB_VENDOR_ADIRONDACK	0x0976		/* Adirondack Wire & Cable */
#define	USB_VENDOR_BECKHOFF	0x0978		/* Beckhoff Gmbh */
#define	USB_VENDOR_ALTIUS	0x09b3		/* Altius Solutions, Inc. */
#define	USB_VENDOR_ARRIS	0x09c1		/* Arris Interactive LLC */
#define	USB_VENDOR_ACTIVCARD	0x09c3		/* ACTIVCARD, INC. */
#define	USB_VENDOR_ACTISYS	0x09c4		/* ACTiSYS Corp. */
#define	USB_VENDOR_AFOURTECH	0x09da		/* A-FOUR TECH CO., LTD. */
#define	USB_VENDOR_AIMEX	0x09dc		/* AIMEX Corp. */
#define	USB_VENDOR_ADDONICS	0x09df		/* Addonics Technologies Corp. */
#define	USB_VENDOR_AKAI	0x09e8		/* AKAI professional M.I. Corp. */
#define	USB_VENDOR_ARESCOM	0x09f5		/* ARESCOM */
#define	USB_VENDOR_BAY	0x09f9		/* Bay Associates */
#define	USB_VENDOR_ALTERA	0x09fb		/* Altera */
#define	USB_VENDOR_ASAHIOPTICAL	0x0a17		/* Asahi Optical Co., Ltd. */
#define	USB_VENDOR_BOCASYSTEMS	0x0a43		/* Boca Systems, Inc. */
#define	USB_VENDOR_BROADCOM	0x0a5c		/* Broadcom Corp. */
#define	USB_VENDOR_MOTOROLA	0x1063		/* Motorola */
#define	USB_VENDOR_PLX	0x10b5		/* PLX */
#define	USB_VENDOR_INSIDEOUT	0x1608		/* Inside Out Networks */
#define	USB_VENDOR_ENTREGA	0x1645		/* Entrega */
#define	USB_VENDOR_ACTIONTEC	0x1668		/* Actiontec Electronics, Inc. */
#define	USB_VENDOR_DLINK	0x2001		/* D-Link Corp */
#define	USB_VENDOR_INTEL	0x8086		/* Intel */

/*
 * List of known products.  Grouped by vendor.
 */

/* 3Com products */
#define	USB_PRODUCT_3COM_HOMECONN	0x009d		/* HomeConnect USB Camera */
#define	USB_PRODUCT_3COM_3C19250	0x03E8		/* 3C19250 Ethernet adapter */
#define	USB_PRODUCT_3COM_USR56K	0x3021		/* U.S.Robotics 56000 Voice Faxmodem Pro */

#define	USB_PRODUCT_3COM_3C460	0x11f8		/* HomeConnect Ethernet USB Adapter */

#define	USB_PRODUCT_3COMUSR_OFFICECONN	0x0082		/* 3Com OfficeConnect Analog Modem */
#define	USB_PRODUCT_3COMUSR_USRISDN	0x008f		/* 3Com U.S. Robotics Pro ISDN TA */
#define	USB_PRODUCT_3COMUSR_HOMECONN	0x009d		/* 3Com HomeConnect camera */
#define	USB_PRODUCT_3COMUSR_USR56K	0x3021		/* U.S.Robotics 56000 Voice Faxmodem Pro */


/* Acer Peripherals, Inc. products */
#define	USB_PRODUCT_ACERP_ACERSCAN_C310U	0x12a6		/* Acerscan C310U */

/* ActiveWire, Inc. products */
#define	USB_PRODUCT_ACTIVEWIRE_IOBOARD	0x0100		/* I/O Board */
#define	USB_PRODUCT_ACTIVEWIRE_IOBOARD_FW1	0x0101		/* I/O Board, rev. 1 firmware */

/* ADMtek products */
#define	USB_PRODUCT_ADMTEK_PEGASUS	0x0986		/* AN986 USB Ethernet adapter */

/* ADS products */
#define	USB_PRODUCT_ADS_UBS10BT	0x0008		/* UBS-10BT Ethernet adapter */

/* Agiler products */
#define	USB_PRODUCT_ELECOM_MOUSE29UO	0x0002		/* mouse 29UO */

/* AKS products */
#define	USB_PRODUCT_AKS_USBHASP	0x0001		/* USB-HASP 0.06 */

/* Altec Lansing products */
#define	USB_PRODUCT_ALTEC_ADA70	0x0070		/* ADA70 Speakers */
#define	USB_PRODUCT_ALTEC_ASC495	0xff05		/* ASC495 Speakers */

/* American Power Conversion products */
#define	USB_PRODUCT_APC_UPSPRO500	0x0002		/* Back-UPS Pro 500 */

/* Anchor products */
#define	USB_PRODUCT_ANCHOR_EZUSB	0x2131		/* EZUSB */

/* AOX, Inc. products */
#define	USB_PRODUCT_AOX_USB101	0x0008		/* USB ethernet controller engine */

/* ATen products */
#define	USB_PRODUCT_ATEN_UC1284	0x2001		/* Parallel printer adapter */
#define	USB_PRODUCT_ATEN_UC10T	0x2002		/* 10Mbps ethernet adapter */

/* Belkin products */
/*product BELKIN F5U111		0x????	F5U111 Ethernet adapter*/

/* Billionton products */
#define	USB_PRODUCT_BILLIONTON_USB100	0x0986		/* USB100N 10/100 FastEthernet Adapter */

/* Brother Industries products */
#define	USB_PRODUCT_BROTHER_HL1050	0x0002		/* HL-1050 laser printer */

/* Behavior Technology Computer products */
#define	USB_PRODUCT_BTC_BTC7932	0x6782		/* Keyboard with mouse port */

/* Canon, Inc. products */
#define	USB_PRODUCT_CANON_S10	0x3041		/* PowerShot S10 */

/* CATC products */
#define	USB_PRODUCT_CATC_NETMATE	0x000a		/* Netmate ethernet adapter */
#define	USB_PRODUCT_CATC_NETMATE2	0x000c		/* Netmate2 ethernet adapter */
#define	USB_PRODUCT_CATC_CHIEF	0x000d		/* USB Chief Bus & Protocol Analyzer */
#define	USB_PRODUCT_CATC_ANDROMEDA	0x1237		/* Andromeda hub */

/* Cherry products */
#define	USB_PRODUCT_CHERRY_MY3000KBD	0x0001		/* My3000 keyboard */
#define	USB_PRODUCT_CHERRY_MY3000HUB	0x0003		/* My3000 hub */

/* Chic Technology products */
#define	USB_PRODUCT_CHIC_MOUSE1	0x0001		/* mouse */
#define	USB_PRODUCT_CHIC_CYPRESS	0x0003		/* Cypress USB Mouse */

/* Chicony products */
#define	USB_PRODUCT_CHICONY_KB8933	0x0001		/* KB-8933 keyboard */

/* Connectix products */
#define	USB_PRODUCT_CONNECTIX_QUICKCAM	0x0001		/* QuickCam */

/* Corega products */
#define	USB_PRODUCT_COREGA_ETHER_USB_T	0x0001		/* Ether USB-T */
#define	USB_PRODUCT_COREGA_FETHER_USB_TX	0x0004		/* FEther USB-TX */

/* Cypress Semiconductor products */
#define	USB_PRODUCT_CYPRESS_MOUSE	0x0001		/* mouse */
#define	USB_PRODUCT_CYPRESS_THERMO	0x0002		/* thermometer */

/* D-Link products */
/*product DLINK DSBS25		0x0100	DSB-S25 serial adapter*/
#define	USB_PRODUCT_DLINK_DSB650C	0x4000		/* 10Mbps ethernet adapter */
#define	USB_PRODUCT_DLINK_DSB650TX	0x4002		/* 10/100 ethernet adapter */
#define	USB_PRODUCT_DLINK_DSB650TX_PNA	0x4003		/* 1/10/100 ethernet adapter */

/* Dallas Semiconductor products */
#define	USB_PRODUCT_DALLAS_J6502	0x4201		/* J-6502 speakers */

/* Diamond products */
#define	USB_PRODUCT_DIAMOND_RIO500USB	0x0001		/* Rio 500 USB */

/* Digi International products */
#define	USB_PRODUCT_DIGI_ACCELEPORT2	0x0002		/* AccelePort USB 2 */
#define	USB_PRODUCT_DIGI_ACCELEPORT4	0x0004		/* AccelePort USB 4 */
#define	USB_PRODUCT_DIGI_ACCELEPORT8	0x0008		/* AccelePort USB 8 */

/* EIZO products */
#define	USB_PRODUCT_EIZO_HUB	0x0000		/* hub */
#define	USB_PRODUCT_EIZO_MONITOR	0x0001		/* monitor */

/* Elsa products */
#define	USB_PRODUCT_ELSA_MODEM1	0x2265		/* ELSA Modem Board */

/* Entrega products */
#define	USB_PRODUCT_ENTREGA_1S	0x0001		/* 1S serial connector */
#define	USB_PRODUCT_ENTREGA_2S	0x0002		/* 2S serial connector */
#define	USB_PRODUCT_ENTREGA_1S25	0x0003		/* 1S25 serial connector */
#define	USB_PRODUCT_ENTREGA_4S	0x0004		/* 4S serial connector */
#define	USB_PRODUCT_ENTREGA_E45	0x0005		/* E45 Ethernet adapter */
#define	USB_PRODUCT_ENTREGA_CENTRONICS	0x0006		/* Centronics connector */
#define	USB_PRODUCT_ENTREGA_1S9	0x0093		/* 1S9 serial connector */
#define	USB_PRODUCT_ENTREGA_EZUSB	0x8000		/* EZ-USB */
/*product ENTREGA SERIAL	0x8001	DB25 Serial connector*/
/*product ENTREGA SERIAL_DB9	0x8093	DB9 Serial connector*/

/* e-TEK Labs products */
#define	USB_PRODUCT_ETEK_1COM	0x8007		/* Serial port */

/* Epson products */
#define	USB_PRODUCT_EPSON_PRINTER2	0x0002		/* ISD USB Smart Cable for Mac */
#define	USB_PRODUCT_EPSON_PRINTER3	0x0003		/* ISD USB Smart Cable */

/* Gravis products */
#define	USB_PRODUCT_GRAVIS_GAMEPADPRO	0x4001		/* GamePad Pro */

/* Freecom products */
#define	USB_PRODUCT_FREECOM_DVD	0xfc01		/* Connector for DVD drive */

/* Future Technology Devices products */
#define	USB_PRODUCT_FUTURE_SERIAL	0x8372		/* Serial converter */

/* Handspring, Inc. */
#define	USB_PRODUCT_HANDSPRING_VISOR	0x0100		/* Handspring Visor */

/* HP products */
#define	USB_PRODUCT_HP_4100C	0x0101		/* Scanjet 4100C */
#define	USB_PRODUCT_HP_S20	0x0102		/* Photosmart S20 */
#define	USB_PRODUCT_HP_4200C	0x0105		/* ScanJet 4200C */
#define	USB_PRODUCT_HP_6200C	0x0201		/* ScanJet 6200C */
#define	USB_PRODUCT_HP_S20b	0x0202		/* PhotoSmart S20 */
#define	USB_PRODUCT_HP_3300C	0x0205		/* ScanJet 3300C */
#define	USB_PRODUCT_HP_5200	0x0401		/* Scanjet 5200 */
#define	USB_PRODUCT_HP_6300C	0x0601		/* Scanjet 6300C */
#define	USB_PRODUCT_HP_970CSE	0x1004		/* Deskjet 970Cse */
#define	USB_PRODUCT_HP_P1100	0x3102		/* Photosmart P1100 */

/* Inside Out Networks products */
#define	USB_PRODUCT_INSIDEOUT_EDGEPORT4	0x0001		/* EdgePort/4 serial ports */

/* In-System products */
#define	USB_PRODUCT_INSYSTEM_F5U002	0x0002		/* Parallel printer adapter */
#define	USB_PRODUCT_INSYSTEM_ISD110	0x0200		/* IDE adapter */

/* Intel products */
#define	USB_PRODUCT_INTEL_TESTBOARD	0x9890		/* 82930 test board */

/* I/O DATA products */
#define	USB_PRODUCT_IODATA_USBETTX	0x0904		/* USB ETTX */

/* Iomega products */
#define	USB_PRODUCT_IOMEGA_ZIP100	0x0001		/* Zip 100 */

/* Kawasaki products */
#define	USB_PRODUCT_KLSI_DUH3E10BT	0x0008		/* USB ethernet controller engine */

/* Kawatsu products */
#define	USB_PRODUCT_KAWATSU_MH4000P	0x0003		/* MiniHub 4000P */

/* Keisokugiken Corp. products */
#define	USB_PRODUCT_KEISOKUGIKEN_USBDAQ	0x0068		/* HKS-0200 USBDAQ */

/* Kawasaki LSI products (?) */
#define	USB_PRODUCT_KLSI_DUH3E10BT	0x0008		/* 10BT Ethernet adapter, in the DU-H3E */

/* Kensington products */
#define	USB_PRODUCT_KENSINGTON_ORBIT	0x1003		/* Orbit USB/PS2 trackball */
#define	USB_PRODUCT_KENSINGTON_TURBOBALL	0x1005		/* TurboBall */

/* Kodak products */
#define	USB_PRODUCT_KODAK_DC220	0x0100		/* Digital Science DC220 */
#define	USB_PRODUCT_KODAK_DC260	0x0110		/* Digital Science DC260 */
#define	USB_PRODUCT_KODAK_DC265	0x0111		/* Digital Science DC265 */
#define	USB_PRODUCT_KODAK_DC290	0x0112		/* Digital Science DC290 */
#define	USB_PRODUCT_KODAK_DC240	0x0120		/* Digital Science DC240 */
#define	USB_PRODUCT_KODAK_DC280	0x0130		/* Digital Science DC280 */

/* Konica Corp. Products */
#define	USB_PRODUCT_KONICA_CAMERA	0x0720		/* Digital Color Camera */

/* KYE products */
#define	USB_PRODUCT_KYE_NICHE	0x0001		/* Niche mouse */
#define	USB_PRODUCT_KYE_NETSCROLL	0x0003		/* Genius NetScroll mouse */
#define	USB_PRODUCT_KYE_FLIGHT2000	0x1004		/* Flight 2000 joystick */

/* Lexmark products */
#define	USB_PRODUCT_LEXMARK_S2450	0x0009		/* Optra S 2450 */

/* Linksys products */
#define	USB_PRODUCT_LINKSYS_USB10T	0x2202		/* USB10T Ethernet */
#define	USB_PRODUCT_LINKSYS_USB100TX	0x2203		/* USB100TX Ethernet */
#define	USB_PRODUCT_LINKSYS_USB10TA	0x2206		/* USB10TA Ethernet */

/* Logitech products */
#define	USB_PRODUCT_LOGITECH_M2452	0x0203		/* M2452 keyboard */
#define	USB_PRODUCT_LOGITECH_M4848	0x0301		/* M4848 mouse */
#define	USB_PRODUCT_LOGITECH_QUICKCAM	0x0801		/* QuickCam */
#define	USB_PRODUCT_LOGITECH_QUICKCAMPRO	0x0810		/* QuickCam Pro */
#define	USB_PRODUCT_LOGITECH_N43	0xc000		/* N43 */
#define	USB_PRODUCT_LOGITECH_N48	0xc001		/* N48 mouse */
#define	USB_PRODUCT_LOGITECH_MBA47	0xc002		/* M-BA47 mouse */
#define	USB_PRODUCT_LOGITECH_WMMOUSE	0xc004		/* WingMan Gaming Mouse */
#define	USB_PRODUCT_LOGITECH_WMPAD	0xc208		/* WingMan GamePad Extreme */
#define	USB_PRODUCT_LOGITECH_WMJOY	0xc281		/* WingMan Force joystick */

/* Lucent products */
#define	USB_PRODUCT_LUCENT_EVALKIT	0x1001		/* USS-720 evaluation kit */

/* Macally products */
#define	USB_PRODUCT_MACALLY_MOUSE1	0x0101		/* mouse */

/* Melco, Inc products */
#define	USB_PRODUCT_MELCO_LUATX	0x0001		/* LU-ATX Ethernet */

/* Microsoft products */
#define	USB_PRODUCT_MICROSOFT_SIDEPREC	0x0008		/* SideWinder Precision Pro */
#define	USB_PRODUCT_MICROSOFT_INTELLIMOUSE	0x0009		/* IntelliMouse */
#define	USB_PRODUCT_MICROSOFT_NATURALKBD	0x000b		/* Natural Keyboard Elite */
#define	USB_PRODUCT_MICROSOFT_DDS80	0x0014		/* Digital Sound System 80 */
#define	USB_PRODUCT_MICROSOFT_SIDEWINDER	0x001a		/* Sidewinder Precision Racing Wheel */
#define	USB_PRODUCT_MICROSOFT_INTELLIEYE	0x0025		/* IntelliEye mouse */
#define	USB_PRODUCT_MICROSOFT_INETPRO	0x002b		/* Internet Keyboard Pro */

/* Midiman products */
#define	USB_PRODUCT_MIDIMAN_MIDISPORT2X2	0x1001		/* Midisport 2x2 */

/* Motorola products */
#define	USB_PRODUCT_MOTOROLA_MC141555	0x1555		/* MC141555 hub controller */

/* MultiTech products */
#define	USB_PRODUCT_MULTITECH_ATLAS	0xf101		/* MT5634ZBA-USB modem */

/* Mustek products */
#define	USB_PRODUCT_MUSTEK_MDC800	0xa800		/* MDC-800 digital camera */

/* NEC products */
#define	USB_PRODUCT_NEC_HUB	0x55aa		/* hub */
#define	USB_PRODUCT_NEC_HUB_B	0x55ab		/* hub */

/* NetChip Technology Products */
#define	USB_PRODUCT_NETCHIP_TURBOCONNECT	0x1080		/* Turbo-Connect */

/* Netgear products */
#define	USB_PRODUCT_NETGEAR_EA101	0x1001		/* Ethernet adapter */

/* OmniVision Technologies, Inc. products */
#define	USB_PRODUCT_OMNIVISION_OV511	0x0511		/* OV511 Camera */

/* Palm Computing, Inc. */
#define	USB_PRODUCT_PALM_SERIAL	0x0080		/* USB Serial Adaptor */

/* Peracom products */
#define	USB_PRODUCT_PERACOM_SERIAL1	0x0001		/* Serial Converter */
#define	USB_PRODUCT_PERACOM_ENET	0x0002		/* Ethernet adapter */
#define	USB_PRODUCT_PERACOM_ENET3	0x0003		/* At Home Ethernet Adapter */
#define	USB_PRODUCT_PERACOM_ENET2	0x0005		/* Ethernet adapter */

/* Philips products */
#define	USB_PRODUCT_PHILIPS_DSS350	0x0101		/* DSS 350 Digital Speaker System */
#define	USB_PRODUCT_PHILIPS_DSS	0x0104		/* DSS XXX Digital Speaker System */
#define	USB_PRODUCT_PHILIPS_HUB	0x0201		/* hub */
#define	USB_PRODUCT_PHILIPS_DSS150	0x0471		/* DSS 150 Digital Speaker System */

/* P.I. Engineering products */
#define	USB_PRODUCT_PIENGINEERING_PS2USB	0x020b		/* PS2 to Mac USB Adapter */

/* PLX products */
#define	USB_PRODUCT_PLX_TESTBOARD	0x9060		/* test board */

/* Primax products */
#define	USB_PRODUCT_PRIMAX_COMFORT	0x4d01		/* Comfort */
#define	USB_PRODUCT_PRIMAX_MOUSEINABOX	0x4d02		/* Mouse-in-a-Box */
#define	USB_PRODUCT_PRIMAX_PCGAUMS1	0x4d04		/* Sony PCGA-UMS1 */

/* Quickshot products */
#define	USB_PRODUCT_QUICKSHOT_STRIKEPAD	0x6238		/* USB StrikePad */

/* Rockfire products */
#define	USB_PRODUCT_ROCKFIRE_GAMEPAD	0x2033		/* gamepad 203USB */

/* Qtronix products */
#define	USB_PRODUCT_QTRONIX_980N	0x2011		/* Scorpion-980N keyboard */

/* SanDisk products */
#define	USB_PRODUCT_SANDISK_IMAGEMATE	0x0001		/* USB ImageMate */

/* Shuttle Technology products */
#define	USB_PRODUCT_SHUTTLE_EUSB	0x0001		/* E-USB Bridge */

/* SIIG products */
#define	USB_PRODUCT_SIIG_DIGIFILMREADER	0x0004		/* DigiFilm-Combo Reader */

/* Sirius Technologies products */
#define	USB_PRODUCT_SIRIUS_ROADSTER	0x0001		/* NetComm Roadster II 56 USB */

/* SMC products */
#define	USB_PRODUCT_SMC_2102USB	0x0100		/* 10Mbps ethernet adapter */
#define	USB_PRODUCT_SMC_2202USB	0x0200		/* 10/100 ethernet adapter */

/* SOLID YEAR products */
#define	USB_PRODUCT_SOLIDYEAR_KEYBOARD	0x2101		/* Solid Year USB keyboard */

/* STMicroelectronics products */
#define	USB_PRODUCT_STMICRO_COMMUNICATOR	0x7554		/* USB Communicator */

/* Sun Microsystems products */
#define	USB_PRODUCT_SUN_KEYBOARD	0x0005		/* Type 6 USB */
/* XXX The above is a North American PC style keyboard possibly */

/* Telex Communications products */
#define	USB_PRODUCT_TELEX_MIC1	0x0001		/* Enhanced USB Microphone */

/* Texas Intel products */
#define	USB_PRODUCT_TI_UTUSB41	0x1446		/* UT-USB41 hub */

/* Thrustmaster products */
#define	USB_PRODUCT_THRUST_FUSION_PAD	0xa0a3		/* Fusion Digital Gamepad */

/* Universal Access products */
#define	USB_PRODUCT_UNIACCESS_PANACHE	0x0101		/* Panache Surf USB ISDN Adapter */

/* Vision products */
#define	USB_PRODUCT_VISION_VC6452V002	0x0002		/* VC6452V002 Camera */

/* Wacom products */
#define	USB_PRODUCT_WACOM_CT0405U	0x0000		/* CT-0405-U Tablet */
#define	USB_PRODUCT_WACOM_GRAPHIRE	0x0010		/* Graphire */
#define	USB_PRODUCT_WACOM_INTUOSA5	0x0021		/* Intuos A5 */
 
/* Y-E Data products */
#define	USB_PRODUCT_YEDATA_FLASHBUSTERU	0x0000		/* Flashbuster-U */

/* Zoom Telephonics, Inc. products */
#define	USB_PRODUCT_ZOOM_2986L	0x9700		/* 2986L Fax modem */
