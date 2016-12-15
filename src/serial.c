#include "config.h"
#include "board.h"

#include "serial.h"
#include "jz.h"
#include "utils.h"

void serial_putc(const char c)
{
	volatile u8 *uart_lsr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_LSR);
	volatile u8 *uart_tdr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_TDR);

	if (c == '\n') serial_putc('\r');

	/* Wait for fifo to shift out some bytes */
	while ( !((*uart_lsr & (UART_LSR_TDRQ | UART_LSR_TEMT)) == 0x60) );

	*uart_tdr = (u8)c;
}

void serial_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

int serial_getc(void)
{
	volatile u8 *uart_rdr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_RDR);

	while (!serial_tstc());

	return *uart_rdr;
}

int serial_tstc(void)
{
	volatile u8 *uart_lsr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_LSR);

	if (*uart_lsr & UART_LSR_DR) {
		/* Data in rfifo */
		return 1;
	}
	return 0;
}

void serial_init(void)
{
	volatile u8 *uart_fcr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_FCR);
	volatile u8 *uart_lcr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_LCR);
	volatile u8 *uart_ier = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_IER);
	volatile u8 *uart_sircr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_SIRCR);
	volatile u8 *uart_dlhr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_DLHR);
	volatile u8 *uart_dllr = (volatile u8 *)(UART_BASE(LOG_UART) + OFF_DLLR);
	u32 baud_div, tmp;

	/* Disable port interrupts while changing hardware */
	*uart_ier = 0;

	/* Disable UART unit function */
	*uart_fcr = ~UART_FCR_UUE;

	/* Set both receiver and transmitter in UART mode (not SIR) */
	*uart_sircr = ~(SIRCR_RSIRE | SIRCR_TSIRE);

	/* Set databits, stopbits and parity. (8-bit data, 1 stopbit, no parity) */
	*uart_lcr = UART_LCR_WLEN_8 | UART_LCR_STOP_1;

	/* Set baud rate */
	baud_div = CFG_EXTAL / 16 / LOG_BAUDRATE;
	tmp = *uart_lcr;
	tmp |= UART_LCR_DLAB;
	*uart_lcr = tmp;

	*uart_dlhr = (baud_div >> 8) & 0xff;
	*uart_dllr = baud_div & 0xff;

	tmp &= ~UART_LCR_DLAB;
	*uart_lcr = tmp;

	/* Enable UART unit, enable and clear FIFO */
	*uart_fcr = UART_FCR_UUE | UART_FCR_FE | UART_FCR_TFLS | UART_FCR_RFLS;
}



void serial_putb(unsigned int d)
{
	unsigned int m;
	for (m = 0x80000000; m != 0x00800000; m >>= 1) serial_putc(m & d ? '1' : '0');
	serial_putc(' ');
	for (m = 0x00800000; m != 0x00008000; m >>= 1) serial_putc(m & d ? '1' : '0');
	serial_putc(' ');
	for (m = 0x00008000; m != 0x00000080; m >>= 1) serial_putc(m & d ? '1' : '0');
	serial_putc(' ');
	for (m = 0x00000080; m != 0x00000000; m >>= 1) serial_putc(m & d ? '1' : '0');
}

void serial_puth(unsigned int d)
{
	char *message = "ERR: 0x00000000\r";
	/*               0123456789012345 */
	write_hex_digits(d, &message[14]);
	serial_puts(message);
}

void serial_put_regb(const char *name, unsigned int value)
{
	serial_puts(name);
	serial_puts(" = ");
	serial_putb(value);
	serial_putc('\n');
}

void serial_put_regh(const char *name, unsigned int value)
{
	serial_puts(name);
	serial_puts(" = ");
	serial_puth(value);
	serial_putc('\n');
}
