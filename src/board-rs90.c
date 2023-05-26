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
	while (!__cpm_pll_is_on());
}

#define SDRAM_SPEED (CFG_CPU_SPEED * CDIV / MDIV)
#define SDRAM_CLK_PERIOD_PS (1000000000000ULL / SDRAM_SPEED)
static const jz4740_sdram_config_t sdram_cfg = {
	.ckoFrequency = SDRAM_SPEED,
	.refreshPeriod = SDRAM_REFRESH,
	.CL    = SDRAM_CAS_LATENCY,
	.tRAS  = DIV_ROUND_UP(SDRAM_TRAS * 1000, SDRAM_CLK_PERIOD_PS),
	.tRCD  = DIV_ROUND_UP(SDRAM_TRCD * 1000, SDRAM_CLK_PERIOD_PS),
	.tRP   = DIV_ROUND_UP(SDRAM_TRP * 1000, SDRAM_CLK_PERIOD_PS),
	.tWR   = SDRAM_TWR,
	.nRows = SDRAM_ROW,
	.nCols = SDRAM_COL,
	.nBanks = SDRAM_BANKS,
	.busWidth = SDRAM_BUS_WIDTH,
};

void sdram_init(void)
{
	jz4740_sdram_init(&sdram_cfg);
}

unsigned int get_memory_size(void)
{
	return jz4740_get_sdram_capacity(&sdram_cfg);
}

int alt_key_pressed(void)
{
	return !__gpio_get_pin(GPIOD, 17); /* START button */
}

int alt2_key_pressed(void)
{
	return !__gpio_get_pin(GPIOC, 12); /* Right shoulder button */
}

int alt3_key_pressed(void)
{
	return !__gpio_get_pin(GPIOD, 24); /* A button on the RG99 */
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

void board_init(void)
{
#ifdef USE_NAND
	__gpio_as_func_mask(GPIOC, 0x36300300, 0);
	__gpio_as_input(GPIOC, 27);
	__gpio_disable_pull(GPIOC, 27);
#endif

	/* SDRAM pins */
	__gpio_as_func_mask(0, 0xffff, 0);
	__gpio_as_func_mask(1, 0x033fffff, 0);

	/* MSC1 pins */
	__gpio_as_func_mask(3, 0xfc000000, 0);

#ifdef USE_SERIAL
	__gpio_as_func(2, 12, 1); /* UART_TX */

	/* Start UART clock */
	REG_CPM_CLKGR &= ~(1 << 0);

	serial_init();
#endif

	pll_init();
	SERIAL_PUTS_ARGI("PLL running at ", __cpm_get_pllout() / 1000000, " MHz.\n");

	sdram_init();
	SERIAL_PUTS_ARGI("SDRAM running at ", __cpm_get_mclk() / 1000000, " MHz.\n");
	SERIAL_PUTS_ARGI("SDRAM size is ", get_memory_size() / 1048576, " MiB.\n");

	/* Ungate MSC1 clock */
	REG_CPM_CLKGR &= ~(1 << 16);

	/* Set divider for the MSC clock */
	__cpm_set_mscdiv((__cpm_get_pllout2() / 24000000) - 1);

#ifdef BKLIGHT_ON
	__gpio_clear_pin(GPIOD, 31);
	__gpio_as_output(GPIOD, 31);
#endif
}

#ifdef USE_NAND
void nand_wait_ready(void)
{
	unsigned int timeout = 10000;

	while (__gpio_get_pin(GPIOC, 27) && timeout--);
	while (!__gpio_get_pin(GPIOC, 27));
}

void nand_init(void)
{
	REG32(EMC_SMCR1) = (EMC_TAS << EMC_SMCR_TAS_BIT) |
			   (EMC_TAH << EMC_SMCR_TAH_BIT) |
			   (EMC_TBP << EMC_SMCR_TBP_BIT) |
			   (EMC_TAW << EMC_SMCR_TAW_BIT) |
			   (EMC_STRV << EMC_SMCR_STRV_BIT);
}
#endif
