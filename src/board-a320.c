/*
 * board.c
 *
 * Board init routines.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 */

#include <stdint.h>

#include "config.h"

#include "board.h"
#include "serial.h"
#include "utils.h"
#include "jz.h"

#define PIN_X (32*3 + 19)		/* Port 3 pin 19: X button */
#define PIN_Y (32*3 + 2)		/* Port 3 pin 2:  Y button */
#define PIN_A (32*3 + 0)		/* Port 3 pin 0: A button */
#define PIN_BKLIGHT	(32*3+31)	/* Port 3 pin 31: Backlight PWM  */

#define CDIV 1
#define HDIV 3
#define PDIV 3
#define MDIV 3
#define LDIV 3

/* PLL output clock = EXTAL * NF / (NR * NO)
 *
 * NF = FD + 2, NR = RD + 2
 * NO = 1 (if OD = 0), NO = 2 (if OD = 1 or 2), NO = 4 (if OD = 3)
 */
static void pll_init(void)
{
	register unsigned int cfcr, plcr1, pllout2;
	static const uint8_t n2FR[33] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0,
		7, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
		9
	};

	cfcr = CPM_CPCCR_CLKOEN |
		CPM_CPCCR_PCS |
		((unsigned int) n2FR[CDIV] << CPM_CPCCR_CDIV_BIT) |
		((unsigned int) n2FR[HDIV] << CPM_CPCCR_HDIV_BIT) |
		((unsigned int) n2FR[PDIV] << CPM_CPCCR_PDIV_BIT) |
		((unsigned int) n2FR[MDIV] << CPM_CPCCR_MDIV_BIT) |
		((unsigned int) n2FR[LDIV] << CPM_CPCCR_LDIV_BIT);

	pllout2 = (cfcr & CPM_CPCCR_PCS) ? CFG_CPU_SPEED : (CFG_CPU_SPEED / 2);

	/* Init USB Host clock, pllout2 must be n*48MHz */
	REG_CPM_UHCCDR = pllout2 / 48000000 - 1;

#define NF (CFG_CPU_SPEED * 2 / CFG_EXTAL)
	plcr1 = ((NF - 2) << CPM_CPPCR_PLLM_BIT) | /* FD */
		(0 << CPM_CPPCR_PLLN_BIT) |	/* RD=0, NR=2 */
		(0 << CPM_CPPCR_PLLOD_BIT) |    /* OD=0, NO=1 */
		(0x20 << CPM_CPPCR_PLLST_BIT) | /* PLL stable time */
		CPM_CPPCR_PLLEN;                /* enable PLL */          

	/* init PLL */
	REG_CPM_CPCCR = cfcr;
	REG_CPM_CPPCR = plcr1;

	__cpm_enable_pll_change();
}

/*
 * Failsafe SDRAM configuration values
 *
 * If you want to live on the edge, the Dingoo Hynix HY57V281620FTP-6
 * chips should work with these accoring to the datasheet:
 *
 *   TRAS 42
 *   RCD  18
 *   TPC  18
 *
 */
#define SDRAM_CASL		3		/* CAS latency: 2 or 3 */
#define SDRAM_TRAS		42		/* RAS# Active Time (ns) */
#define SDRAM_RCD		18		/* RAS# to CAS# Delay (ns) */
#define SDRAM_TPC		18		/* RAS# Precharge Time (ns) */
#define SDRAM_TRWL		7		/* Write Latency Time (ns) */
#define SDRAM_TREF		15625	/* Refresh period (ns): 4096 refresh cycles/64ms */
#define SDRAM_BW16		0
#define SDRAM_BANK40	0
#define SDRAM_BANK4		1
#define SDRAM_ROW0		11
#define SDRAM_ROW		12
#define SDRAM_COL0		8
#define SDRAM_COL		9

