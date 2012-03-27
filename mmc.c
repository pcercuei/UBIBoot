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

static unsigned int is_sdhc;

static inline void jz_mmc_stop_clock(void)
{
	uint32_t timeout = 1000;

	__msc_stop_clk();
	while (timeout-- && (__msc_get_stat() & MSC_STAT_CLK_EN)) {
		uint32_t wait = 12; /* 1us */

		if (!timeout)
			return;
		while (wait--);
	}
}

static inline void jz_mmc_start_clock(void)
{
	__msc_start_op();
}

static void mmc_cmd(uint16_t cmd, unsigned int arg, unsigned int cmdat, uint16_t rtype, uint8_t *resp)
{
	uint32_t timeout = 0x3fffff;
	int words, i;

	jz_mmc_stop_clock();
	__msc_set_cmd(cmd);
	__msc_set_arg(arg);
	__msc_set_cmdat(cmdat);
	__msc_unmask_endcmdres();
	jz_mmc_start_clock();

	while (timeout-- && __msc_stat_not_end_cmd_res());

	__msc_ireg_clear_end_cmd_res();

	switch (rtype) {
		case MSC_CMDAT_RESPONSE_R1:
		case MSC_CMDAT_RESPONSE_R3:
			words = 3;
			break;
		case MSC_CMDAT_RESPONSE_R2:
			words = 8;
			break;
		default:
			return;
	}

	for (i = words-1; i >= 0; i--) {
		uint16_t res_fifo = __msc_rd_resfifo();
		int offset = i << 1;

		resp[offset] = ((uint8_t *) &res_fifo) [0];
		resp[offset + 1] = ((uint8_t *) &res_fifo) [1];
	}
}

int mmc_block_read(uint8_t *dst, uint32_t src, size_t nb)
{
	uint32_t nob = nb;
	uint8_t resp[20];

	mmc_cmd(16, 0x200, 0x401, MSC_CMDAT_RESPONSE_R1, resp);
	__msc_set_blklen(0x200);
	__msc_set_nob(nob);

	if (is_sdhc) 
		mmc_cmd(18, src, 0x409, MSC_CMDAT_RESPONSE_R1, resp);
	else
		mmc_cmd(18, src * MMC_SECTOR_SIZE, 0x409, MSC_CMDAT_RESPONSE_R1, resp);

	for (; nob >= 1; nob--) {
		uint32_t cnt = 128, timeout = 0x3ffffff;

		while (timeout--) {
			uint32_t wait = 12,
					 stat = __msc_get_stat();

			if (stat & MSC_STAT_TIME_OUT_READ) {
				SERIAL_PUTS("MMC: Time out!\n");
				return -1;
			}
			else if (stat & MSC_STAT_CRC_READ_ERROR) {
				SERIAL_PUTS("MMC: Read error!\n");
				return -1;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)) {
				/* Ready to read data */
				break;
			}

			while (wait--);
		}

		if (!timeout) {
			SERIAL_PUTS("MMC: Time out!\n");
			return -1;
		}

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
		while (cnt--) {
			uint32_t data;

			while (__msc_get_stat() & MSC_STAT_DATA_FIFO_EMPTY);
			data = __msc_rd_rxfifo();

			*dst++ = (uint8_t)(data >> 0);
			*dst++ = (uint8_t)(data >> 8);
			*dst++ = (uint8_t)(data >> 16);
			*dst++ = (uint8_t)(data >> 24);
		}
	}

	mmc_cmd(12, 0, 0x41, MSC_CMDAT_RESPONSE_R1, resp);
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
	mmc_cmd(8, 0x1aa, 0x1, MSC_CMDAT_RESPONSE_R1, resp);
	mmc_cmd(55, 0, 0x1, MSC_CMDAT_RESPONSE_R1, resp);

	mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3, resp);

	while (retries-- && !(resp[4] & 0x80)) {
		uint32_t wait = 33600000;

		mmc_cmd(55, 0, 0x1, MSC_CMDAT_RESPONSE_R1, resp);
		mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3, resp);
		while (wait--);
	}

	if (!(resp[4] & 0x80)) {
		SERIAL_PUTS("MMC: initialization failed.\n");
		return -1;
	}

	/* try to get card id */
	mmc_cmd(2, 0, 0x2, MSC_CMDAT_RESPONSE_R2, resp);
	mmc_cmd(3, 0, 0x6, MSC_CMDAT_RESPONSE_R1, resp);
	rca = ((resp[4] << 8) | resp[3]) << 16; 

	mmc_cmd(9, rca, 0x2, MSC_CMDAT_RESPONSE_R2, resp);
	is_sdhc = (resp[14] & 0xc0) >> 6;

	__msc_set_clkrt(0);
	mmc_cmd(7, rca, 0x41, MSC_CMDAT_RESPONSE_R1, resp);
	mmc_cmd(55, rca, 0x1, MSC_CMDAT_RESPONSE_R1, resp);
	mmc_cmd(6, 0x2, 0x401, MSC_CMDAT_RESPONSE_R1, resp);
	return 0;
}

