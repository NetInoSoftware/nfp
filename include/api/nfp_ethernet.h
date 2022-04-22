/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

/*
 * Fundamental constants relating to ethernet.
 *
 * $FreeBSD: release/9.1.0/sys/net/ethernet.h 191148 2009-04-16 20:30:28Z kmacy $
 *
 */

#ifndef __NFP_ETHERNET_H__
#define __NFP_ETHERNET_H__

#include <stdint.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Some basic Ethernet constants.
 */
#define	NFP_ETHER_ADDR_LEN		6	/* length of an Ethernet address */
#define	NFP_ETHER_TYPE_LEN		2	/* length of the Ethernet type field */
#define	NFP_ETHER_CRC_LEN		4	/* length of the Ethernet CRC */
#define	NFP_ETHER_HDR_LEN		(NFP_ETHER_ADDR_LEN*2+NFP_ETHER_TYPE_LEN)
#define	NFP_ETHER_MIN_LEN		64	/* minimum frame len, including CRC */
#define	NFP_ETHER_MAX_LEN		1518	/* maximum frame len, including CRC */
#define	NFP_ETHER_MAX_LEN_JUMBO	9018	/* max jumbo frame len, including CRC */

#define	NFP_ETHER_VLAN_ENCAP_LEN	4	/* len of 802.1Q VLAN encapsulation */

/*
 * A macro to validate a length with
 */
#define	NFP_ETHER_IS_VALID_LEN(foo)	\
	((foo) >= NFP_ETHER_MIN_LEN && (foo) <= NFP_ETHER_MAX_LEN)

/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct nfp_ether_header {
	uint8_t ether_dhost[NFP_ETHER_ADDR_LEN];
	uint8_t ether_shost[NFP_ETHER_ADDR_LEN];
	uint16_t ether_type;
} __attribute__((packed));

/*
 * Structure of a 48-bit Ethernet address.
 */
struct nfp_ether_addr {
	uint8_t octet[NFP_ETHER_ADDR_LEN];
} __attribute__((packed));

#define	NFP_ETHER_IS_MULTICAST(addr) (*(addr) & 0x01) /* is address mcast/bcast? */

#define NFP_ETHERTYPE_IS_VLAN(et)			\
	(((et) == NFP_ETHERTYPE_VLAN)	  ||	\
	 ((et) == NFP_ETHERTYPE_QINQ_STD)	  ||	\
	 ((et) == NFP_ETHERTYPE_QINQ_VENDOR1) ||	\
	 ((et) == NFP_ETHERTYPE_QINQ_VENDOR2) ||	\
	 ((et) == NFP_ETHERTYPE_QINQ_VENDOR3))

/*
 *  NOTE: 0x0000-0x05DC (0..1500) are generally IEEE 802.3 length fields.
 *  However, there are some conflicts.
 */

#define	NFP_ETHERTYPE_8023		0x0004	/* IEEE 802.3 packet */
		   /* 0x0101 .. 0x1FF	   Experimental */
#define	NFP_ETHERTYPE_PUP		0x0200	/* Xerox PUP protocol - see 0A00 */
#define	NFP_ETHERTYPE_PUPAT		0x0200	/* PUP Address Translation - see 0A01 */
#define	NFP_ETHERTYPE_SPRITE	0x0500	/* ??? */
			     /* 0x0400	   Nixdorf */
#define	NFP_ETHERTYPE_NS		0x0600	/* XNS */
#define	NFP_ETHERTYPE_NSAT		0x0601	/* XNS Address Translation (3Mb only) */
#define	NFP_ETHERTYPE_DLOG1	0x0660	/* DLOG (?) */
#define	NFP_ETHERTYPE_DLOG2	0x0661	/* DLOG (?) */
#define	NFP_ETHERTYPE_IP		0x0800	/* IP protocol */
#define	NFP_ETHERTYPE_X75		0x0801	/* X.75 Internet */
#define	NFP_ETHERTYPE_NBS		0x0802	/* NBS Internet */
#define	NFP_ETHERTYPE_ECMA		0x0803	/* ECMA Internet */
#define	NFP_ETHERTYPE_CHAOS	0x0804	/* CHAOSnet */
#define	NFP_ETHERTYPE_X25		0x0805	/* X.25 Level 3 */
#define	NFP_ETHERTYPE_ARP		0x0806	/* Address resolution protocol */
#define	NFP_ETHERTYPE_NSCOMPAT	0x0807	/* XNS Compatibility */
#define	NFP_ETHERTYPE_FRARP	0x0808	/* Frame Relay ARP (RFC1701) */
			     /* 0x081C	   Symbolics Private */
		    /* 0x0888 - 0x088A	   Xyplex */
