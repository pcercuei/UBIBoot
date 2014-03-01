/*
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdlib.h>
#include <stdint.h>

#include "config.h"

#include "serial.h"
#include "mmc.h"
#include "board.h"
#include "jz.h"

#define MSC_RESPONSE_R1 3
#define MSC_RESPONSE_R2 8
#define MSC_RESPONSE_R3 3

static unsigned int is_sdhc;

static inline void jz_mmc_stop_clock(unsigned int id)
{
	uint32_t timeout = 1000;

	__msc_stop_clk(id);
	while (timeout-- && (__msc_get_stat(id) & MSC_STAT_CLK_EN))
		udelay(1);
}

static inline void jz_mmc_start_clock(unsigned int id)
{
	__msc_start_op(id);
}

static void mmc_cmd(unsigned int id, uint16_t cmd,
			unsigned int arg, unsigned int cmdat, int words, uint8_t *resp)
{
	uint32_t timeout = 0x3fffff;
	int i;

	jz_mmc_stop_clock(id);
	__msc_set_cmd(id, cmd);
	__msc_set_arg(id, arg);
	__msc_set_cmdat(id, cmdat);
	__msc_unmask_endcmdres(id);
	jz_mmc_start_clock(id);

	while (timeout-- && __msc_stat_not_end_cmd_res(id));

	__msc_ireg_clear_end_cmd_res(id);

	if (!words)
		return;

	for (i = words-1; i >= 0; i--) {
		uint16_t res_fifo = __msc_rd_resfifo(id);
		int offset = i << 1;

		resp[offset] = ((uint8_t *) &res_fifo) [0];
		resp[offset + 1] = ((uint8_t *) &res_fifo) [1];
	}
}

int mmc_block_read(unsigned int id, uint32_t *dst,
			uint32_t src, uint32_t num_blocks)
{
	uint8_t resp[20];

	mmc_cmd(id, 16, 0x200, 0x401, MSC_RESPONSE_R1, resp);
	__msc_set_blklen(id, 0x200);
	__msc_set_nob(id, num_blocks);

	if (is_sdhc) 
		mmc_cmd(id, 18, src, 0x409, MSC_RESPONSE_R1, resp);
	else
		mmc_cmd(id, 18, src * MMC_SECTOR_SIZE, 0x409, MSC_RESPONSE_R1, resp);

	for (; num_blocks >= 1; num_blocks--) {
		uint32_t cnt = 128, timeout = 0x3ffffff;

		while (--timeout) {
			uint32_t stat = __msc_get_stat(id);

			if (stat & MSC_STAT_TIME_OUT_READ) {
				/* Time out. */
				SERIAL_PUTI(0x11);
				return -1;
			}
			else if (stat & MSC_STAT_CRC_READ_ERROR) {
				/* Read error. */
				SERIAL_PUTI(0x12);
				return -1;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)) {
				/* Ready to read data */
				break;
			}

			udelay(1);
		}

		if (!timeout) {
			/* Time out. */
			SERIAL_PUTI(0x11);
			return -1;
		}

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
		while (cnt--) {
			while (__msc_get_stat(id) & MSC_STAT_DATA_FIFO_EMPTY);
			*dst++ = __msc_rd_rxfifo(id);
		}
	}

	mmc_cmd(id, 12, 0, 0x41, MSC_RESPONSE_R1, resp);
	jz_mmc_stop_clock(id);

	return 0;
}

int mmc_init(unsigned int id)
{
	uint8_t resp[20];
	uint32_t rca, retries = 2000;

#if JZ_VERSION >= 4770
	if (id > 2)
		return -1;
#else
	if (id)
		return -1;
#endif

	__msc_set_rdto(id, 0xffff);

	__msc_reset(id);
	__msc_mask_all_intrs(id);
	__msc_set_clkrt(id, 7);

	/* reset */
	mmc_cmd(id, 0, 0, 0x80, 0, resp);
	mmc_cmd(id, 8, 0x1aa, 0x1, MSC_RESPONSE_R1, resp);

	do {
		mmc_cmd(id, 55, 0, 0x1, MSC_RESPONSE_R1, resp);
		mmc_cmd(id, 41, 0x40ff8000, 0x3, MSC_RESPONSE_R3, resp);
	} while (!(resp[4] & 0x80) && --retries);

	if (!retries) {
		/* Initialization failed. */
		SERIAL_PUTI(0x10);
		return -1;
	}

	/* try to get card id */
	mmc_cmd(id, 2, 0, 0x2, MSC_RESPONSE_R2, resp);
	mmc_cmd(id, 3, 0, 0x6, MSC_RESPONSE_R1, resp);
	rca = ((resp[4] << 8) | resp[3]) << 16; 

	mmc_cmd(id, 9, rca, 0x2, MSC_RESPONSE_R2, resp);
	is_sdhc = (resp[14] & 0xc0) >> 6;

	__msc_set_clkrt(id, 0);
	mmc_cmd(id, 7, rca, 0x41, MSC_RESPONSE_R1, resp);
	mmc_cmd(id, 55, rca, 0x1, MSC_RESPONSE_R1, resp);
	mmc_cmd(id, 6, 0x2, 0x401, MSC_RESPONSE_R1, resp);
	return 0;
}
