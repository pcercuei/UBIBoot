#ifndef _CONFIG_H
#define _CONFIG_H

/* FAT parameters */
#define FAT_BOOTFILE_NAME		"VMLINUZ"
#define FAT_BOOTFILE_EXT		"BIN"
#define FAT_BOOTFILE_ALT_NAME	"VMLINUZ"
#define FAT_BOOTFILE_ALT_EXT	"BAK"

/* Physical address to load kernel image at */
#define LD_ADDR					0x00600000

/* Board-specific config */
#if defined(BOARD_gcw0)
#include "config-gcw0.h"
#elif defined(BOARD_a320)
#include "config-a320.h"
#else
#error "No config for your board"
#endif

#endif