#define	NFP_ETHERTYPE_UBDEBUG	0x0900	/* Ungermann-Bass network debugger */
#define	NFP_ETHERTYPE_IEEEPUP	0x0A00	/* Xerox IEEE802.3 PUP */
#define	NFP_ETHERTYPE_IEEEPUPAT	0x0A01	/* Xerox IEEE802.3 PUP Address Translation */
#define	NFP_ETHERTYPE_VINES	0x0BAD	/* Banyan VINES */
#define	NFP_ETHERTYPE_VINESLOOP	0x0BAE	/* Banyan VINES Loopback */
#define	NFP_ETHERTYPE_VINESECHO	0x0BAF	/* Banyan VINES Echo */

/*		       0x1000 - 0x100F	   Berkeley Trailer */
/*
 * The NFP_NFP_ETHERTYPE_NTRAILER packet types starting at NFP_NFP_ETHERTYPE_TRAIL have
 * (type-NFP_ETHERTYPE_TRAIL)*512 bytes of data followed
 * by an NFP_ETHER type (as given above) and then the (variable-length) header.
 */
#define	NFP_ETHERTYPE_TRAIL		0x1000	/* Trailer packet */
#define	NFP_ETHERTYPE_NTRAILER	16

#define	NFP_ETHERTYPE_DCA		0x1234	/* DCA - Multicast */
#define	NFP_ETHERTYPE_VALID	0x1600	/* VALID system protocol */
#define	NFP_ETHERTYPE_DOGFIGHT	0x1989	/* Artificial Horizons ("Aviator" dogfight simulator [on Sun]) */
#define	NFP_ETHERTYPE_RCL		0x1995	/* Datapoint Corporation (RCL lan protocol) */

					/* The following 3C0x types
					   are unregistered: */
#define	NFP_ETHERTYPE_NBPVCD	0x3C00	/* 3Com NBP virtual circuit datagram (like XNS SPP) not registered */
#define	NFP_ETHERTYPE_NBPSCD	0x3C01	/* 3Com NBP System control datagram not registered */
#define	NFP_ETHERTYPE_NBPCREQ	0x3C02	/* 3Com NBP Connect request (virtual cct) not registered */
#define	NFP_ETHERTYPE_NBPCRSP	0x3C03	/* 3Com NBP Connect response not registered */
#define	NFP_ETHERTYPE_NBPCC		0x3C04	/* 3Com NBP Connect complete not registered */
#define	NFP_ETHERTYPE_NBPCLREQ	0x3C05	/* 3Com NBP Close request (virtual cct) not registered */
#define	NFP_ETHERTYPE_NBPCLRSP	0x3C06	/* 3Com NBP Close response not registered */
#define	NFP_ETHERTYPE_NBPDG		0x3C07	/* 3Com NBP Datagram (like XNS IDP) not registered */
#define	NFP_ETHERTYPE_NBPDGB	0x3C08	/* 3Com NBP Datagram broadcast not registered */
#define	NFP_ETHERTYPE_NBPCLAIM	0x3C09	/* 3Com NBP Claim NetBIOS name not registered */
#define	NFP_ETHERTYPE_NBPDLTE	0x3C0A	/* 3Com NBP Delete NetBIOS name not registered */
#define	NFP_ETHERTYPE_NBPRAS	0x3C0B	/* 3Com NBP Remote adaptor status request not registered */
#define	NFP_ETHERTYPE_NBPRAR	0x3C0C	/* 3Com NBP Remote adaptor response not registered */
#define	NFP_ETHERTYPE_NBPRST	0x3C0D	/* 3Com NBP Reset not registered */

