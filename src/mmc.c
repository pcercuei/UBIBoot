/*
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "config.h"

#include "errorcodes.h"
#include "serial.h"
#include "mmc.h"
#include "board.h"
#include "utils.h"
#include "jz.h"
#include "jz4740-mmc.h"

#define CMD_GO_IDLE_STATE	0
#define CMD_SEND_OP_COND	1
#define CMD_ALL_SEND_CID	2
#define CMD_SEND_RCA		3
#define CMD_SWITCH		6
#define CMD_SELECT		7
#define CMD_SEND_IF_COND	8
#define CMD_SEND_CSD		9
#define CMD_STOP_TRANSMISSION	12
#define CMD_SET_BLOCKLEN	16
#define CMD_READ_MULTIPLE	18
#define CMD_APP_CMD		55

#define ACMD_SET_BUS_WIDTH	6
#define ACMD_SD_SEND_OP_COND	41

#ifndef MMC_1BIT
#define CMDAT_4BIT		BIT(10)
#define MMC_BUS_WIDTH		0x2
#else
#define CMDAT_4BIT		0
#define MMC_BUS_WIDTH		0x0
#endif

#define MMC_OCR_HCS      (0x2 << 29)
#define MMC_OCR_HCS_MASK (0x3 << 29)
#define MMC_OCR_BUSY     BIT(31)

enum response {
	MSC_NO_RESPONSE,
	MSC_RESPONSE_R1,
	MSC_RESPONSE_R2,
	MSC_RESPONSE_R3,
	MSC_RESPONSE_R4,
	MSC_RESPONSE_R5,
	MSC_RESPONSE_R6,
	MSC_RESPONSE_R7,
	MSC_RESPONSE_MAX,
};

static const u8 response_size[] = {
	0, 3, 8, 3, 0, 0, 3, 3,
};

static bool is_sdhc;

static inline void jz_mmc_stop_clock(unsigned int id)
{
	uint32_t timeout = 1000;

	__msc_stop_clk(id);
	while (--timeout && (__msc_get_stat(id) & MSC_STAT_CLK_EN))
		udelay(1);
}

static inline void jz_mmc_start_clock(unsigned int id)
{
	__msc_start_op(id);
}

static int mmc_cmd(unsigned int id, uint16_t cmd, uint32_t arg,
			uint32_t flags, uint8_t resp_type, uint16_t *resp)
{
	uint32_t timeout = 0x3fffff;
	uint8_t words = response_size[resp_type];
	unsigned int i;

	jz_mmc_stop_clock(id);
	__msc_set_cmd(id, cmd);
	__msc_set_arg(id, arg);
	__msc_set_cmdat(id, resp_type | flags);
	__msc_unmask_endcmdres(id);
	jz_mmc_start_clock(id);

	while (--timeout && __msc_stat_not_end_cmd_res(id));

	if (!timeout || __msc_stat_resto_err(id))
		return ERR_MMC_TIMEOUT;

	__msc_ireg_clear_end_cmd_res(id);

	for (i = words; i; i--)
		resp[i - 1] = __msc_rd_resfifo(id);

	return 0;
}

void mmc_start_block(unsigned int id, uint32_t src, uint32_t num_blocks)
{
	uint16_t resp[MSC_RESPONSE_MAX];

	mmc_cmd(id, CMD_SET_BLOCKLEN, MMC_SECTOR_SIZE, CMDAT_4BIT, MSC_RESPONSE_R1, resp);

	jz_mmc_stop_clock(id);
	__msc_set_nob(id, num_blocks);
	__msc_set_blklen(id, MMC_SECTOR_SIZE);

	if (is_sdhc)
		mmc_cmd(id, CMD_READ_MULTIPLE, src, CMDAT_4BIT | CMDAT_DATA_EN, MSC_RESPONSE_R1, resp);
	else
		mmc_cmd(id, CMD_READ_MULTIPLE, src * MMC_SECTOR_SIZE, CMDAT_4BIT | CMDAT_DATA_EN, MSC_RESPONSE_R1, resp);
}

void mmc_stop_block(unsigned int id)
{
	uint16_t resp[MSC_RESPONSE_MAX];

	mmc_cmd(id, CMD_STOP_TRANSMISSION, 0, CMDAT_BUSY, MSC_RESPONSE_R1, resp);
	jz_mmc_stop_clock(id);
}

int mmc_receive_block(unsigned int id, uint32_t *dst)
{
	uint32_t cnt = 128, timeout = 0x3ffffff;

	while (--timeout) {
		uint32_t stat = __msc_get_stat(id);

		if (stat & MSC_STAT_TIME_OUT_READ)
			return ERR_MMC_TIMEOUT;
		if (stat & MSC_STAT_CRC_READ_ERROR)
			return ERR_MMC_IO;
		if (!(stat & MSC_STAT_DATA_FIFO_EMPTY))
			break; /* Ready to read data */

		udelay(1);
	}

	if (!timeout)
		return ERR_MMC_TIMEOUT;

	/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
	while (cnt--) {
		while (__msc_get_stat(id) & MSC_STAT_DATA_FIFO_EMPTY);
		*dst++ = __msc_rd_rxfifo(id);
	}

	return 0;
}

int mmc_block_read(unsigned int id, uint32_t *dst,
			uint32_t src, uint32_t num_blocks)
{
	int err = 0;

	mmc_start_block(id, src, num_blocks);

	for (; num_blocks >= 1; num_blocks--) {
		err = mmc_receive_block(id, dst);
		if (err)
			break;
		dst += 128;
	}

	mmc_stop_block(id);

	return err;
}

