#ifndef _BOARD_H
#define _BOARD_H

extern void board_init (void);

void light(int set);
int alt_key_pressed(void);
int alt2_key_pressed(void);
int alt3_key_pressed(void);
unsigned int get_memory_size(void);

void nand_init(void);
void nand_wait_ready(void);

#ifdef TRY_ORIGINAL_FIRMWARE
void original_firmware_load(void);
#endif

#endif

