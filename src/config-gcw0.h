/* Board-specific config for GCW Zero. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#define CFG_CPU_SPEED		996000000
#define CFG_EXTAL			12000000

#define USES_HIGHMEM

/* serial parameters */
#define LOG_UART			2
#define LOG_BAUDRATE		57600

/* MMC parameters */
#define MMC_ID 0
