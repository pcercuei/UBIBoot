OUTPUT_ARCH(mips)
ENTRY(_start)

MEMORY
{
	ram	: ORIGIN = 0x80000000 , LENGTH = 0x2000
}

SECTIONS
{
	.text : {
		*(.text.1)
		*(.text.0)
	} > ram

	.data : {
		. = ALIGN(4);
		*(.rodata*) *(.data*) *(.scommon*) *(.reginfo*)
	} > ram

	.bss : {
		. = ALIGN(4);
		_bss_start = ABSOLUTE(.);
		*(.sbss*) *(.bss*)
		. = ALIGN(4);
		_bss_end = ABSOLUTE(.);
	} > ram

	__stack = 0x80004000;
}
