/*	$OpenBSD: sdhcvar.h,v 1.6 2011/07/31 16:55:01 kettenis Exp $	*/

/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
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

#ifndef _SDHCVAR_H_
#define _SDHCVAR_H_

#include <machine/bus.h>

struct sdhc_host;
struct sdmmc_command;

struct sdhc_softc {
	struct device		sc_dev;
	struct sdhc_host	**sc_host;
	int			sc_nhosts;
	bus_dma_tag_t		sc_dmat;
	uint32_t		sc_flags;
	uint32_t		sc_caps;
	uint32_t		sc_clkbase;
	int			sc_clkmsk;	/* Mask for SDCLK */

	int (*sc_vendor_rod)(struct sdhc_softc *, int);
	int (*sc_vendor_card_detect)(struct sdhc_softc *);
	int (*sc_vendor_bus_clock)(struct sdhc_softc *, int);
	int (*sc_vendor_transfer_data_dma)(struct sdhc_softc *, struct sdmmc_command *);
};

/* Host controller functions called by the attachment driver. */
int	sdhc_host_found(struct sdhc_softc *, bus_space_tag_t,
	    bus_space_handle_t, bus_size_t);
int	sdhc_activate(struct device *, int);
void	sdhc_shutdown(void *);
int	sdhc_intr(void *);

/* flag values */
#define	SDHC_F_USE_DMA		0x00000001
#define	SDHC_F_FORCE_DMA	0x00000002
#define	SDHC_F_NOPWR0		0x00000004
#define	SDHC_F_32BIT_ACCESS	0x00000010
#define	SDHC_F_ENHANCED		0x00000020	/* Freescale ESDHC */
#define	SDHC_F_8BIT_MODE	0x00000040	/* MMC 8bit mode is supported */
#define	SDHC_F_NO_LED_ON	0x00000100	/* LED_ON unsupported in HOST_CTL */
#define	SDHC_F_HOSTCAPS		0x00000200	/* No device provided capabilities */
#define	SDHC_F_RSP136_CRC	0x00000400	/* Resp 136 with CRC and end-bit */
#define	SDHC_F_SINGLE_ONLY	0x00000800	/* Single transfer only */
#define	SDHC_F_WAIT_RESET	0x00001000	/* Wait for soft resets to start */
#define	SDHC_F_NO_HS_BIT	0x00002000	/* Don't set SDHC_HIGH_SPEED bit */
#define	SDHC_F_EXTERNAL_DMA	0x00004000
#define	SDHC_F_EXTDMA_DMAEN	0x00008000	/* ext. dma need SDHC_DMA_ENABLE */
#define	SDHC_F_SINGLE_POWER_WRITE 0x00040000

#endif