#define	NFP_ETHERTYPE_PCS		0x4242	/* PCS Basic Block Protocol */
#define	NFP_ETHERTYPE_IMLBLDIAG	0x424C	/* Information Modes Little Big LAN diagnostic */
#define	NFP_ETHERTYPE_DIDDLE	0x4321	/* THD - Diddle */
#define	NFP_ETHERTYPE_IMLBL		0x4C42	/* Information Modes Little Big LAN */
#define	NFP_ETHERTYPE_SIMNET	0x5208	/* BBN Simnet Private */
#define	NFP_ETHERTYPE_DECEXPER	0x6000	/* DEC Unassigned, experimental */
#define	NFP_ETHERTYPE_MOPDL		0x6001	/* DEC MOP dump/load */
#define	NFP_ETHERTYPE_MOPRC		0x6002	/* DEC MOP remote console */
#define	NFP_ETHERTYPE_DECnet	0x6003	/* DEC DECNET Phase IV route */
#define	NFP_ETHERTYPE_DN		NFP_ETHERTYPE_DECnet	/* libpcap, tcpdump */
#define	NFP_ETHERTYPE_LAT		0x6004	/* DEC LAT */
#define	NFP_ETHERTYPE_DECDIAG	0x6005	/* DEC diagnostic protocol (at interface initialization?) */
#define	NFP_ETHERTYPE_DECCUST	0x6006	/* DEC customer protocol */
#define	NFP_ETHERTYPE_SCA		0x6007	/* DEC LAVC, SCA */
#define	NFP_ETHERTYPE_AMBER		0x6008	/* DEC AMBER */
#define	NFP_ETHERTYPE_DECMUMPS	0x6009	/* DEC MUMPS */
		    /* 0x6010 - 0x6014	   3Com Corporation */
#define	NFP_ETHERTYPE_TRANSETHER	0x6558	/* Trans Ether Bridging (RFC1701)*/
#define	NFP_ETHERTYPE_RAWFR		0x6559	/* Raw Frame Relay (RFC1701) */
#define	NFP_ETHERTYPE_UBDL		0x7000	/* Ungermann-Bass download */
#define	NFP_ETHERTYPE_UBNIU		0x7001	/* Ungermann-Bass NIUs */
#define	NFP_ETHERTYPE_UBDIAGLOOP	0x7002	/* Ungermann-Bass diagnostic/loopback */
#define	NFP_ETHERTYPE_UBNMC		0x7003	/* Ungermann-Bass ??? (NMC to/from UB Bridge) */
#define	NFP_ETHERTYPE_UBBST		0x7005	/* Ungermann-Bass Bridge Spanning Tree */
#define	NFP_ETHERTYPE_OS9		0x7007	/* OS/9 Microware */
#define	NFP_ETHERTYPE_OS9NET	0x7009	/* OS/9 Net? */
		    /* 0x7020 - 0x7029	   LRT (England) (now Sintrom) */
