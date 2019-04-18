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
#define ECC_BLOCK	512
#define PAGE_PER_BLOCK	64
#define ECC_POS		3
#define PAR_SIZE	13

#define EMC_TAS		2
#define EMC_TAH		1
#define EMC_TBP		2
#define EMC_TAW		2
#define EMC_STRV	13

/* UBI parameters */
#define UBI_MTD_EB_START	1
#define UBI_MTD_NB_EB		2045
#define UBI_KERNEL_VOLUME	"kernel"
#define UBI_KERNEL_BAK_VOLUME	"kernel_bak"
#define UBI_ROOTFS_MTDNAME	"system"
#define UBI_ROOTFS_VOLUME	"rootfs"
