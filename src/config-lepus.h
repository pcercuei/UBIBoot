/* Board-specific config for lepus. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#ifndef __ASSEMBLER__
#include "jz4760-cpm.h"
#define CFG_CPU_SPEED		((REG_CPM_CLKGR0 & BIT(31)) ? 1200000000 : 1056000000)
#endif

#define CFG_EXTAL		12000000

#define MBR_PRELOAD_ADDR	0x80000000

#define SYSPART_INIT		"/mininit-syspart"
#define MININIT1_COMPAT

#define RFKILL_STATE		0

//#define USES_HIGHMEM

#define UBIBOOT_DESTINATION	DESTINATION_MMC

/* serial parameters */
#define LOG_UART		1
#define LOG_BAUDRATE		57600

/* MMC parameters */
#define MMC_ID 0
#define MMC_ID2 2
