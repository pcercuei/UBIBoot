/* Board-specific config for the RS90. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#define CFG_CPU_SPEED		360000000
#define CFG_EXTAL			12000000

#define SYSPART_INIT		"/mininit-syspart"

/* serial parameters */
#define LOG_UART			0
#define LOG_BAUDRATE		57600

/* MMC parameters */
#define MMC_ID 1
#define MMC_1BIT 1

/* NAND parameters */
#define PAGE_SIZE 2048 /* 512, 2048 or 4096 */
#define BUS_WIDTH 8 /* 8 or 16 */
#define ROW_CYCLE 3 /* 3 or 2 */