#define	NFP_ETHERTYPE_RACAL		0x7030	/* Racal-Interlan */
#define	NFP_ETHERTYPE_PRIMENTS	0x7031	/* Prime NTS (Network Terminal Service) */
#define	NFP_ETHERTYPE_CABLETRON	0x7034	/* Cabletron */
#define	NFP_ETHERTYPE_CRONUSVLN	0x8003	/* Cronus VLN */
#define	NFP_ETHERTYPE_CRONUS	0x8004	/* Cronus Direct */
#define	NFP_ETHERTYPE_HP		0x8005	/* HP Probe */
#define	NFP_ETHERTYPE_NESTAR	0x8006	/* Nestar */
#define	NFP_ETHERTYPE_ATTSTANFORD	0x8008	/* AT&T/Stanford (local use) */
#define	NFP_ETHERTYPE_EXCELAN	0x8010	/* Excelan */
#define	NFP_ETHERTYPE_SG_DIAG	0x8013	/* SGI diagnostic type */
#define	NFP_ETHERTYPE_SG_NETGAMES	0x8014	/* SGI network games */
#define	NFP_ETHERTYPE_SG_RESV	0x8015	/* SGI reserved type */
#define	NFP_ETHERTYPE_SG_BOUNCE	0x8016	/* SGI bounce server */
#define	NFP_ETHERTYPE_APOLLODOMAIN	0x8019	/* Apollo DOMAIN */
#define	NFP_ETHERTYPE_TYMSHARE	0x802E	/* Tymeshare */
#define	NFP_ETHERTYPE_TIGAN		0x802F	/* Tigan, Inc. */
#define	NFP_ETHERTYPE_REVARP	0x8035	/* Reverse addr resolution protocol */
#define	NFP_ETHERTYPE_AEONIC	0x8036	/* Aeonic Systems */
#define	NFP_ETHERTYPE_IPXNEW	0x8037	/* IPX (Novell Netware?) */
#define	NFP_ETHERTYPE_LANBRIDGE	0x8038	/* DEC LANBridge */
#define	NFP_ETHERTYPE_DSMD	0x8039	/* DEC DSM/DDP */
#define	NFP_ETHERTYPE_ARGONAUT	0x803A	/* DEC Argonaut Console */
#define	NFP_ETHERTYPE_VAXELN	0x803B	/* DEC VAXELN */
#define	NFP_ETHERTYPE_DECDNS	0x803C	/* DEC DNS Naming Service */
#define	NFP_ETHERTYPE_ENCRYPT	0x803D	/* DEC Ethernet Encryption */
#define	NFP_ETHERTYPE_DECDTS	0x803E	/* DEC Distributed Time Service */
#define	NFP_ETHERTYPE_DECLTM	0x803F	/* DEC LAN Traffic Monitor */
#define	NFP_ETHERTYPE_DECNETBIOS	0x8040	/* DEC PATHWORKS DECnet NETBIOS Emulation */
#define	NFP_ETHERTYPE_DECLAST	0x8041	/* DEC Local Area System Transport */
			     /* 0x8042	   DEC Unassigned */
#define	NFP_ETHERTYPE_PLANNING	0x8044	/* Planning Research Corp. */
		    /* 0x8046 - 0x8047	   AT&T */
#define	NFP_ETHERTYPE_DECAM		0x8048	/* DEC Availability Manager for Distributed Systems DECamds (but someone at DEC says not) */
#define	NFP_ETHERTYPE_EXPERDATA	0x8049	/* ExperData */
#define	NFP_ETHERTYPE_VEXP		0x805B	/* Stanford V Kernel exp. */
#define	NFP_ETHERTYPE_VPROD		0x805C	/* Stanford V Kernel prod. */
#define	NFP_ETHERTYPE_ES		0x805D	/* Evans & Sutherland */
#define	NFP_ETHERTYPE_LITTLE	0x8060	/* Little Machines */
#define	NFP_ETHERTYPE_COUNTERPOINT	0x8062	/* Counterpoint Computers */
		    /* 0x8065 - 0x8066	   Univ. of Mass @ Amherst */
#define	NFP_ETHERTYPE_VEECO		0x8067	/* Veeco Integrated Auto. */
#define	NFP_ETHERTYPE_GENDYN	0x8068	/* General Dynamics */
#define	NFP_ETHERTYPE_ATT		0x8069	/* AT&T */
#define	NFP_ETHERTYPE_AUTOPHON	0x806A	/* Autophon */
#define	NFP_ETHERTYPE_COMDESIGN	0x806C	/* ComDesign */
#define	NFP_ETHERTYPE_COMPUGRAPHIC	0x806D	/* Compugraphic Corporation */
		    /* 0x806E - 0x8077	   Landmark Graphics Corp. */
#define	NFP_ETHERTYPE_MATRA		0x807A	/* Matra */
#define	NFP_ETHERTYPE_DDE		0x807B	/* Dansk Data Elektronik */
#define	NFP_ETHERTYPE_MERIT		0x807C	/* Merit Internodal (or Univ of Michigan?) */
		    /* 0x807D - 0x807F	   Vitalink Communications */
#define	NFP_ETHERTYPE_VLTLMAN	0x8080	/* Vitalink TransLAN III Management */
		    /* 0x8081 - 0x8083	   Counterpoint Computers */
		    /* 0x8088 - 0x808A	   Xyplex */