static void sdram_init(void)
{
	unsigned int dmcr0, dmcr, sdmode, tmp;

	static const unsigned int cas_latency_sdmr[2] = {
		EMC_SDMR_CAS_2,
		EMC_SDMR_CAS_3,
	};

	static const unsigned int cas_latency_dmcr[2] = {
		1 << EMC_DMCR_TCL_BIT,	/* CAS latency is 2 */
		2 << EMC_DMCR_TCL_BIT	/* CAS latency is 3 */
	};

	REG_EMC_BCR = 0;	/* Disable bus release */
	REG_EMC_RTCSR = 0;	/* Disable clock for counting */

	/* Fault DMCR value for mode register setting*/
	dmcr0 = ((SDRAM_ROW0-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL0-8)<<EMC_DMCR_CA_BIT) |
		(SDRAM_BANK40<<EMC_DMCR_BA_BIT) |
		(SDRAM_BW16<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* Basic DMCR value */
	dmcr = ((SDRAM_ROW-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL-8)<<EMC_DMCR_CA_BIT) |
		(SDRAM_BANK4<<EMC_DMCR_BA_BIT) |
		(SDRAM_BW16<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* SDRAM timimg */
#define NS (1000000000 / (CFG_CPU_SPEED * CDIV / MDIV))
	tmp = SDRAM_TRAS/NS;
	if (tmp < 4) tmp = 4;
	if (tmp > 11) tmp = 11;
	dmcr |= ((tmp-4) << EMC_DMCR_TRAS_BIT);
	tmp = SDRAM_RCD/NS;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << EMC_DMCR_RCD_BIT);
	tmp = SDRAM_TPC/NS;
	if (tmp > 7) tmp = 7;
	dmcr |= (tmp << EMC_DMCR_TPC_BIT);
	tmp = SDRAM_TRWL/NS;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << EMC_DMCR_TRWL_BIT);
	tmp = (SDRAM_TRAS + SDRAM_TPC)/NS;
	if (tmp > 14) tmp = 14;
	dmcr |= (((tmp + 1) >> 1) << EMC_DMCR_TRC_BIT);

	/* SDRAM mode value */
	sdmode = EMC_SDMR_BT_SEQ | 
		 EMC_SDMR_OM_NORMAL |
		 EMC_SDMR_BL_4 | 
		 cas_latency_sdmr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* Stage 1. Precharge all banks by writing SDMR with DMCR.MRSET=0 */
	REG_EMC_DMCR = dmcr;
	REG8(EMC_SDMR0|sdmode) = 0;

	/* Wait for precharge, > 200us */
	udelay(1000);

	/* Stage 2. Enable auto-refresh */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH;

	tmp = SDRAM_TREF/NS;
	tmp = tmp/64 + 1;
	if (tmp > 0xff) tmp = 0xff;
	REG_EMC_RTCOR = tmp;
	REG_EMC_RTCNT = 0;
	REG_EMC_RTCSR = EMC_RTCSR_CKS_64;	/* Divisor is 64, CKO/64 */

	/* Wait for number of auto-refresh cycles */
	udelay(1000);

 	/* Stage 3. Mode Register Set */
	REG_EMC_DMCR = dmcr0 | EMC_DMCR_RFSH | EMC_DMCR_MRSET;
	REG8(EMC_SDMR0|sdmode) = 0;

        /* Set back to basic DMCR value */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* everything is ok now */
}

int alt_key_pressed(void)
{
	return !__gpio_get_pin(PIN_X);
}

int alt2_key_pressed(void)
{
	return !__gpio_get_pin(PIN_Y);
}

int alt3_key_pressed(void)
{
	return !__gpio_get_pin(PIN_A);
}

#ifdef BKLIGHT_ON
void light(int set)
{
	if (set)
		__gpio_set_pin(PIN_BKLIGHT);
	else
		__gpio_clear_pin(PIN_BKLIGHT);
}
#endif

unsigned int get_memory_size(void)
{
	return 1 << (SDRAM_ROW + SDRAM_COL + (2 - SDRAM_BW16) +
				(2 - SDRAM_BANK4) + 1);
}

void board_init(void)
{
#ifdef USE_NAND
	__gpio_as_nand();
#endif
	__gpio_as_sdram_32bit();
	__gpio_as_msc();
#ifdef USE_SERIAL
	__gpio_as_uart0();
	serial_init();
#endif

	pll_init();
	sdram_init();

#ifdef BKLIGHT_ON
	__gpio_clear_pin(PIN_BKLIGHT);
	__gpio_as_output(PIN_BKLIGHT);
#endif

	__gpio_as_input(PIN_X);
	__gpio_as_input(PIN_Y);
}
