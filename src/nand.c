/*
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Author: Peter <jlwei@ingenic.cn>
 *
 * Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "bch.h"
#include "board.h"
#include "serial.h"
#include "nand.h"
#include "config.h"
#include "errorcodes.h"
#include "jz.h"

#include "jz4740-emc.h"

/*
 * NAND flash definitions
 */

#define NAND_DATAPORT	0xb8000000
#define NAND_ADDRPORT	0xb8010000
#define NAND_COMMPORT	0xb8008000

#define __nand_enable()		(REG_EMC_NFCSR |= EMC_NFCSR_NFE1 | EMC_NFCSR_NFCE1)
#define __nand_disable()	(REG_EMC_NFCSR &= ~(EMC_NFCSR_NFCE1))

#define __nand_cmd(n)		(REG8(NAND_COMMPORT) = (n))
#define __nand_addr(n)		(REG8(NAND_ADDRPORT) = (n))
#define __nand_data8()		REG8(NAND_DATAPORT)
#define __nand_data16()		REG16(NAND_DATAPORT)

#define OOB_SIZE 	(PAGE_SIZE / 32)

/*
 * NAND flash routines
 */

#if (BUS_WIDTH == 16)
static void nand_read_buf(void *buf, size_t count)
{
	size_t i;
	u16 *p = (u16 *)buf;

	for (i = 0; i < count; i += 2)
		*p++ = __nand_data16();
}
#elif (BUS_WIDTH == 8)
static void nand_read_buf(void *buf, size_t count)
{
	size_t i;
	u8 *p = (u8 *)buf;

	for (i = 0; i < count; i++)
		*p++ = __nand_data8();
}
#endif

static void nand_read_oob(uint32_t page_addr, uint8_t *buf, size_t size)
{
	int col_addr;

#if (PAGE_SIZE == 512)
	col_addr = 0;
	nand_wait_ready();

	/* Send READOOB command */
	__nand_cmd(NAND_CMD_READOOB);
#else
	col_addr = PAGE_SIZE;

	/* Send READ0 command */
	__nand_cmd(NAND_CMD_READ0);
#endif

	/* Send column address */
	__nand_addr(col_addr & 0xff);

#if (PAGE_SIZE != 512)
	__nand_addr((col_addr >> 8) & 0xff);
#endif

	/* Send page address */
	__nand_addr(page_addr & 0xff);
	__nand_addr((page_addr >> 8) & 0xff);

#if (ROW_CYCLE == 3)
	__nand_addr((page_addr >> 16) & 0xff);
#endif

#if (PAGE_SIZE != 512)
	/* Send READSTART command for 2048 or 4096 ps NAND */
	__nand_cmd(NAND_CMD_READSTART);
#endif

	/* Wait for device ready */
	nand_wait_ready();

	/* Read oob data */
	nand_read_buf(buf, size);

#if (PAGE_SIZE == 512)
	nand_wait_ready();
#endif
}

static void __nand_read_page(uint32_t page_addr, uint8_t *dst, uint8_t *oobbuf)
{
	unsigned int i;

	/* Read oob data */
	nand_read_oob(page_addr, oobbuf, OOB_SIZE);

	/* Send READ0 command */
	__nand_cmd(NAND_CMD_READ0);

	/* Send column address */
	__nand_addr(0);
#if (PAGE_SIZE != 512)
	__nand_addr(0);
#endif

	/* Send page address */
	__nand_addr(page_addr & 0xff);
	__nand_addr((page_addr >> 8) & 0xff);
#if (ROW_CYCLE == 3)
	__nand_addr((page_addr >> 16) & 0xff);
#endif

	/* Send READSTART command for 2048 or 4096 ps NAND */
#if (PAGE_SIZE != 512)
	__nand_cmd(NAND_CMD_READSTART);
#endif

	/* Wait for device ready */
	nand_wait_ready();

	for (i = 0; i < PAGE_SIZE / ECC_BLOCK; i++) {
		/* Read data */
		nand_read_buf(dst, ECC_BLOCK);

		/* Correct data */
		bch_correct_block(dst, oobbuf + ECC_POS + i * PAR_SIZE);

		dst += ECC_BLOCK;
	}
}

void nand_read_page(uint32_t page, uint8_t *dst)
{
	uint8_t oob_buf[OOB_SIZE];

	__nand_enable();
	__nand_read_page(page, dst, oob_buf);
	__nand_disable();
}

void nand_load(uint32_t page_start, size_t nb, uint8_t *dst)
{
	uint8_t oob_buf[OOB_SIZE];

	__nand_enable();
	while (nb--) {
		__nand_read_page(page_start++, dst, oob_buf);
		dst += PAGE_SIZE;
	}
	__nand_disable();
}
