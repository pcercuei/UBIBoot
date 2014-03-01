/*
 *  Copyright (C) 2009 Ignacio Garcia Perez <iggarpe@gmail.com>
 *  Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdlib.h>

#include "config.h"

#include "board.h"
#include "nand.h"
#include "serial.h"
#include "ubi.h"
#include "mmc.h"
#include "fat.h"
#include "jz.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Time how long UBIBoot takes to do its job.
 * Uses the JZ4770 OST, so won't work on JZ4740.
 */
#define BENCHMARK 0

#define MMC_ID 0

/* Kernel parameters list */
static char *kernel_params [] = {
	[0] = "linux",
	[1] = "mem=0x0000M",
	[2] = "mem=0x0000M@0x30000000",
	[3] = "",
	[4] = "",
#if BENCHMARK
	"bootbench=0x0000000000000000",
#endif
	"hwvariant=" VARIANT,
#ifdef JZ_SLCD_PANEL
	"jz4740_slcd_panels.panel=" JZ_SLCD_PANEL,
#endif
};

static void set_alt_param(void)
{
	kernel_params[3] = "kernel_bak";
}

static void set_alt2_param(void)
{
	kernel_params[4] = "rootfs_bak";
}

static void write_hex_digits(unsigned int value, char *last_digit)
{
	char *ptr = last_digit;
	do {
		char nb = (char) (value & 0xf);
		if (nb >= 10)
			*ptr-- = 'a' + nb - 10;
		else
			*ptr-- = '0' + nb;
		value >>= 4;
	} while(value && (*ptr != 'x'));
}

static void set_mem_param(void)
{
	unsigned int mem_size = get_memory_size() >> 20;
	unsigned int low_mem_size = mem_size > 256 ? 256 : mem_size;
	unsigned int high_mem_size = mem_size > 256 ? mem_size - 256 : 0;

	write_hex_digits(low_mem_size, &kernel_params[1][9]);
	write_hex_digits(high_mem_size, &kernel_params[2][9]);
}


void c_main(void)
{
	register uint32_t reg;
	int boot = 0;
	int mmc_inited;

#if BENCHMARK
	/* Setup 3 MHz timer, 64-bit wrap, abrupt stop. */
	REG_OST_OSTCSR = OSTCSR_CNT_MD | OSTCSR_SD
				   | OSTCSR_EXT_EN | OSTCSR_PRESCALE4;
	__tcu_stop_counter(15);
	REG_OST_OSTCNTL = 0;
	REG_OST_OSTCNTH = 0;
	__tcu_start_counter(15);
#endif

	board_init();

	SERIAL_PUTS("UBIBoot by Paul Cercueil <paul@crapouillou.net>\n");
#ifdef BKLIGHT_ON
	light(1);
#endif

	/* Tests on JZ4770 show that the data cache lines that contain the boot
	 * loader are not marked as dirty initially. Therefore, if those cache
	 * lines are evicted, the data is lost. To avoid that, we load to the
	 * uncached kseg1 virtual address region, so we never trigger a cache
	 * miss and therefore cause no evictions.
	 */

	mmc_inited = !mmc_init(MMC_ID);
	if (mmc_inited) {
		/* Alt key not pressed: try to boot the regular kernel;
		 * if it fails, try to boot the alt kernel */
		if (!alt_key_pressed()) {
			boot = !mmc_load_kernel(MMC_ID, (void *) (KSEG1 + LD_ADDR),
						FAT_BOOTFILE_NAME, FAT_BOOTFILE_EXT);
			if (!boot) {
				boot = !mmc_load_kernel(MMC_ID, (void *) (KSEG1 + LD_ADDR),
							FAT_BOOTFILE_ALT_NAME, FAT_BOOTFILE_ALT_EXT);
				set_alt_param();
			}
		}

		/* Alt key is pressed: try to boot the alt kernel;
		 * if it fails, try to boot the regular kernel */
		else {
			boot = !mmc_load_kernel(MMC_ID, (void *) (KSEG1 + LD_ADDR),
						FAT_BOOTFILE_ALT_NAME, FAT_BOOTFILE_ALT_EXT);
			if (!boot)
				boot = !mmc_load_kernel(MMC_ID, (void *) (KSEG1 + LD_ADDR),
							FAT_BOOTFILE_NAME, FAT_BOOTFILE_EXT);
			else
				set_alt_param();
		}
	}

	if (!mmc_inited || !boot)
		SERIAL_PUTS("Unable to boot from SD."
#ifdef USE_NAND
					" Falling back to NAND."
#endif
					"\n");

#ifdef USE_NAND
	if (!boot) {
		nand_init();
#ifdef USE_UBI
		if (ubi_load_kernel((void *) (KSEG1 + LD_ADDR))) {
			SERIAL_PUTS("Unable to boot from NAND.\n");
			return;
		}
#else /* USE_UBI */
#warning UBI is currently the only supported NAND file system and it was not selected.
#endif /* USE_UBI */
	}
#endif /* USE_NAND */

#if BENCHMARK
	/* Stop timer. */
	__tcu_stop_counter(15);
	/* Store timer count in kernel command line. */
	write_hex_digits(REG_OST_OSTCNTL, &kernel_params[4][27]);
	write_hex_digits(REG_OST_OSTCNTH_BUF, &kernel_params[4][27 - 8]);
#endif

	if (alt2_key_pressed())
		set_alt2_param();

	set_mem_param();

	/* Since we load to kseg1, there is no data we want to keep in the cache,
	 * so no need to flush it to RAM.
	jz_flush_dcache();
	jz_flush_icache();
	*/

	SERIAL_PUTS("Kernel loaded. Executing...\n");

	/* WP bit clear in CP0_CAUSE ($13), needed to boot dingux zImage
	 * (original fix by BouKiCHi) */
	 asm volatile("mfc0 %0, $13\n\t"
				"and %0, ~(0x00400000)\n\t"
				"mtc0 %0, $13\n\t" : "=r"(reg) :);

	/* Boot the kernel */
	((void (*)(int, char**, char**, int*)) (KSEG0 + LD_ADDR)) (
			ARRAY_SIZE(kernel_params), kernel_params, NULL, NULL );
}

