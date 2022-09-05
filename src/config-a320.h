/* Board-specific config for Dingoo A320. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#define CFG_CPU_SPEED		336000000
#define CFG_EXTAL			12000000

#define SYSPART_INIT		"/mininit-syspart"

/* serial parameters */
#define LOG_UART			0
#define LOG_BAUDRATE		57600

/* NAND flash parameters */
#define BUS_WIDTH	8 /* 8 or 16 */
#define ROW_CYCLE	3 /* 3 or 2 */
#define PAGE_SIZE_OFFT	12
#define PAGE_SIZE	(1 << (PAGE_SIZE_OFFT))
#define PAGE_PER_BLOCK	128
#define BAD_BLOCK_POS	((PAGE_SIZE==512) ? 5 : 0)
#define BAD_BLOCK_PAGE	(PAGE_PER_BLOCK -1)
#define ECC_BLOCK	512
#define ECC_POS		6
#define PAR_SIZE	9
#define UBIBOOT_DESTINATION	DESTINATION_NAND

/* UBI parameters */
#define UBI_MTD_EB_START	5
#define UBI_MTD_NB_EB		127
#define UBI_KERNEL_VOLUME	"kernel"
#define UBI_KERNEL_BAK_VOLUME	"kernel_bak"
#define UBI_ROOTFS_MTDNAME	"rootfs"
#define UBI_ROOTFS_VOLUME	"rootfs"

/* MMC parameters */
#define MMC_IDS { 0 }
#define MMC_BLKIDS { 0 }
