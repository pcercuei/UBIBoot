#ifndef _CONFIG_H
#define _CONFIG_H

/* UBIBoot destination storages */
#define DESTINATION_NAND	(1)
#define DESTINATION_MMC		(2)

/* FAT parameters */
/* File names must be 8+3 characters, with spaces at unused positions. */
#define FAT_BOOTFILE_NAME		"VMLINUZ BIN"
/*                       		12345678123 */
#define FAT_BOOTFILE_ALT_NAME	"VMLINUZ BAK"
/*                           	12345678123 */
#define FAT_BOOTIMAGE_NAME		"UZIMAGE BIN"
/*                        		12345678123 */
#define FAT_BOOTIMAGE_ALT_NAME	"UZIMAGE BAK"
/*                            	12345678123 */

/* Physical address to load kernel image at */
#define LD_ADDR					0x00600000

/* Board-specific config */
#if defined(BOARD_gcw0)
#include "config-gcw0.h"
#elif defined(BOARD_a320)
#include "config-a320.h"
#elif defined(BOARD_rs90)
#include "config-rs90.h"
#elif defined(BOARD_lepus)
#include "config-lepus.h"
#elif defined(BOARD_rzx27)
#include "config-rzx27.h"
#elif defined(BOARD_rzx50)
#include "config-rzx50.h"
#else
#error "No config for your board"
#endif

#endif
