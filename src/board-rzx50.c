/*
 * board.c
 *
 * Board init routines.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 * Maintainer Siarhei Volkau <lis8215@gmail.com>
 *
 */

#include <stdint.h>

#include "config.h"

/* HACK actual freq is 24M but for peripheral it divised by 2 in pll_init
 * this allows to use 12M for rest of the source code e.g. serial.c
 */
#undef  CFG_EXTAL
#define CFG_EXTAL 24000000

#include "board.h"
#include "serial.h"
#include "utils.h"

#include "jz.h"
#include "jz4740-cpm.h"
#include "jz4740-emc.h"
#include "jz4740-gpio.h"
#include "mmc.h"

#define CDIV 1
#define H0DIV 3
#define PDIV 3
#define MDIV 3
#define H1DIV 2

#define NS (1000000000 / (CFG_CPU_SPEED * CDIV / MDIV))

/* PLL output clock = EXTAL * NF / (NR * NO)
 *
 * NF = FD + 2, NR = RD + 2
 * NO = 1 (if OD = 0), NO = 2 (if OD = 1 or 2), NO = 4 (if OD = 3)
 */
static void pll_init(void)
{
	register unsigned int cfcr, plcr1;
	static const uint8_t n2FR[9] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5
	};

	cfcr = CPM_CPCCR_CLKOEN | CPM_CPCCR_PCS |
		((unsigned int) n2FR[CDIV] << CPM_CPCCR_CDIV_BIT) |
		((unsigned int) n2FR[H0DIV] << CPM_CPCCR_HDIV_BIT) |
		((unsigned int) n2FR[PDIV] << CPM_CPCCR_PDIV_BIT) |
		((unsigned int) n2FR[MDIV] << CPM_CPCCR_MDIV_BIT) |
		((unsigned int) n2FR[H1DIV] << CPM_CPCCR_LDIV_BIT);

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

/*
 * Those settings are matched settings from
 * original U-Boot.
 */
#define SDRAM_CASL      2       /* CAS latency: 2 or 3 */
#define SDRAM_TRAS      42      /* RAS# Active Time (ns) */
#define SDRAM_RCD       18      /* RAS# to CAS# Delay (ns) */
#define SDRAM_TPC       18      /* RAS# Precharge Time (ns) */
#define SDRAM_TREF      15625   /* Refresh period (ns) */
#define SDRAM_TRWL      7       /* Write Latency Time (ns) */
#define SDRAM_BW16      0
#define SDRAM_BANK40    0
#define SDRAM_BANK4     1
#define SDRAM_ROW0      11
#define SDRAM_ROW       13
#define SDRAM_COL0      9
#define SDRAM_COL       9
#define NR_DRAM_BANKS   1
//#define MOBILE_SDRAM

/* JZ4750/4755 specific */
#define EMC_MEM_PHY_BASE        0x20000000
#define EMC_MEM_PHY_BASE_SHIFT  24

static inline unsigned int get_ram_size_per_bank(void)
{
	u32 dmcr;
	u32 rows, cols, dw, banks;
	ulong size;

	dmcr = REG_EMC_DMCR;
	rows = 11 + ((dmcr & EMC_DMCR_RA_MASK) >> EMC_DMCR_RA_BIT);
	cols = 8 + ((dmcr & EMC_DMCR_CA_MASK) >> EMC_DMCR_CA_BIT);
	dw = (dmcr & EMC_DMCR_BW) ? 2 : 4;
	banks = (dmcr & EMC_DMCR_BA) ? 4 : 2;

	size = (1 << (rows + cols)) * dw * banks;

	return size;
}

void sdram_init(void)
{
	unsigned int dmcr, sdmode, tmp;

#ifdef MOBILE_SDRAM
	unsigned int sdemode; /*SDRAM Extended Mode*/
#endif
	unsigned int cas_latency_sdmr[2] = {
		EMC_SDMR_CAS_2,
		EMC_SDMR_CAS_3,
	};

	unsigned int cas_latency_dmcr[2] = {
		1 << EMC_DMCR_TCL_BIT,	/* CAS latency is 2 */
		2 << EMC_DMCR_TCL_BIT	/* CAS latency is 3 */
	};

	REG_EMC_BCR = 0;	/* Disable bus release */
	REG_EMC_RTCSR = 0;	/* Disable clock for counting */

	/* Basic DMCR value */
	dmcr = ((SDRAM_ROW-11)<<EMC_DMCR_RA_BIT) |
		((SDRAM_COL-8)<<EMC_DMCR_CA_BIT) |
		(SDRAM_BANK4<<EMC_DMCR_BA_BIT) |
		(SDRAM_BW16<<EMC_DMCR_BW_BIT) |
		EMC_DMCR_EPIN |
		cas_latency_dmcr[((SDRAM_CASL == 3) ? 1 : 0)];

	/* SDRAM timimg */
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

	/* Precharge Bank1 SDRAM */
#if NR_DRAM_BANKS == 2
	REG_EMC_DMCR = dmcr | EMC_DMCR_MBSEL;
	REG8(EMC_SDMR0|sdmode) = 0;
#endif

#ifdef MOBILE_SDRAM
	/* Mobile SDRAM Extended Mode Register */
	sdemode = EMC_SDMR_SET_BA1 | EMC_SDMR_DS_HALF | EMC_SDMR_PRSR_ALL;
#endif

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
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;
	REG8(EMC_SDMR0|sdmode) = 0;


#ifdef MOBILE_SDRAM
	REG8(EMC_SDMR0|sdemode) = 0;   	/* Set Mobile SDRAM Extended Mode Register */
#endif

#if NR_DRAM_BANKS == 2
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET | EMC_DMCR_MBSEL;
	REG8(EMC_SDMR0|sdmode) = 0;	/* Set Bank1 SDRAM Register */


#ifdef MOBILE_SDRAM
	REG8(EMC_SDMR0|sdemode) = 0;	/* Set Mobile SDRAM Extended Mode Register */
#endif

#endif   /*NR_DRAM_BANKS == 2*/

	/* Set back to basic DMCR value */
	REG_EMC_DMCR = dmcr | EMC_DMCR_RFSH | EMC_DMCR_MRSET;

	/* bank_size: 32M 64M 128M ... */
	unsigned int bank_size = get_ram_size_per_bank();
	unsigned int mem_base0, mem_base1, mem_mask;

	mem_base0 = EMC_MEM_PHY_BASE >> EMC_MEM_PHY_BASE_SHIFT;
	mem_base1 = ((EMC_MEM_PHY_BASE + bank_size) >> EMC_MEM_PHY_BASE_SHIFT);
	mem_mask = EMC_DMAR_MASK_MASK &
		(~(((bank_size) >> EMC_MEM_PHY_BASE_SHIFT)-1) & EMC_DMAR_MASK_MASK);

	REG_EMC_DMAR0 = (mem_base0 << EMC_DMAR_BASE_BIT) | mem_mask;
	REG_EMC_DMAR1 = (mem_base1 << EMC_DMAR_BASE_BIT) | mem_mask;

	/* everything is ok now */
}

