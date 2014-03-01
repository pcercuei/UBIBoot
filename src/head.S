/*
 * head.S
 *
 * Entry point of the firmware.
 * The firmware code are executed in the ICache.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 */


#include "asm/regdef.h"

	.text

	.extern c_main

	.globl _start
	.set noreorder

_start:

	//----------------------------------------------------
	// setup stack, jump to C code
	//----------------------------------------------------

#if JZ_VERSION >= 4770
	// The JZ4770 won't load the program
	// if the first word is not 'MSPL'
	.word 0x4d53504c
#else
	// The JZ4740 won't load the program
	// if the first byte is not 0xff.
	addiu zero, zero, 0xffff
#endif

	la	sp, __stack
	j	c_main
	nop

	.set reorder

