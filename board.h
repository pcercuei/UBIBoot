#ifndef _BOARD_H
#define _BOARD_H

extern int pllout2;

extern void gpio_init (void);
extern void sdram_init (void);
extern void serial_init (void);
extern void pll_init (void);
extern void serial_setbrg (void);

#endif

