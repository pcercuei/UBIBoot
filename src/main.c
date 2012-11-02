/*
 *  Copyright (C) 2009 Ignacio Garcia Perez <iggarpe@gmail.com>
 *  Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdlib.h>

#include "config.h"	/* Always first, defines CFG_EXTAL for jz4740.h */
#include "jz4740.h"

#include "board.h"
#include "nand.h"
#include "serial.h"
#include "ubi.h"
#include "mmc.h"
#include "fat.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Kernel parameters list */
static char *kernel_params [] = {
	[0] = "linux",
	[1] = "mem=0x0000M",
#ifdef JZ_SLCD_PANEL
	"jz4740_slcd_panels.panel=" JZ_SLCD_PANEL,
#endif
};


static void set_mem_param(void)
{
	unsigned int mem_size = get_memory_size() >> 20;
	char *ptr = &kernel_params[1][9];

	do {
		char nb = (char) (mem_size & 0xf);
		if (nb >= 10)
			*ptr-- = 'a' + nb - 10;
		else
			*ptr-- = '0' + nb;
		mem_size >>= 4;
	} while(mem_size && (*ptr != 'x'));
}

void c_main(void)
{
	register uint32_t reg;
	int boot_from_sd = 0;

	board_init();

	SERIAL_PUTS("UBIBoot by Paul Cercueil <paul@crapouillou.net>\n");
#ifdef BKLIGHT_ON
	light(1);
#endif

	if (!alt_key_pressed()) {
		if (mmc_init() || mmc_load_kernel((unsigned char *) LD_ADDR))
			SERIAL_PUTS("Unable to boot from SD."
#ifdef USE_NAND
				" Falling back to NAND."
#endif
				"\n");
		else
			boot_from_sd = 1;
	}

	if (!boot_from_sd) {
#ifdef USE_NAND
		nand_init();
#ifdef USE_UBI
		if (ubi_load_kernel((unsigned char *) LD_ADDR)) {
			SERIAL_PUTS("Unable to boot from NAND.\n");
			return;
		}
#else /* USE_UBI */
#warning UBI is currently the only supported NAND file system and it was not selected.
#endif /* USE_UBI */
#endif /* USE_NAND */
	}

	set_mem_param();

	jz_flush_dcache();
	jz_flush_icache();

	SERIAL_PUTS("Kernel loaded. Executing...\n");

	/* WP bit clear in CP0_CAUSE ($13), needed to boot dingux zImage
	 * (original fix by BouKiCHi) */
	 asm volatile("mfc0 %0, $13\n\t"
				"and %0, ~(0x00400000)\n\t"
				"mtc0 %0, $13\n\t" : "=r"(reg) :);

	/* Boot the kernel */
	((void (*)(int, char**, char**, int*)) LD_ADDR) (
			ARRAY_SIZE(kernel_params), kernel_params, NULL, NULL );
}

