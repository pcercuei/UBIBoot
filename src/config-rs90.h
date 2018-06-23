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
