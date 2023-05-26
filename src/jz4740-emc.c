/*
 * jz4740_sdram.c
 *
 * Common SDRAM init routine for jz4740/25b/55
 *
 * Author: Siarhei Volkau <lis8215@gmail.com>
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "jz.h"
#include "utils.h"

#include "jz4740-emc.h"

static const unsigned int cas_latency_sdmr[2] = {
	EMC_SDMR_CAS_2,
	EMC_SDMR_CAS_3,
};

static const uint8_t rtcsr_cko_div[] = {
	2, 4, 6, 8, 10, 11, 12, /* CKO /4 /16 /64 /256 /1024 /2048 /4096 */
};

void jz4740_sdram_init(const jz4740_sdram_config_t *cfg)
{
	uint32_t mrset_dmcr, dmcr = 0, sdmode, rtcor, rtcsr;
	uint32_t row_refresh_period_ns, refresh_freq;
	uint32_t trcd, trc, twp, tras, trp;

	REG_EMC_BCR = 0;	/* Disable bus release */
	REG_EMC_RTCSR = 0;	/* Disable clock for counting */

	dmcr |= (cfg->busWidth == 16 ? 1 : 0) << EMC_DMCR_BW_BIT;
	dmcr |= (cfg->CL - 1) << EMC_DMCR_TCL_BIT;
	dmcr |= EMC_DMCR_EPIN;
	mrset_dmcr = dmcr;

	/* SDRAM timings */

	dmcr |= (cfg->nBanks == 4 ? 1 : 0) << EMC_DMCR_BA_BIT;

	dmcr |= (cfg->nCols - 8) << EMC_DMCR_CA_BIT;
	dmcr |= (cfg->nRows - 11) << EMC_DMCR_RA_BIT;

	/* tRAS: 4-11 clock cycles */
	tras = cfg->tRAS;
	if (tras < 4) tras = 4;
	if (tras > 11) tras = 11;
	dmcr |= (tras - 4) << EMC_DMCR_TRAS_BIT;

	/* tRCD: 1-4 clock cycles */
	trcd = cfg->tRCD;
	if (trcd < 1) trcd = 1;
	if (trcd > 4) trcd = 4;
	dmcr |= (trcd - 1) << EMC_DMCR_RCD_BIT;

	/* tRP: 1-8 clock cycles */
	trp = cfg->tRP;
	if (trp < 1) trp = 1;
	if (trp > 8) trp = 8;
	dmcr |= (trp - 1) << EMC_DMCR_TPC_BIT;

	/* tWP: 1-4 clock cycles */
	twp = cfg->tWR;
	if (twp < 1) twp = 1;
	if (twp > 4) twp = 4;
	dmcr |= (twp - 1) << EMC_DMCR_TRWL_BIT;

	trc = (tras + trp) | 1; /* odd */
	dmcr |= (trc >> 1) << EMC_DMCR_TRC_BIT;

	row_refresh_period_ns = cfg->refreshPeriod / (1 << cfg->nRows);
	refresh_freq = 1000000000 / row_refresh_period_ns;

	rtcor = cfg->ckoFrequency / refresh_freq;
	for (rtcsr = 1; rtcsr <= 7; rtcsr++)
		if ((rtcor >> rtcsr_cko_div[rtcsr - 1]) <= 0xff)
			break;
	rtcor >>= rtcsr_cko_div[rtcsr - 1];
	if (rtcor > 0xff)
		rtcor = 0xff;

	/* SDRAM mode value */
	sdmode = EMC_SDMR_BT_SEQ |
		 EMC_SDMR_OM_NORMAL |
		 EMC_SDMR_BL_4 |
		 cas_latency_sdmr[cfg->CL - 2];

	/* Stage 1. Precharge all banks by writing SDMR with DMCR.MRSET=0 */
	REG_EMC_DMCR = dmcr;
	REG8(EMC_SDMR0|sdmode) = 0;

	/* Wait for precharge, > 200us */
	udelay(1000);

	/* Stage 2. Enable auto-refresh */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH;

	REG_EMC_RTCOR = rtcor;
	REG_EMC_RTCNT = 0;
	REG_EMC_RTCSR = rtcsr;

	/* Wait for number of auto-refresh cycles */
	udelay(1000);

	/* Stage 3. Mode Register Set */
	REG_EMC_DMCR = mrset_dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;
	REG8(EMC_SDMR0|sdmode) = 0;

	/* Set back to basic DMCR value */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* everything is ok now */
}
