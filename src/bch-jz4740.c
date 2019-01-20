/*
 * Copyright (C) 2019 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "bch.h"
#include "config.h"
#include "errorcodes.h"
#include "jz4740-emc.h"
#include "jz.h"
#include "serial.h"

#define __nand_ecc_rs_decoding() \
	(REG_EMC_NFECR = EMC_NFECR_ECCE | EMC_NFECR_ERST | EMC_NFECR_RS | EMC_NFECR_RS_DECODING)
#define __nand_ecc_decode_sync() while (!(REG_EMC_NFINTS & EMC_NFINTS_DECF))
#define __nand_ecc_disable()	(REG_EMC_NFECR &= ~EMC_NFECR_ECCE)

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

int bch_correct_block(uint8_t *dst, uint8_t *oobbuf)
{
	volatile unsigned char *paraddr = (volatile unsigned char *)EMC_NFPAR0;
	unsigned int i, stat;

	/* Enable RS decoding */
	REG_EMC_NFINTS = 0x0;
	__nand_ecc_rs_decoding();

	/* Set PAR values */
	for (i = 0; i < PAR_SIZE; i++)
		*paraddr++ = oobbuf[i];

	/* Set PRDY */
	REG_EMC_NFECR |= EMC_NFECR_PRDY;

	/* Wait for completion */
	__nand_ecc_decode_sync();

	/* Disable decoding */
	__nand_ecc_disable();

	/* Check result of decoding */
	stat = REG_EMC_NFINTS;
	if (stat & EMC_NFINTS_ERR) {
		unsigned int errcnt, index, mask;

		if (stat & EMC_NFINTS_UNCOR) {
			/* XXX: Why is that appearing all the time, and the
			 * kernel still boots fine? */

			SERIAL_ERR(ERR_NAND_IO_UNC);
			return 0;
		}

		SERIAL_ERR(ERR_NAND_IO);

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

	return 0;
}
