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

	// This does nothing, but the JZ won't load the program
	// if the first byte is not 0xff.
	addiu zero,zero,0xffff

	la	sp, 0x80004000
	j	c_main

	.set reorder

