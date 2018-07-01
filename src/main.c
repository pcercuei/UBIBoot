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
#include "utils.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Time how long UBIBoot takes to do its job.
 * Uses the JZ4770 OST, so won't work on JZ4740.
 */
#define BENCHMARK 0

/* Kernel parameters list */

/* Fill in root device and file system type? */
#if defined(SYSPART_INIT) || (defined(USE_UBI) && defined(UBI_ROOTFS_VOLUME))
#define PASS_ROOTFS_PARAMS 1
#else
#define PASS_ROOTFS_PARAMS 0
#endif

enum {
	/* Arguments for the kernel itself. */
	PARAM_EXEC = 0,
	PARAM_LOWMEM,
#ifdef USES_HIGHMEM
	PARAM_HIGHMEM,
#endif
#if defined(USE_UBI) && defined(UBI_ROOTFS_MTDNAME)
	PARAM_UBIMTD,
#endif
#if PASS_ROOTFS_PARAMS
#ifdef MININIT1_COMPAT
	PARAM_ROOTCOMPAT,
#endif
	PARAM_ROOTDEV,
	PARAM_ROOTTYPE,
	PARAM_ROOTWAIT,
#endif
#ifdef SYSPART_INIT
	PARAM_READONLY,
#ifdef MININIT1_COMPAT
	PARAM_INITCOMPAT,
#endif
	PARAM_INIT,
#endif
#ifdef USE_SERIAL
	PARAM_CONSOLE_SERIAL,
	PARAM_CONSOLE_SERIAL_EARLYCON,
#endif
	PARAM_CONSOLE_LOCAL,
	PARAM_LOGO,
#ifdef JZ_SLCD_PANEL
	PARAM_SLCD_PANEL,
#endif
#ifdef RFKILL_STATE
	PARAM_RFKILL_STATE,
#endif
	/* Arguments for user space (init and later). */
	PARAM_SEPARATOR,
	PARAM_HWVARIANT,
	PARAM_KERNEL_BAK,
	PARAM_ROOTFS_BAK,
#if BENCHMARK
	PARAM_BOOTBENCH,
#endif
};

#define STRINGIFY(s) #s
#define STRINGIFY_IND(s) STRINGIFY(s)

static char *kernel_params[] = {
	[PARAM_EXEC] = "linux",
	[PARAM_LOWMEM] = "mem=0x0000M",
#ifdef USES_HIGHMEM
	[PARAM_HIGHMEM] = "mem=0x0000M@0x30000000",
#endif
#if defined(USE_UBI) && defined(UBI_ROOTFS_MTDNAME)
	[PARAM_UBIMTD] = "",
#endif
#if PASS_ROOTFS_PARAMS
#ifdef MININIT1_COMPAT
	/* mininit 1.x will pick up the first root=, the kernel the second */
	[PARAM_ROOTCOMPAT] = "root=/dev/loop0",
#endif
	[PARAM_ROOTDEV] = "",
	[PARAM_ROOTTYPE] = "",
	[PARAM_ROOTWAIT] = "rootwait",
#endif
#ifdef SYSPART_INIT
	[PARAM_READONLY] = "ro",
#ifdef MININIT1_COMPAT
	/* mininit 1.x will pick up the first init=, the kernel the second */
	[PARAM_INITCOMPAT] = "init=/sbin/init",
#endif
	[PARAM_INIT] = "init=" SYSPART_INIT,
#endif
#ifdef USE_SERIAL
	[PARAM_CONSOLE_SERIAL] = "console=ttyS" STRINGIFY_IND(LOG_UART)
			"," STRINGIFY_IND(LOG_BAUDRATE),
	[PARAM_CONSOLE_SERIAL_EARLYCON] = "earlycon",
#endif
	[PARAM_CONSOLE_LOCAL] = "console=tty0",
	[PARAM_LOGO] = "",
#ifdef JZ_SLCD_PANEL
	[PARAM_SLCD_PANEL] = "jz4740_slcd_panels.panel=" JZ_SLCD_PANEL,
#endif
#ifdef RFKILL_STATE
	[PARAM_RFKILL_STATE] = "rfkill.default_state=" STRINGIFY_IND(RFKILL_STATE),
#endif
	[PARAM_SEPARATOR] = "--",
	[PARAM_HWVARIANT] = "hwvariant=" VARIANT,
	[PARAM_KERNEL_BAK] = "",
	[PARAM_ROOTFS_BAK] = "",
#if BENCHMARK
	[PARAM_BOOTBENCH] = "bootbench=0x0000000000000000",
#endif
};

