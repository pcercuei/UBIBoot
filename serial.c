#include "config.h"	/* Always first, defines CFG_EXTAL for jz4740.h */
#include "jz4740.h"
#include "board.h"

#include "serial.h"

void serial_putc (const char c)
{
	volatile u8 *uart_lsr = (volatile u8 *)(UART_BASE + OFF_LSR);
	volatile u8 *uart_tdr = (volatile u8 *)(UART_BASE + OFF_TDR);

	if (c == '\n') serial_putc ('\r');

	/* Wait for fifo to shift out some bytes */
	while ( !((*uart_lsr & (UART_LSR_TDRQ | UART_LSR_TEMT)) == 0x60) );

	*uart_tdr = (u8)c;
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

int serial_getc (void)
{
	volatile u8 *uart_rdr = (volatile u8 *)(UART_BASE + OFF_RDR);

	while (!serial_tstc());

	return *uart_rdr;
}

int serial_tstc (void)
{
	volatile u8 *uart_lsr = (volatile u8 *)(UART_BASE + OFF_LSR);

	if (*uart_lsr & UART_LSR_DR) {
		/* Data in rfifo */
		return (1);
	}
	return 0;
}

void serial_init(void)
{
	volatile u8 *uart_fcr = (volatile u8 *)(UART_BASE + OFF_FCR);
	volatile u8 *uart_lcr = (volatile u8 *)(UART_BASE + OFF_LCR);
	volatile u8 *uart_ier = (volatile u8 *)(UART_BASE + OFF_IER);
	volatile u8 *uart_sircr = (volatile u8 *)(UART_BASE + OFF_SIRCR);

	/* Disable port interrupts while changing hardware */
	*uart_ier = 0;

	/* Disable UART unit function */
	*uart_fcr = ~UART_FCR_UUE;

	/* Set both receiver and transmitter in UART mode (not SIR) */
	*uart_sircr = ~(SIRCR_RSIRE | SIRCR_TSIRE);

	/* Set databits, stopbits and parity. (8-bit data, 1 stopbit, no parity) */
	*uart_lcr = UART_LCR_WLEN_8 | UART_LCR_STOP_1;

	/* Set baud rate */
	serial_setbrg();

	/* Enable UART unit, enable and clear FIFO */
	*uart_fcr = UART_FCR_UUE | UART_FCR_FE | UART_FCR_TFLS | UART_FCR_RFLS;
}



static const char hex [16] = "0123456789ABCDEF";

void serial_putb (unsigned int d) {
	unsigned int m;
	for (m = 0x80000000; m != 0x00800000; m >>= 1) serial_putc(m & d ? '1' : '0');
	serial_putc(' ');
	for (m = 0x00800000; m != 0x00008000; m >>= 1) serial_putc(m & d ? '1' : '0');
	serial_putc(' ');
	for (m = 0x00008000; m != 0x00000080; m >>= 1) serial_putc(m & d ? '1' : '0');
	serial_putc(' ');
	for (m = 0x00000080; m != 0x00000000; m >>= 1) serial_putc(m & d ? '1' : '0');
}

void serial_puth (unsigned int d) {
	serial_putc(hex[(d >> 28) & 0xF]);
	serial_putc(hex[(d >> 24) & 0xF]);
	serial_putc(hex[(d >> 20) & 0xF]);
	serial_putc(hex[(d >> 16) & 0xF]);
	serial_putc(hex[(d >> 12) & 0xF]);
	serial_putc(hex[(d >>  8) & 0xF]);
	serial_putc(hex[(d >>  4) & 0xF]);
	serial_putc(hex[(d >>  0) & 0xF]);
}

void serial_put_regb(const char *name, unsigned int value) {
	serial_puts(name);
	serial_puts(" = ");
	serial_putb(value);
	serial_putc('\n');
}

void serial_put_regh(const char *name, unsigned int value) {
	serial_puts(name);
	serial_puts(" = ");
	serial_puth(value);
	serial_putc('\n');
}

