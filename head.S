/*
 * head.S
 *
 * Entry point of the firmware.
 * The firmware code are executed in the ICache.
 *
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 *
 * FIXED: WP bit clear in CP0_CAUSE, needed to boot dingux zImage
 *        (original fix by BouKiCHi)
 *
 */


#include "asm/regdef.h"
#include "asm/mipsregs.h"
#include "asm/addrspace.h"
#include "asm/cacheops.h"

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

	addiu	sp,sp,-4
	sw	ra,0(sp)

	jal	c_main
	nop

	lw	ra,0(sp)
	addiu	sp,sp,4
	
	mfc0	t0,CP0_CAUSE
	and	t0,~(0x00400000)
	mtc0	t0,CP0_CAUSE

	jr	ra
	nop

	.set reorder

