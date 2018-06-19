#ifndef _SERIAL_H
#define _SERIAL_H

#ifdef USE_SERIAL
#define SERIAL_PUTS(x) serial_puts(x)
#define SERIAL_PUTC(x) serial_putc(x)
#define SERIAL_PUTH(x) serial_puth(x)
#define SERIAL_PUTI(x) serial_puti(x)
#else
#define SERIAL_PUTS(x)
#define SERIAL_PUTC(x)
#define SERIAL_PUTH(x)
#define SERIAL_PUTI(x)
#endif

#define SERIAL_PUTS_ARGI(pre, val, post) \
	do {	\
		SERIAL_PUTS(pre);	\
		SERIAL_PUTI(val);	\
		SERIAL_PUTS(post);	\
	} while(0)

#define SERIAL_PUTS_ARGH(pre, val, post) \
	do {	\
		SERIAL_PUTS(pre);	\
		SERIAL_PUTH(val);	\
		SERIAL_PUTS(post);	\
	} while(0)

#define SERIAL_ERR(err) SERIAL_PUTS_ARGI("ERROR ", (err), "\n")

void serial_putc(const char c);
void serial_puts(const char *s);
int serial_getc(void);
int serial_tstc(void);
void serial_init(void);
void serial_putb(unsigned int d);
void serial_puth(unsigned int d);
void serial_puti(unsigned int d);

#endif

