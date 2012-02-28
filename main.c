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

#define PIN_BKLIGHT	(32*3+31)	/* Port 3 pin 31: Backlight PWM  */

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


/* Kernel parameters list */
static char *kernel_params [] = {
	"linux",
#ifdef JZ_SLCD_PANEL
	"jz_slcd=panel:" JZ_SLCD_PANEL,
#endif
};


void c_main(void)
{
	register uint32_t reg;

	gpio_init();

	__gpio_clear_pin(PIN_BKLIGHT);
	__gpio_as_output(PIN_BKLIGHT);


#ifdef USE_SERIAL
	serial_init();
#endif

	pll_init();
	sdram_init();
	nand_init();

	SERIAL_PUTS("UBIBoot by Paul Cercueil <paul@crapouillou.net>\n");

	__gpio_set_pin(PIN_BKLIGHT);

	if (load_kernel(UBI_MTD_EB_START, UBI_MTD_NB_EB,
					(unsigned char*)LD_ADDR))
		return;

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