static void set_alt_param(void)
{
	kernel_params[PARAM_KERNEL_BAK] = "kernel_bak";
}

static void set_alt2_param(void)
{
	kernel_params[PARAM_ROOTFS_BAK] = "rootfs_bak";
}

static void set_logo_param(int show_logo)
{
	kernel_params[PARAM_LOGO] = show_logo ? "fbcon=bind:0" : "logo.nologo";
}

static void set_mem_param(void)
{
	unsigned int mem_size = get_memory_size() >> 20;
	unsigned int low_mem_size = mem_size > 256 ? 256 : mem_size;

	write_hex_digits(low_mem_size, &kernel_params[PARAM_LOWMEM][9]);

#ifdef USES_HIGHMEM
	unsigned int high_mem_size = mem_size > 256 ? mem_size - 256 : 0;
	if (high_mem_size) {
		write_hex_digits(high_mem_size,
				&kernel_params[PARAM_HIGHMEM][9]);
	} else {
		kernel_params[PARAM_HIGHMEM][0] = '\0';
	}
#endif
}

typedef void (*kernel_main)(int, char**, char**, int*) __attribute__((noreturn));

void c_main(void)
{
	void *exec_addr = NULL;
	int mmc_inited;
	extern unsigned int _bss_start, _bss_end;
	unsigned int *ptr;

	/* Clear the BSS section */
	for (ptr = &_bss_start; ptr < &_bss_end; ptr++)
		*ptr = 0;

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

#ifdef STAGE1_ONLY
	return;
#endif

	/* Tests on JZ4770 show that the data cache lines that contain the boot
	 * loader are not marked as dirty initially. Therefore, if those cache
	 * lines are evicted, the data is lost. To avoid that, we load to the
	 * uncached kseg1 virtual address region, so we never trigger a cache
	 * miss and therefore cause no evictions.
	 */

	mmc_inited = !mmc_init(MMC_ID);
	if (mmc_inited) {
		if (mmc_load_kernel(
				MMC_ID, (void *) (KSEG1 + LD_ADDR), alt_key_pressed(),
				&exec_addr) == 1)
			set_alt_param();

		if (exec_addr) {
#if PASS_ROOTFS_PARAMS
			kernel_params[PARAM_ROOTDEV] =
					"root=/dev/mmcblk0p1";
			kernel_params[PARAM_ROOTTYPE] = "rootfstype=vfat";
#endif
		}
	}

	if (!mmc_inited || !exec_addr) {
		SERIAL_PUTS("Unable to boot from SD."
#ifdef USE_NAND
					" Falling back to NAND."
#endif
					"\n");
#ifndef USE_NAND
		return;
#endif
	}

#ifdef USE_NAND
	if (!exec_addr) {
		nand_init();
#ifdef USE_UBI
		if (ubi_load_kernel((void *) (KSEG1 + LD_ADDR))) {
			SERIAL_PUTS("Unable to boot from NAND.\n");
			return;
		} else {
			exec_addr = (void *) (KSEG0 + LD_ADDR);
#ifdef UBI_ROOTFS_MTDNAME
			kernel_params[PARAM_UBIMTD] = "ubi.mtd=" UBI_ROOTFS_MTDNAME;
#endif
#if PASS_ROOTFS_PARAMS
			kernel_params[PARAM_ROOTDEV] = "root=ubi0:" UBI_ROOTFS_VOLUME;
			kernel_params[PARAM_ROOTTYPE] = "rootfstype=ubifs";
#endif
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
	write_hex_digits(REG_OST_OSTCNTL,
			&kernel_params[PARAM_BOOTBENCH][27]);
	write_hex_digits(REG_OST_OSTCNTH_BUF,
			&kernel_params[PARAM_BOOTBENCH][27 - 8]);
#endif

	if (alt2_key_pressed())
		set_alt2_param();

	set_logo_param(!alt3_key_pressed());
	set_mem_param();

	/* Since we load to kseg1, there is no data we want to keep in the cache,
	 * so no need to flush it to RAM.
	jz_flush_dcache();
	jz_flush_icache();
	*/

	SERIAL_PUTS("Kernel loaded. Executing...\n\n");

	/* Boot the kernel */
	((kernel_main) exec_addr) (
			ARRAY_SIZE(kernel_params), kernel_params, NULL, NULL );
}

