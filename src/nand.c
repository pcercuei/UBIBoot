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

#include "serial.h"
#include "nand.h"
#include "config.h"
#include "errorcodes.h"
#include "jz.h"

/*
 * NAND flash definitions
 */

#define NAND_DATAPORT	0xb8000000
#define NAND_ADDRPORT	0xb8010000
#define NAND_COMMPORT	0xb8008000

#define __nand_enable()		(REG_EMC_NFCSR |= EMC_NFCSR_NFE1 | EMC_NFCSR_NFCE1)
#define __nand_disable()	(REG_EMC_NFCSR &= ~(EMC_NFCSR_NFCE1))
#define __nand_ecc_rs_encoding() \
	(REG_EMC_NFECR = EMC_NFECR_ECCE | EMC_NFECR_ERST | EMC_NFECR_RS | EMC_NFECR_RS_ENCODING)
#define __nand_ecc_rs_decoding() \
	(REG_EMC_NFECR = EMC_NFECR_ECCE | EMC_NFECR_ERST | EMC_NFECR_RS | EMC_NFECR_RS_DECODING)
#define __nand_ecc_disable()	(REG_EMC_NFECR &= ~EMC_NFECR_ECCE)
#define __nand_ecc_encode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_ENCF))
#define __nand_ecc_decode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_DECF))

static inline void __nand_dev_ready(void)
{
	unsigned int timeout = 10000;
	while ((REG_GPIO_PXPIN(2) & 0x40000000) && timeout--);
	while (!(REG_GPIO_PXPIN(2) & 0x40000000));
}

#define __nand_cmd(n)		(REG8(NAND_COMMPORT) = (n))
#define __nand_addr(n)		(REG8(NAND_ADDRPORT) = (n))
#define __nand_data8()		REG8(NAND_DATAPORT)
#define __nand_data16()		REG16(NAND_DATAPORT)

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

/* Correct 1~9-bit errors in 512-bytes data */
static void rs_correct(unsigned char *dat, int idx, int mask)
{
	int i;

	idx--;

	i = idx + (idx >> 3);
	if (i >= 512)
		return;

	mask <<= (idx & 0x7);

	dat[i] ^= mask & 0xff;
	if (i < 511)
		dat[i+1] ^= (mask >> 8) & 0xff;
}

static void nand_read_oob(uint32_t page_addr, uint8_t *buf, size_t size)
{
	int col_addr;

#if (PAGE_SIZE == 512)
	col_addr = 0;
	__nand_dev_ready();

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
	__nand_dev_ready();

	/* Read oob data */
	nand_read_buf(buf, size);

#if (PAGE_SIZE == 512)
	__nand_dev_ready();
#endif
}

static void __nand_read_page(uint32_t page_addr, uint8_t *dst, uint8_t *oobbuf)
{
	size_t i, j;

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
	__nand_dev_ready();

	for (i = 0; i < PAGE_SIZE / ECC_BLOCK; i++) {
		volatile unsigned char *paraddr = (volatile unsigned char *)EMC_NFPAR0;
		unsigned int stat;

		/* Enable RS decoding */
		REG_EMC_NFINTS = 0x0;
		__nand_ecc_rs_decoding();

		/* Read data */
		nand_read_buf((void *)dst, ECC_BLOCK);

		/* Set PAR values */
		for (j = 0; j < PAR_SIZE; j++) {
#if defined(CFG_NAND_ECC_POS)
			*paraddr++ = oobbuf[CFG_NAND_ECC_POS + i*PAR_SIZE2 + j];
#else
			*paraddr++ = oobbuf[ECC_POS + i*PAR_SIZE2 + j];
#endif
		}

		/* Set PRDY */
		REG_EMC_NFECR |= EMC_NFECR_PRDY;

		/* Wait for completion */
		__nand_ecc_decode_sync();

		/* Disable decoding */
		__nand_ecc_disable();

		/* Check result of decoding */
		stat = REG_EMC_NFINTS;
		if (stat & EMC_NFINTS_ERR) {
			if (stat & EMC_NFINTS_UNCOR) {
				/* XXX: Why is that appearing all the time, and the
				 * kernel still boots fine? */

				SERIAL_PUTI(ERR_NAND_IO_UNC);
			}
			else {
				unsigned int errcnt, index, mask;
				SERIAL_PUTI(ERR_NAND_IO);

				errcnt = (stat & EMC_NFINTS_ERRCNT_MASK) >> EMC_NFINTS_ERRCNT_BIT;
				switch (errcnt) {
				case 4:
					index = (REG_EMC_NFERR3 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR3 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(dst, index, mask);
					/* FALL-THROUGH */
				case 3:
					index = (REG_EMC_NFERR2 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR2 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(dst, index, mask);
					/* FALL-THROUGH */
				case 2:
					index = (REG_EMC_NFERR1 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR1 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(dst, index, mask);
					/* FALL-THROUGH */
				case 1:
					index = (REG_EMC_NFERR0 & EMC_NFERR_INDEX_MASK) >> EMC_NFERR_INDEX_BIT;
					mask = (REG_EMC_NFERR0 & EMC_NFERR_MASK_MASK) >> EMC_NFERR_MASK_BIT;
					rs_correct(dst, index, mask);
					break;
				default:
					break;
				}
			}
		}

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

void nand_init(void)
{

 	/* Optimize the timing of nand */
	REG_EMC_SMCR1 = 0x094c4400;
}

