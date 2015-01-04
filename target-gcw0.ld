OUTPUT_ARCH(mips)
ENTRY(_start)
MEMORY
{
	ram	: ORIGIN = 0x80000200 , LENGTH = 0x100000
}

SECTIONS
{
	. = ALIGN(4);
	.text : { output/gcw0/head.o(.text*) } > ram
	.text : { *(EXCLUDE_FILE (output/gcw0/head.o) .text*) } > ram

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

	__stack = 0x80002000;
}