#define	NFP_ETHERTYPE_ATALK		0x809B	/* AppleTalk */
#define	NFP_ETHERTYPE_AT		NFP_ETHERTYPE_ATALK		/* old NetBSD */
#define	NFP_ETHERTYPE_APPLETALK	NFP_ETHERTYPE_ATALK		/* HP-UX */
		    /* 0x809C - 0x809E	   Datability */
#define	NFP_ETHERTYPE_SPIDER	0x809F	/* Spider Systems Ltd. */
			     /* 0x80A3	   Nixdorf */
		    /* 0x80A4 - 0x80B3	   Siemens Gammasonics Inc. */
		    /* 0x80C0 - 0x80C3	   DCA (Digital Comm. Assoc.) Data Exchange Cluster */
		    /* 0x80C4 - 0x80C5	   Banyan Systems */
#define	NFP_ETHERTYPE_PACER		0x80C6	/* Pacer Software */
#define	NFP_ETHERTYPE_APPLITEK	0x80C7	/* Applitek Corporation */
		    /* 0x80C8 - 0x80CC	   Intergraph Corporation */
		    /* 0x80CD - 0x80CE	   Harris Corporation */
		    /* 0x80CF - 0x80D2	   Taylor Instrument */
		    /* 0x80D3 - 0x80D4	   Rosemount Corporation */
#define	NFP_ETHERTYPE_SNA		0x80D5	/* IBM SNA Services over Ethernet */
#define	NFP_ETHERTYPE_VARIAN	0x80DD	/* Varian Associates */
		    /* 0x80DE - 0x80DF	   TRFS (Integrated Solutions Transparent Remote File System) */
		    /* 0x80E0 - 0x80E3	   Allen-Bradley */
		    /* 0x80E4 - 0x80F0	   Datability */
#define	NFP_ETHERTYPE_RETIX		0x80F2	/* Retix */
#define	NFP_ETHERTYPE_AARP		0x80F3	/* AppleTalk AARP */
		    /* 0x80F4 - 0x80F5	   Kinetics */
#define	NFP_ETHERTYPE_APOLLO	0x80F7	/* Apollo Computer */
#define NFP_ETHERTYPE_VLAN		0x8100	/* IEEE 802.1Q VLAN tagging (XXX conflicts) */
		    /* 0x80FF - 0x8101	   Wellfleet Communications (XXX conflicts) */
#define	NFP_ETHERTYPE_BOFL		0x8102	/* Wellfleet; BOFL (Breath OF Life) pkts [every 5-10 secs.] */
#define	NFP_ETHERTYPE_WELLFLEET	0x8103	/* Wellfleet Communications */
		    /* 0x8107 - 0x8109	   Symbolics Private */
#define	NFP_ETHERTYPE_TALARIS	0x812B	/* Talaris */
#define	NFP_ETHERTYPE_WATERLOO	0x8130	/* Waterloo Microsystems Inc. (XXX which?) */
#define	NFP_ETHERTYPE_HAYES		0x8130	/* Hayes Microcomputers (XXX which?) */
#define	NFP_ETHERTYPE_VGLAB		0x8131	/* VG Laboratory Systems */
		    /* 0x8132 - 0x8137	   Bridge Communications */
#define	NFP_ETHERTYPE_IPX		0x8137	/* Novell (old) NetWare IPX (ECONFIG E option) */
#define	NFP_ETHERTYPE_NOVELL	0x8138	/* Novell, Inc. */
		    /* 0x8139 - 0x813D	   KTI */
#define	NFP_ETHERTYPE_MUMPS		0x813F	/* M/MUMPS data sharing */
#define	NFP_ETHERTYPE_AMOEBA	0x8145	/* Vrije Universiteit (NL) Amoeba 4 RPC (obsolete) */
#define	NFP_ETHERTYPE_FLIP		0x8146	/* Vrije Universiteit (NL) FLIP (Fast Local Internet Protocol) */
#define	NFP_ETHERTYPE_VURESERVED	0x8147	/* Vrije Universiteit (NL) [reserved] */
#define	NFP_ETHERTYPE_LOGICRAFT	0x8148	/* Logicraft */
#define	NFP_ETHERTYPE_NCD		0x8149	/* Network Computing Devices */
#define	NFP_ETHERTYPE_ALPHA		0x814A	/* Alpha Micro */
#define	NFP_ETHERTYPE_SNMP		0x814C	/* SNMP over Ethernet (see RFC1089) */
		    /* 0x814D - 0x814E	   BIIN */
