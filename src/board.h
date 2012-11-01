#ifndef _BOARD_H
#define _BOARD_H

extern void gpio_init (void);
extern void sdram_init (void);
extern void serial_init (void);
extern void pll_init (void);
extern void serial_setbrg (void);

void udelay(unsigned int us);

#endif

