/*	$OpenBSD: conf.c,v 1.15 2007/05/30 01:25:43 tom Exp $	*/

/*
 * Copyright (c) 2004 Tom Cosgrove
 * Copyright (c) 1996 Michael Shalayeff
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
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <netinet/in.h>
#include <libsa.h>
#include <lib/libsa/ufs.h>
#ifdef notdef
#include <lib/libsa/cd9660.h>
#include <lib/libsa/fat.h>
#endif
#include <lib/libsa/nfs.h>
#include <lib/libsa/tftp.h>
#include <lib/libsa/netif.h>
#include <lib/libsa/unixdev.h>
#include <biosdev.h>
#include <dev/cons.h>
#include "pxeboot.h"
#include "pxe_net.h"

const char version[] = "2.00";
int	debug = 0;

#undef _TEST


void (*sa_cleanup)(void) = pxe_shutdown;


void (*i386_probe1[])(void) = {
	gateA20on, cninit, pxeprobe, memprobe
};
void (*i386_probe2[])(void) = {
	diskprobe
};
void (*i386_probe3[])(void) = {
	pxeinfo
/*	netprobe_pxe, netprobe_mac, netprobe_inet4, netprobe_bootdev */
};

struct i386_boot_probes probe_list[] = {
	{ "probing", i386_probe1, NENTS(i386_probe1) },
	{ "disk",    i386_probe2, NENTS(i386_probe2) },
	{ "net",     i386_probe3, NENTS(i386_probe3) },
};
int nibprobes = NENTS(probe_list);

/* This next list must match file_system[]. */
char *fs_name[] = {
	NULL, "tftp", "nfs"
};
int nfsname = NENTS(fs_name);

struct fs_ops file_system[] = {
	{ ufs_open,    ufs_close,    ufs_read,    ufs_write,    ufs_seek,
	  ufs_stat,    ufs_readdir    },
	{ tftp_open,   tftp_close,   tftp_read,   tftp_write,   tftp_seek,
	  tftp_stat,   tftp_readdir   },
	{ nfs_open,    nfs_close,    nfs_read,    nfs_write,    nfs_seek,
	  nfs_stat,    nfs_readdir    },
#ifdef notdef
	{ fat_open,    fat_close,    fat_read,    fat_write,    fat_seek,
	  fat_stat,    fat_readdir    },
	{ cd9660_open, cd9660_close, cd9660_read, cd9660_write, cd9660_seek,
	  cd9660_stat, cd9660_readdir },
#endif
#ifdef _TEST
	{ null_open,   null_close,   null_read,   null_write,   null_seek,
	  null_stat,   null_readdir   }
#endif
};
int nfsys = NENTS(file_system);

struct devsw	devsw[] = {
#ifdef _TEST
	{ "UNIX", unixstrategy, unixopen, unixclose, unixioctl },
#else
	{ "BIOS", biosstrategy, biosopen, biosclose, biosioctl },
#endif
};
int ndevs = NENTS(devsw);

struct devsw	netsw[] = {
	{ "net",  net_strategy, net_open, net_close, net_ioctl },
};

struct netif_driver	*netif_drivers[] = {
};
int n_netif_drivers = NENTS(netif_drivers);

struct consdev constab[] = {
#ifdef _TEST
	{ unix_probe, unix_init, unix_getc, unix_putc },
#else
	{ pc_probe, pc_init, pc_getc, pc_putc },
	{ com_probe, com_init, com_getc, com_putc },
#endif
	{ NULL }
};
struct consdev *cn_tab = constab;
