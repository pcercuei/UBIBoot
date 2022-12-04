/* Board-specific config for GCW Zero. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#define CFG_CPU_SPEED		996000000
#define CFG_EXTAL			12000000

#define MBR_PRELOAD_ADDR	0x80000000

#define SYSPART_INIT		"/mininit-syspart"

#define RFKILL_STATE		0

#define USES_HIGHMEM

#define UBIBOOT_DESTINATION	DESTINATION_MMC

/* serial parameters */
#define LOG_UART			2
#define LOG_BAUDRATE		57600

/* MMC parameters */
#define MMC_IDS { 0 }
#define MMC_BLKIDS { 0 }
