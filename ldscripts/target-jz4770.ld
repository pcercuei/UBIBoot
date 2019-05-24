OUTPUT_ARCH(mips)
ENTRY(_start)
LOAD_OFFSET = DEFINED(LOAD_OFFSET) ? LOAD_OFFSET : 0x0 ;

MEMORY
{
	ram	: ORIGIN = 0x80000000 + LOAD_OFFSET , LENGTH = 0x2000 - LOAD_OFFSET
}

SECTIONS
{
	. = ALIGN(4);
	.text.1 : { output/gcw0/head.o(.text*) } > ram

	. = ALIGN(4);
	.rodata : { *(.rodata*) } > ram

	. = ALIGN(4);
	.sdata : { *(.sdata*) } > ram

	. = ALIGN(4);
	.data : { *(.data*) *(.scommon*) *(.reginfo*) } > ram

	_gp = ABSOLUTE(.); /* Base of small data */

	.got : { *(.got*) } > ram

	. = ALIGN(4);
	_bss_start = ABSOLUTE(.);
	.sbss : { *(.sbss*) } > ram
	.bss : { *(.bss*) } > ram
	. = ALIGN (4);
	_bss_end = ABSOLUTE(.);

	/*
	 * Having the code at the end allows the stack to use the same address
	 * space, since instruction and data cache are independent.
	 */
	. = ALIGN(4);
	.text.0 : { *(EXCLUDE_FILE (output/gcw0/head.o) .text*) } > ram

	__stack = 0x80002000;
}

