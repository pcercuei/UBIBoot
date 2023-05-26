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
#include "sdram.h"

#include "board.h"
#include "serial.h"
#include "utils.h"
#include "jz.h"

#include "jz4740-cpm.h"
#include "jz4740-emc.h"
#include "jz4740-gpio.h"

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

#define SDRAM_BANK40  0
#define SDRAM_ROW0   11
#define SDRAM_COL0    8

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
		((SDRAM_BUS_WIDTH == 16 ? 1 : 0)<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CAS_LATENCY == 3) ? 1 : 0)];

	/* Basic DMCR value */
	dmcr = ((SDRAM_ROW-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL-8)<<EMC_DMCR_CA_BIT) |
		((SDRAM_BANKS == 4 ? 1 : 0)<<EMC_DMCR_BA_BIT) |
		((SDRAM_BUS_WIDTH == 16 ? 1 : 0)<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CAS_LATENCY == 3) ? 1 : 0)];

	/* SDRAM timimg */
#define NS (1000000000 / (CFG_CPU_SPEED * CDIV / MDIV))
	tmp = SDRAM_TRAS/NS;
	if (tmp < 4) tmp = 4;
	if (tmp > 11) tmp = 11;
	dmcr |= ((tmp-4) << EMC_DMCR_TRAS_BIT);
	tmp = SDRAM_TRCD/NS;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << EMC_DMCR_RCD_BIT);
	tmp = SDRAM_TRP/NS;
	if (tmp > 7) tmp = 7;
	dmcr |= (tmp << EMC_DMCR_TPC_BIT);
	tmp = SDRAM_TWR - 1;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << EMC_DMCR_TRWL_BIT);
	tmp = (SDRAM_TRAS + SDRAM_TRP)/NS;
	if (tmp > 14) tmp = 14;
	dmcr |= (((tmp + 1) >> 1) << EMC_DMCR_TRC_BIT);

	/* SDRAM mode value */
	sdmode = EMC_SDMR_BT_SEQ | 
		 EMC_SDMR_OM_NORMAL |
		 EMC_SDMR_BL_4 | 
		 cas_latency_sdmr[((SDRAM_CAS_LATENCY == 3) ? 1 : 0)];

	/* Stage 1. Precharge all banks by writing SDMR with DMCR.MRSET=0 */
	REG_EMC_DMCR = dmcr;
	REG8(EMC_SDMR0|sdmode) = 0;

	/* Wait for precharge, > 200us */
	udelay(1000);

	/* Stage 2. Enable auto-refresh */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH;

	tmp = SDRAM_REFRESH / (1 << SDRAM_ROW) / NS;
	tmp = tmp / 64 + 1;
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
	return !__gpio_get_pin(GPIOC, 19);	/* Port 3 pin 19: X button */
}

int alt2_key_pressed(void)
{
	return !__gpio_get_pin(GPIOC, 2);	/* Port 3 pin 2:  Y button */
}

int alt3_key_pressed(void)
{
	return !__gpio_get_pin(GPIOC, 0);	/* Port 3 pin 0: A button */
}

#ifdef BKLIGHT_ON
void light(int set)
{
	if (set)
		__gpio_set_pin(GPIOD, 31);
	else
		__gpio_clear_pin(GPIOD, 31);
}
#endif

unsigned int get_memory_size(void)
{
	return (SDRAM_BUS_WIDTH / 8) * (SDRAM_BANKS) *
		(1 << (SDRAM_ROW + SDRAM_COL));
}

void board_init(void)
{
#ifdef USE_NAND
	/* NAND pins */
	__gpio_as_func_mask(GPIOB, 0x02418000, 0);
	__gpio_as_func_mask(GPIOC, 0x30000000, 0);
	__gpio_as_input(GPIOC, 30);

	__gpio_disable_pull_mask(GPIOB, 0x02018000);
	__gpio_disable_pull_mask(GPIOC, 0x70000000);
#endif

	/* SDRAM pins */
	__gpio_as_func_mask(GPIOA, 0xffffffff, 0);
	__gpio_as_func_mask(GPIOB, 0x81f9ffff, 0);
	__gpio_as_func_mask(GPIOC, 0x07000000, 0);

	__gpio_disable_pull_mask(GPIOA, 0xffffffff);
	__gpio_disable_pull_mask(GPIOB, 0x81f9ffff);
	__gpio_disable_pull_mask(GPIOC, 0x07000000);

	/* MMC pins */
	__gpio_as_func_mask(GPIOD, 0x00003f00, 0);
	__gpio_disable_pull_mask(GPIOD, 0x00003f00);

#ifdef USE_SERIAL
	__gpio_as_func_mask(GPIOD, 0x06000000, 1);
	serial_init();
#endif

	pll_init();
	SERIAL_PUTS_ARGI("PLL running at ", __cpm_get_pllout() / 1000000, " MHz.\n");

	sdram_init();
	SERIAL_PUTS_ARGI("SDRAM running at ", __cpm_get_mclk() / 1000000, " MHz.\n");
	SERIAL_PUTS_ARGI("SDRAM size is ", get_memory_size() / 1048576, " MiB.\n");

#ifdef BKLIGHT_ON
	__gpio_clear_pin(GPIOD, 31);	/* D31: Backlight PWM  */
	__gpio_as_output(GPIOD, 31);
#endif

	/* X/A/Y buttons */
	__gpio_as_input_mask(GPIOC, 0x00080005);
}

#ifdef USE_NAND
void nand_wait_ready(void)
{
	unsigned int timeout = 10000;

	while (__gpio_get_pin(GPIOC, 30) && timeout--);
	while (!__gpio_get_pin(GPIOC, 30));
}

void nand_init(void)
{
	/* Optimize the timing of nand */
	REG_EMC_SMCR1 = 0x094c4400;
}
#endif
