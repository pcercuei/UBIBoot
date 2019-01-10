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
#elif !defined(STAGE1_ONLY) && (JZ_VERSION >= 4750)
#if BUS_WIDTH == 8
	.word 0x555555ff
#else
	.word 0x55555500
#endif
	.word 0x55555555
#if ROW_CYCLE == 3
	.byte 0xff
#else
	.byte 0x00
#endif
#if PAGE_SIZE == 512
	.byte 0x00
	.byte 0x00
#elif PAGE_SIZE == 2048
	.byte 0xff
	.byte 0xff
#else
	.byte 0xff
	.byte 0x00
#endif
	.byte 0x00
#else /* JZ4740 */
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

	/* Without a NOP here it won't boot on the RS90. Go figure. */
	nop

	/* Disable interrupts in CP0_STATUS */
	li $8, 0x00400004
	mtc0 $8, $12

	/* Clear CP0_CAUSE */
	li $9, 0x00800000
	mtc0 $9, $13

	/* Load the stack and branch to c_main() */
	la	sp, __stack
#ifdef STAGE1_ONLY
	j	c_main
#else
	jal	c_main
	nop

_end:
	wait
	b _end
#endif
	nop

	.set reorder

