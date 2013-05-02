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

#include "config.h"	/* Always first, defines CFG_EXTAL for jz4740.h */
#include "jz4740.h"

#include "serial.h"
#include "mmc.h"
#include "board.h"

#define MSC_RESPONSE_R1 3
#define MSC_RESPONSE_R2 8
#define MSC_RESPONSE_R3 3

static unsigned int is_sdhc;

static inline void jz_mmc_stop_clock(void)
{
	uint32_t timeout = 1000;

	__msc_stop_clk();
	while (timeout-- && (__msc_get_stat() & MSC_STAT_CLK_EN))
		udelay(1);
}

static inline void jz_mmc_start_clock(void)
{
	__msc_start_op();
}

static void mmc_cmd(uint16_t cmd, unsigned int arg, unsigned int cmdat, int words, uint8_t *resp)
{
	uint32_t timeout = 0x3fffff;
	int i;

	jz_mmc_stop_clock();
	__msc_set_cmd(cmd);
	__msc_set_arg(arg);
	__msc_set_cmdat(cmdat);
	__msc_unmask_endcmdres();
	jz_mmc_start_clock();

	while (timeout-- && __msc_stat_not_end_cmd_res());

	__msc_ireg_clear_end_cmd_res();

	if (!words)
		return;

	for (i = words-1; i >= 0; i--) {
		uint16_t res_fifo = __msc_rd_resfifo();
		int offset = i << 1;

		resp[offset] = ((uint8_t *) &res_fifo) [0];
		resp[offset + 1] = ((uint8_t *) &res_fifo) [1];
	}
}

int mmc_block_read(uint32_t *dst, uint32_t src, uint32_t num_blocks)
{
	uint8_t resp[20];

	mmc_cmd(16, 0x200, 0x401, MSC_RESPONSE_R1, resp);
	__msc_set_blklen(0x200);
	__msc_set_nob(num_blocks);

	if (is_sdhc) 
		mmc_cmd(18, src, 0x409, MSC_RESPONSE_R1, resp);
	else
		mmc_cmd(18, src * MMC_SECTOR_SIZE, 0x409, MSC_RESPONSE_R1, resp);

	for (; num_blocks >= 1; num_blocks--) {
		uint32_t cnt = 128, timeout = 0x3ffffff;

		while (--timeout) {
			uint32_t stat = __msc_get_stat();

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
			while (__msc_get_stat() & MSC_STAT_DATA_FIFO_EMPTY);
			*dst++ = __msc_rd_rxfifo();
		}
	}

	mmc_cmd(12, 0, 0x41, MSC_RESPONSE_R1, resp);
	jz_mmc_stop_clock();

	return 0;
}

int mmc_init(void)
{
	uint8_t resp[20];
	uint32_t rca, retries = 2000;

	__msc_reset();
	__msc_mask_all_intrs();
	__msc_set_clkrt(7);

	/* reset */
	mmc_cmd(0, 0, 0x80, 0, resp);
	mmc_cmd(8, 0x1aa, 0x1, MSC_RESPONSE_R1, resp);

	do {
		mmc_cmd(55, 0, 0x1, MSC_RESPONSE_R1, resp);
		mmc_cmd(41, 0x40ff8000, 0x3, MSC_RESPONSE_R3, resp);
	} while (!(resp[4] & 0x80) && --retries);

	if (!retries) {
		/* Initialization failed. */
		SERIAL_PUTI(0x10);
		return -1;
	}

	/* try to get card id */
	mmc_cmd(2, 0, 0x2, MSC_RESPONSE_R2, resp);
	mmc_cmd(3, 0, 0x6, MSC_RESPONSE_R1, resp);
	rca = ((resp[4] << 8) | resp[3]) << 16; 

	mmc_cmd(9, rca, 0x2, MSC_RESPONSE_R2, resp);
	is_sdhc = (resp[14] & 0xc0) >> 6;

	__msc_set_clkrt(0);
	mmc_cmd(7, rca, 0x41, MSC_RESPONSE_R1, resp);
	mmc_cmd(55, rca, 0x1, MSC_RESPONSE_R1, resp);
	mmc_cmd(6, 0x2, 0x401, MSC_RESPONSE_R1, resp);
	return 0;
}

