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

#define LD_ADDR 0x80600000

void c_main(void)
{
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
	((void (*)(int, char**, char**, int*))LD_ADDR)(0, NULL, NULL, NULL);
}

