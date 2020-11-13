/* Board-specific config for the RetroGame RS-97. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#define CFG_CPU_SPEED		300000000
#define CFG_EXTAL		 12000000

#define MBR_PRELOAD_ADDR	0x80000000

#define SYSPART_INIT		"/mininit-syspart"
#define MININIT1_COMPAT

#define RFKILL_STATE		0

/* serial parameters */
#define LOG_UART		1
#define LOG_BAUDRATE		57600

/* MMC parameters */
#define MMC_ID 0
