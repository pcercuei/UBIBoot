#ifndef _ERRORCODES_H
#define _ERRORCODES_H

/* (replaced by 0x08)		0x00		   Unable to read bootsector. */
#define ERR_FAT_NO_MBR		0x01		/* No MBR detected. */
#define ERR_FAT_NO_PART		0x02		/* Unable to find first partition. */
#define ERR_FAT_IO_PART		0x03		/* Unable to read from first partition. */
#define ERR_FAT_IO_FAT		0x04		/* Unable to read the FAT table. */
#define ERR_FAT_NO_FAT32	0x05		/* No FAT32 filesystem detected. */
#define ERR_FAT_IO_ROOT		0x06		/* Unable to read root directory. */
#define ERR_FAT_NO_KERNEL	0x07		/* Kernel file not found. */
#define ERR_FAT_IO_BOOT		0x08		/* Unable to read bootsector. */
#define ERR_FAT_BAD_IMAGE	0x09		/* uImage header rejected. */

#define ERR_MMC_INIT		0x0a		/* Initialization failed. */
#define ERR_MMC_TIMEOUT		0x0b		/* Time out. */
#define ERR_MMC_IO			0x0c		/* Read error. */
#define ERR_MMC_SEND_IF_COND 0x0e
#define ERR_MMC_SEND_OP_COND 0x0f
#define ERR_MMC_SEND_OP_COND_TOUT 0x10
#define ERR_MMC_SEND_OP_COND_ERR 0x11
#define ERR_MMC_NTOUT       0x12
#define ERR_MMC_CID         0x13
#define ERR_MMC_RCA         0x14
#define ERR_MMC_CSD         0x15
#define ERR_MMC_SELECT      0x16
#define ERR_SD_CMD          0x17
#define ERR_SD_BW           0x18
#define ERR_MMC_SWITCH      0x19

#define ERR_NAND_IO_UNC		0x20		/* Uncorrectable read error. */
#define ERR_NAND_IO			0x21		/* Read error. */

#define ERR_UBI_NO_PART		0x30		/* Partition is not a UBI drive. */
#define ERR_UBI_NO_KERNEL	0x31		/* Unable to locate kernel partition. */
#define ERR_UBI_IO		0x32		/* UBI structure parsing failed */

#endif
