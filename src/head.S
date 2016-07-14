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
#include "config.h"

	.text 1

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
#if BUS_WIDTH == 8
#if ROW_CYCLE == 3
	addiu zero, zero, 0xffff
#else
	addiu zero, zero, 0xf0f0
#endif
#else
#if ROW_CYCLE == 3
	addiu zero, zero, 0x0f0f
#else
	addiu zero, zero, 0x0000
#endif
#endif
#endif

	/* Disable interrupts */
	mfc0 t0, $12
	addi t1, zero, -2
	and t0, t0, t1
	mtc0 t0, $12

	la	sp, __stack
	j	c_main
	nop

	.set reorder

