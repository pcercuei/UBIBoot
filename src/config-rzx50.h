/* Board-specific config for the RZX27. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#define CFG_CPU_SPEED   432000000
/* Actually its 24MHz but peripherals working on CFG_EXTAL/2 */
/* This fixes UART speed, however its redefined to 24M in board file */
#define CFG_EXTAL       12000000

#define SYSPART_INIT    "/mininit-syspart"

/* serial parameters */
#define LOG_UART        1
#define LOG_BAUDRATE    57600

#define UBIBOOT_DESTINATION DESTINATION_MMC

/* MMC parameters */
#define MMC_IDS { 1, 0 } /* try external first */
#define MMC_BLKIDS { 1, 0 } /* /dev/mmcblkX to set root properly */