#define	NFP_ETHERTYPE_TEC	0x814F	/* Technically Elite Concepts */
#define	NFP_ETHERTYPE_RATIONAL	0x8150	/* Rational Corp */
		    /* 0x8151 - 0x8153	   Qualcomm */
		    /* 0x815C - 0x815E	   Computer Protocol Pty Ltd */
		    /* 0x8164 - 0x8166	   Charles River Data Systems */
#define	NFP_ETHERTYPE_XTP		0x817D	/* Protocol Engines XTP */
#define	NFP_ETHERTYPE_SGITW		0x817E	/* SGI/Time Warner prop. */
#define	NFP_ETHERTYPE_HIPPI_FP	0x8180	/* HIPPI-FP encapsulation */
#define	NFP_ETHERTYPE_STP		0x8181	/* Scheduled Transfer STP, HIPPI-ST */
		    /* 0x8182 - 0x8183	   Reserved for HIPPI-6400 */
		    /* 0x8184 - 0x818C	   SGI prop. */
#define	NFP_ETHERTYPE_MOTOROLA	0x818D	/* Motorola */
#define	NFP_ETHERTYPE_NETBEUI	0x8191	/* PowerLAN NetBIOS/NetBEUI (PC) */
		    /* 0x819A - 0x81A3	   RAD Network Devices */
		    /* 0x81B7 - 0x81B9	   Xyplex */
		    /* 0x81CC - 0x81D5	   Apricot Computers */
		    /* 0x81D6 - 0x81DD	   Artisoft Lantastic */
		    /* 0x81E6 - 0x81EF	   Polygon */
		    /* 0x81F0 - 0x81F2	   Comsat Labs */
		    /* 0x81F3 - 0x81F5	   SAIC */
		    /* 0x81F6 - 0x81F8	   VG Analytical */
		    /* 0x8203 - 0x8205	   QNX Software Systems Ltd. */
		    /* 0x8221 - 0x8222	   Ascom Banking Systems */
		    /* 0x823E - 0x8240	   Advanced Encryption Systems */
		    /* 0x8263 - 0x826A	   Charles River Data Systems */
		    /* 0x827F - 0x8282	   Athena Programming */
		    /* 0x829A - 0x829B	   Inst Ind Info Tech */
		    /* 0x829C - 0x82AB	   Taurus Controls */
		    /* 0x82AC - 0x8693	   Walker Richer & Quinn */
#define	NFP_ETHERTYPE_ACCTON	0x8390	/* Accton Technologies (unregistered) */
#define	NFP_ETHERTYPE_TALARISMC	0x852B	/* Talaris multicast */
#define	NFP_ETHERTYPE_KALPANA	0x8582	/* Kalpana */
		    /* 0x8694 - 0x869D	   Idea Courier */
		    /* 0x869E - 0x86A1	   Computer Network Tech */
		    /* 0x86A3 - 0x86AC	   Gateway Communications */
#define	NFP_ETHERTYPE_SECTRA	0x86DB	/* SECTRA */
#define	NFP_ETHERTYPE_IPV6		0x86DD	/* IP protocol version 6 */
#define	NFP_ETHERTYPE_DELTACON	0x86DE	/* Delta Controls */
#define	NFP_ETHERTYPE_ATOMIC	0x86DF	/* ATOMIC */
		    /* 0x86E0 - 0x86EF	   Landis & Gyr Powers */
		    /* 0x8700 - 0x8710	   Motorola */
#define	NFP_ETHERTYPE_RDP		0x8739	/* Control Technology Inc. RDP Without IP */
#define	NFP_ETHERTYPE_MICP		0x873A	/* Control Technology Inc. Mcast Industrial Ctrl Proto. */
		    /* 0x873B - 0x873C	   Control Technology Inc. Proprietary */
