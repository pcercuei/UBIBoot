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

#define BCH_BASE 0xB30D0000

#define BCH_BHCR			(BCH_BASE + 0x0)
#define BCH_BHCSR			(BCH_BASE + 0x4)
#define BCH_BHCCR			(BCH_BASE + 0x8)
#define BCH_BHCNT			(BCH_BASE + 0xc)
#define BCH_BHDR			(BCH_BASE + 0x10)
#define BCH_BHPAR0			(BCH_BASE + 0x14)
#define BCH_BHERR0			(BCH_BASE + 0x28)
#define BCH_BHINT			(BCH_BASE + 0x24)
#define BCH_BHINTES			(BCH_BASE + 0x3c)
#define BCH_BHINTEC			(BCH_BASE + 0x40)
#define BCH_BHINTE			(BCH_BASE + 0x38)

#define BCH_BHCR_BSEL_SHIFT		2
#define BCH_BHCR_BSEL_MASK		(0x1 << BCH_BHCR_BSEL_SHIFT)
#define BCH_BHCR_ENCE			BIT(3)
#define BCH_BHCR_INIT			BIT(1)
#define BCH_BHCR_BCHE			BIT(0)

#define BCH_BHCNT_DEC_COUNT_SHIFT	16
#define BCH_BHCNT_DEC_COUNT_MASK	(0x3ff << BCH_BHCNT_DEC_COUNT_SHIFT)
#define BCH_BHCNT_ENC_COUNT_SHIFT	0
#define BCH_BHCNT_ENC_COUNT_MASK	(0x3ff << BCH_BHCNT_ENC_COUNT_SHIFT)

#define BCH_BHERR_INDEX0_SHIFT		0
#define BCH_BHERR_INDEX0_MASK		(0x1fff << BCH_BHERR_INDEX0_SHIFT)
#define BCH_BHERR_INDEX1_SHIFT		16
#define BCH_BHERR_INDEX1_MASK		(0x1fff << BCH_BHERR_INDEX1_SHIFT)

#define BCH_BHINT_ERRC_SHIFT		28
#define BCH_BHINT_ERRC_MASK		(0xf << BCH_BHINT_ERRC_SHIFT)
#define BCH_BHINT_TERRC_SHIFT		16
#define BCH_BHINT_TERRC_MASK		(0x7f << BCH_BHINT_TERRC_SHIFT)
#define BCH_BHINT_ALL_0			BIT(5)
#define BCH_BHINT_ALL_F			BIT(4)
#define BCH_BHINT_DECF			BIT(3)
#define BCH_BHINT_ENCF			BIT(2)
#define BCH_BHINT_UNCOR			BIT(1)
#define BCH_BHINT_ERR			BIT(0)

/* Timeout for BCH calculation/correction. */
#define BCH_TIMEOUT_US			100000

static void jz4725b_bch_init(void)
{
	u32 reg;

	/* Clear interrupt status. */
	reg = REG32(BCH_BHINT);
	REG32(BCH_BHINT) = reg;

	/* Initialise and enable BCH. */
	REG32(BCH_BHCCR) = 0x1f;
	REG32(BCH_BHCSR) = BCH_BHCR_BCHE;

	/* Set 8-bit strength */
	REG32(BCH_BHCSR) = BCH_BHCR_BSEL_MASK;

	/* Set BCH decode mode */
	REG32(BCH_BHCCR) = BCH_BHCR_ENCE;

	REG32(BCH_BHCSR) = BCH_BHCR_INIT;

	/* Set up BCH count register. */
	reg = ECC_BLOCK << BCH_BHCNT_ENC_COUNT_SHIFT;
	reg |= (ECC_BLOCK + PAR_SIZE) << BCH_BHCNT_DEC_COUNT_SHIFT;
	REG32(BCH_BHCNT) = reg;
}

static void jz4725b_bch_write_data(const uint8_t *buf, unsigned int size)
{
	while (size--)
		REG8(BCH_BHDR) = *buf++;
}

int bch_correct_block(uint8_t *buf, uint8_t *ecc_code)
{
	u32 reg, errors, bit;
	unsigned int i;
	int ret = 0;

	jz4725b_bch_init();
	jz4725b_bch_write_data(buf, ECC_BLOCK);
	jz4725b_bch_write_data(ecc_code, PAR_SIZE);

	do {
		reg = REG32(BCH_BHINT);
	} while (!(reg & BCH_BHINT_DECF));

	REG32(BCH_BHINT) = reg;

	if (reg & (BCH_BHINT_ALL_F | BCH_BHINT_ALL_0)) {
		/* Data and ECC is all 0xff or 0x00 - nothing to correct */
		ret = 0;
		goto out;
	}

	if (reg & BCH_BHINT_UNCOR) {
		/* Uncorrectable ECC error */
		ret = -ERR_NAND_IO_UNC;
		SERIAL_ERR(ERR_NAND_IO_UNC);
		goto out;
	}

	errors = (reg & BCH_BHINT_ERRC_MASK) >> BCH_BHINT_ERRC_SHIFT;

	/* Correct any detected errors. */
	for (i = 0; i < errors; i++) {
		if (i & 1) {
			bit = (reg & BCH_BHERR_INDEX1_MASK) >> BCH_BHERR_INDEX1_SHIFT;
		} else {
			reg = REG32(BCH_BHERR0 + (i * 4));
			bit = (reg & BCH_BHERR_INDEX0_MASK) >> BCH_BHERR_INDEX0_SHIFT;
		}

		buf[(bit >> 3)] ^= BIT(bit & 0x7);
	}

out:
	reg = REG32(BCH_BHINT);
	REG32(BCH_BHINT) = reg;
	REG32(BCH_BHCCR) = BCH_BHCR_BCHE;
	return ret;
}