int alt_key_pressed(void)
{
	return 0; /* TODO */
}

int alt2_key_pressed(void)
{
	return 0; /* TODO */
}

int alt3_key_pressed(void)
{
	return 0; /* TODO */
}

#ifdef BKLIGHT_ON
void light(int set)
{
	if (set)
		__gpio_clear_pin(GPIOE, 22);
	else
		__gpio_set_pin(GPIOE, 22);
}
#endif

unsigned int get_memory_size(void)
{
	return 1 << (SDRAM_ROW + SDRAM_COL + (2 - SDRAM_BW16) +
				(2 - SDRAM_BANK4) + NR_DRAM_BANKS);
}

void board_init(void)
{
	/* SDRAM pins */
	__gpio_as_func_mask(GPIOA, 0xffffffff, 0);
	__gpio_as_func_mask(GPIOB, 0x03ff7fff, 0);

	/* MSC0 pins */
	__gpio_as_func(GPIOB, 15, 2);
	__gpio_as_func(GPIOC, 27, 0);
	__gpio_as_func(GPIOC, 16, 2);
	__gpio_as_func_mask(GPIOC, 0x30020000, 1);

	/* MSC1 pins */
	__gpio_as_func_mask(GPIOB, 0xfc000000, 1);

#ifdef USE_SERIAL
	__gpio_as_func(GPIOE, 25, 1); /* UART_TX */

	/* Start UART clock */
	REG_CPM_CLKGR &= ~BIT(14);

	serial_init();
#endif
	pll_init();
	SERIAL_PUTS_ARGI("PLL running at ", __cpm_get_pllout() / 1000000, " MHz.\n");

	sdram_init();
	SERIAL_PUTS_ARGI("SDRAM running at ", __cpm_get_mclk() / 1000000, " MHz.\n");
	SERIAL_PUTS_ARGI("SDRAM size is ", get_memory_size() / 1048576, " MiB.\n");

#if 1
#ifdef BKLIGHT_ON
	__gpio_set_pin(GPIOE, 22);
	__gpio_as_output(GPIOE, 22);
#endif
#endif
	/* Ungate MSC0/1 clock */
	REG_CPM_CLKGR &= ~(BIT(6) | BIT(16));

	/* Set divider for the MSC0/1 clock */
	__cpm_set_mscdiv((__cpm_get_pllout2() / 24000000) - 1);
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

#define	USB_BASE	0xB3040000

#define REG_USB_FADDR	REG8(USB_BASE + 0x00) /* Function Address 8-bit */
#define REG_USB_POWER	REG8(USB_BASE + 0x01) /* Power Managemetn 8-bit */

void original_firmware_load(void)
{
	// internal MSC0 shall be initialized first
	// original u-boot is at 16kiB offset on SD card.
	// shall be loaded at 0x00100000 offset in RAM
	// loading up to 1MiB is sufficient

	const uint32_t load_addr = 0x00100000;
	const uint32_t entry_offset = 0x200;

	SERIAL_PUTS("Trying original RZX-50 firmware from internal SD.\n");

	if (!mmc_block_read(0, (void *) (KSEG1 + load_addr), 32, 2048-32)) {
		uint32_t cmd_1st = *(uint32_t*)(KSEG1 + load_addr + entry_offset);

		//  first uboot command is always `bal 1f` => 0x04110002 in hex
		if (cmd_1st != 0x04110002) {
			SERIAL_PUTS("Unexpected content, booting might fail.\n");
		}

		/* reset USB to defaults, otherwise it won't work in original kernel.
		 * starting UBIBoot over USB case */
		REG_USB_POWER = 0x20;
		REG_USB_FADDR = 0x80;

		void (*entry)(void) = (void (*)(void))(KSEG1 + load_addr + entry_offset);
		entry();
		while (1);
	}
	SERIAL_PUTS("Can't read firmware from SD.\n");
}
