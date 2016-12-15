#ifndef _BOARD_H
#define _BOARD_H

extern void board_init (void);

void light(int set);
int alt_key_pressed(void);
int alt2_key_pressed(void);
int alt3_key_pressed(void);
unsigned int get_memory_size(void);

#endif

