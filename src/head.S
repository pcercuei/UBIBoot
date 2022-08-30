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

	.text
	.section .text.1

	.extern c_main

	.globl _start
	.set noreorder

_start:

	//----------------------------------------------------
	// setup stack, jump to C code
	//----------------------------------------------------

#ifndef STAGE1_ONLY
#if UBIBOOT_DESTINATION == DESTINATION_NAND
#if JZ_VERSION == 4750
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
	/* Without a NOP here it won't boot on the RS90. Go figure. */
	nop
#elif JZ_VERSION == 4740
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
#else
#error "Booting from NAND doesn't supported yet for the SoC"
#endif
#elif UBIBOOT_DESTINATION == DESTINATION_MMC
#if JZ_VERSION == 4725 || JZ_VERSION == 4760 || JZ_VERSION == 4770
	/* Those won't load the program
	 * if the first word is not 'MSPL' */
	.word 0x4d53504c
#elif JZ_VERSION == 4755
	/* It has no header (at least RZX-50 case) */
#else
#error "Booting from MMC doesn't supported yet for the SoC"
#endif
#ifdef HAVE_MBR
	j _endmbr  /* Jump over MBR immediately */
	nop        /* delay slot */

	/* 504 bytes reserved for MBR.
	 * overalloc a bit whether header is present */
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
_endmbr:
#endif
#else
#error "UBIBOOT_DESTINATION is not set, check your config-<board>.h file"
#endif
#endif /* STAGE1_ONLY */

	/* Disable interrupts in CP0_STATUS */
	li $8, 0x00400004
	mtc0 $8, $12

	/* Clear CP0_CAUSE */
	li $9, 0x00800000
	mtc0 $9, $13

	/* Load the stack and branch to c_main() */
	la	sp, __stack
#ifdef STAGE1_ONLY
	/* Bypass the boot ROM's USB reset after stage1 */
#if JZ_VERSION == 4770
	li ra,0xbfc0130c
#elif JZ_VERSION == 4760
	li ra,0xbfc012e0
#endif
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