#define	NFP_ETHERTYPE_TCPCOMP	0x876B	/* TCP/IP Compression (RFC1701) */
#define	NFP_ETHERTYPE_IPAS		0x876C	/* IP Autonomous Systems (RFC1701) */
#define	NFP_ETHERTYPE_SECUREDATA	0x876D	/* Secure Data (RFC1701) */
#define	NFP_ETHERTYPE_FLOWCONTROL	0x8808	/* 802.3x flow control packet */
#define	NFP_ETHERTYPE_SLOW		0x8809	/* 802.3ad link aggregation (LACP) */
#define	NFP_ETHERTYPE_PPP		0x880B	/* PPP (obsolete by PPPoE) */
#define	NFP_ETHERTYPE_HITACHI	0x8820	/* Hitachi Cable (Optoelectronic Systems Laboratory) */
#define	NFP_ETHERTYPE_MPLS		0x8847	/* MPLS Unicast */
#define	NFP_ETHERTYPE_MPLS_MCAST	0x8848	/* MPLS Multicast */
#define	NFP_ETHERTYPE_AXIS		0x8856	/* Axis Communications AB proprietary bootstrap/config */
#define	NFP_ETHERTYPE_PPPOEDISC	0x8863	/* PPP Over Ethernet Discovery Stage */
#define	NFP_ETHERTYPE_PPPOE		0x8864	/* PPP Over Ethernet Session Stage */
#define	NFP_ETHERTYPE_LANPROBE	0x8888	/* HP LanProbe test? */
#define	NFP_ETHERTYPE_PAE		0x888e	/* EAPOL PAE/802.1x */
#define	NFP_ETHERTYPE_QINQ_STD	0x88A8  /* 802.1ad QinQ */
#define	NFP_ETHERTYPE_LOOPBACK	0x9000	/* Loopback: used to test interfaces */
#define	NFP_ETHERTYPE_LBACK		NFP_ETHERTYPE_LOOPBACK	/* DEC MOP loopback */
#define	NFP_ETHERTYPE_XNSSM		0x9001	/* 3Com (Formerly Bridge Communications), XNS Systems Management */
#define	NFP_ETHERTYPE_TCPSM		0x9002	/* 3Com (Formerly Bridge Communications), TCP/IP Systems Management */
#define	NFP_ETHERTYPE_BCLOOP	0x9003	/* 3Com (Formerly Bridge Communications), loopback detection */
#define	NFP_ETHERTYPE_QINQ_VENDOR1	0x9100  /* Vendor-specific QinQ */
#define	NFP_ETHERTYPE_QINQ_VENDOR2	0x9200  /* Vendor-specific QinQ */
#define	NFP_ETHERTYPE_QINQ_VENDOR3	0x9300  /* Vendor-specific QinQ */
#define	NFP_ETHERTYPE_DEBNI		0xAAAA	/* DECNET? Used by VAX 6220 DEBNI */
#define	NFP_ETHERTYPE_SONIX		0xFAF5	/* Sonix Arpeggio */
#define	NFP_ETHERTYPE_VITAL		0xFF00	/* BBN VITAL-LanBridge cache wakeups */
		    /* 0xFF00 - 0xFFOF	   ISC Bunker Ramo */

#define	NFP_ETHERTYPE_MAX		0xFFFF	/* Maximum valid ethernet type, reserved */

/*
 * The NFP_NFP_ETHERTYPE_NTRAILER packet types starting at NFP_NFP_ETHERTYPE_TRAIL have
 * (type-NFP_ETHERTYPE_TRAIL)*512 bytes of data followed
 * by an NFP_ETHER type (as given above) and then the (variable-length) header.
 */
#define	NFP_ETHERTYPE_TRAIL		0x1000		/* Trailer packet */
#define	NFP_ETHERTYPE_NTRAILER	16

#define	NFP_ETHERMTU	(NFP_ETHER_MAX_LEN-NFP_ETHER_HDR_LEN-NFP_ETHER_CRC_LEN)
#define	NFP_ETHERMIN	(NFP_ETHER_MIN_LEN-NFP_ETHER_HDR_LEN-NFP_ETHER_CRC_LEN)
#define	NFP_ETHERMTU_JUMBO	(NFP_ETHER_MAX_LEN_JUMBO - NFP_ETHER_HDR_LEN - NFP_ETHER_CRC_LEN)

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_ETHERNET_H__ */