int mmc_init(unsigned int id)
{
	uint16_t resp[MSC_RESPONSE_MAX];
	unsigned int retries;
	uint32_t rca;
	int ret;


	__msc_set_rdto(id, 0xffff);

	__msc_reset(id);
	__msc_mask_all_intrs(id);
	__msc_set_clkrt(id, 7);

	/* reset */
	mmc_cmd(id, CMD_GO_IDLE_STATE, 0, CMDAT_INIT, MSC_NO_RESPONSE, resp);

	/* 0x100 -> VHS in 2.7-3.6V
	 * 0x0aa -> pattern to read back */
	mmc_cmd(id, CMD_SEND_IF_COND, 0x1aa, 0x0, MSC_RESPONSE_R7, resp);

	for (retries = 1000; retries; retries--) {
		ret = mmc_cmd(id, CMD_APP_CMD, 0, 0x0, MSC_RESPONSE_R1, resp);
		if (ret)
			break;

		/* 0x3000 OCR: Operating range 3.2-3.4V */
		/* BIT(30): Host supports SDHC/SDXC */
		ret = mmc_cmd(id, ACMD_SD_SEND_OP_COND, 0x40300000, 0x0, MSC_RESPONSE_R3, resp);
		if (ret)
			break;

		/* Poll until the SD card sets the 'ready' bit */
		if (resp[2] & BIT(7))
			break;

		udelay(1000);
	}

	if (!retries) {
		SERIAL_ERR(ERR_MMC_SEND_OP_COND);
		return -1;
	}

	bool is_sd = 1;

	if (ret) {
		if (ret == ERR_MMC_TIMEOUT) {
			uint32_t ocr;
			uint32_t arg = 0;
			for (retries = 1000; retries; retries--)
			{
				ret = mmc_cmd(id, CMD_SEND_OP_COND, arg, 0x0, MSC_RESPONSE_R3, resp);
				if (ret)
					break;

				ocr = (resp[0] >> 8) | (resp[1] << 8) | (resp[2] << 24);

				if (ocr & MMC_OCR_BUSY)
					break;

				arg = ocr | MMC_OCR_HCS;

				udelay(1000);
			}

			if (!retries || ret) {
				int err;
				if (!retries)
					err = ERR_MMC_SEND_OP_COND_TOUT;
				else
					err = ERR_MMC_SEND_OP_COND_ERR;

				SERIAL_ERR(err);

				return -1;
			}

			is_sd = false;
		}
		else
		{
			SERIAL_ERR(ERR_MMC_NTOUT);
			return -1;
		}
	}

	/* try to get card id */
	ret = mmc_cmd(id, CMD_ALL_SEND_CID, 0, 0x0, MSC_RESPONSE_R2, resp);
	if (ret) {
		SERIAL_ERR(ERR_MMC_CID);
		return -1;
	}

	// set address
	rca = 1;

	ret = mmc_cmd(id, CMD_SEND_RCA, rca << 16, 0x0, MSC_RESPONSE_R6, resp);
	if (ret) {
		SERIAL_ERR(ERR_MMC_RCA);
		return -1;
	}

	if (is_sd)
		rca = ((resp[2] & 0x00FF) << 8) | ((resp[1] & 0xFF00) >> 8);

	ret = mmc_cmd(id, CMD_SEND_CSD, rca << 16, 0x0, MSC_RESPONSE_R2, resp);
	if (ret) {
		SERIAL_ERR(ERR_MMC_CSD);
		return -1;
	}

	is_sdhc = resp[7] & 0xc0;

#ifdef USE_SERIAL
	if (is_sd) {
		if (is_sdhc) {
			unsigned int size = (resp[2] >> 8) | (((resp[3] & 0x3fff) << 8) + 1) / 2;

			SERIAL_PUTS_ARGI("Detected a SDHC card of ", size, " MiB.\n");
		} else {
			/* TODO: calculate size */
			SERIAL_PUTS("SD card detected.\n");
		}
	}
#endif

	__msc_set_clkrt(id, 0);
	ret = mmc_cmd(id, CMD_SELECT, rca << 16, 0, MSC_RESPONSE_R1, resp);
	if (ret) {
		SERIAL_ERR(ERR_MMC_SELECT);
		return -1;
	}

	/* Switch to 4-bit mode */
	if (is_sd) {
		ret = mmc_cmd(id, CMD_APP_CMD, rca << 16, 0x0, MSC_RESPONSE_R1, resp);
		if (ret) {
			SERIAL_ERR(ERR_SD_CMD);
			return -1;
		}

		ret = mmc_cmd(id, ACMD_SET_BUS_WIDTH, MMC_BUS_WIDTH, CMDAT_4BIT, MSC_RESPONSE_R1, resp);
		if (ret) {
			SERIAL_ERR(ERR_SD_BW);
			return -1;
		}
	} else {
		/* 0x03 -> set register command
		 * 0xb7 -> register address
		 * 0x01 -> register value
		 * this sets 4-bit bus width */
		ret = mmc_cmd(id, CMD_SWITCH, 0x3b70100, CMDAT_4BIT | CMDAT_BUSY, MSC_RESPONSE_R1, resp);
		if (ret) {
			SERIAL_ERR(ERR_MMC_SWITCH);
			return -1;
		}
	}

	return 0;
}
