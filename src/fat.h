
#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#define FAT_BLOCK_SIZE MMC_SECTOR_SIZE

/* File attributes */
#define ATTR_RO      1
#define ATTR_HIDDEN  2
#define ATTR_SYS     4
#define ATTR_VOLUME  8
#define ATTR_DIR     16
#define ATTR_ARCH    32

struct partition {
	uint8_t		status;
	uint8_t		chs_start[3];
	uint8_t		type;
	uint8_t		chs_end[3];
	uint32_t	lba;
	uint32_t	nb_sectors;
};

struct mbr {
	uint8_t		bootstrap[440];
	uint32_t	signature_opt;
	uint16_t	reserved;
	struct partition partitions[4];
	uint16_t	signature;
} __attribute__((packed));

struct boot_sector {
	uint8_t		ignored[3];		/* Bootstrap code */
	char		system_id[8];	/* Name of fs */
	uint8_t		sector_size[2];	/* Bytes/sector */
	uint8_t		cluster_size;	/* Sectors/cluster */
	uint16_t	reserved;		/* Number of reserved sectors */
	uint8_t		fats;			/* Number of FATs */
	uint8_t		dir_entries[2];	/* Number of root directory entries */
	uint8_t		sectors[2];		/* Number of sectors */
	uint8_t		media;			/* Media code */
	uint16_t	fat_length;		/* Sectors/FAT */
	uint16_t	secs_track;		/* Sectors/track */
	uint16_t	heads;			/* Number of heads */
	uint32_t	hidden;			/* Number of hidden sectors */
	uint32_t	total_sect;		/* Number of sectors (if sectors == 0) */

	/* FAT32 only */
	uint32_t	fat32_length;	/* Sectors/FAT */
	uint16_t	flags;			/* Bit 8: fat mirroring, low 4: active fat */
	uint8_t		version[2];		/* Filesystem version */
	uint32_t	root_cluster;	/* First cluster in root directory */
	uint16_t	info_sector;	/* Filesystem info sector */
	uint16_t	backup_boot;	/* Backup boot sector */
	uint16_t	reserved2[6];	/* Unused */
};

struct volume_info
{
	uint8_t	drive_number;		/* BIOS drive number */
	uint8_t	reserved;			/* Unused */
	uint8_t	ext_boot_sign;		/* 0x29 if fields below exist (DOS 3.3+) */
	uint8_t	volume_id[4];		/* Volume ID number */
	char	volume_label[11];	/* Volume label */
	char	fs_type[8];			/* Typically FAT12, FAT16, or FAT32 */
	/* Boot code comes next, all but 2 bytes to fill up sector */
	/* Boot sign comes last, 2 bytes */
};

struct dir_entry {
	char		name[8], ext[3];	/* Name and extension */
	uint8_t		attr;				/* Attribute bits */
	uint8_t		lcase;				/* Case for base and extension */
	uint8_t		ctime_ms;			/* Creation time, milliseconds */
	uint16_t	ctime;				/* Creation time */
	uint16_t	cdate;				/* Creation date */
	uint16_t	adate;				/* Last access date */
	uint16_t	starthi;			/* High 16 bits of cluster in FAT32 */
	uint16_t	time, date, start;	/* Time, date and first cluster */
	uint32_t	size;				/* File size in bytes */
};

/*
 * Attempts to load a kernel from the MMC/SD card in slot 'id' into memory
 * at 'ld_addr'. If 'alt' is true, try the alternative name first.
 * The execution address of the loaded kernel is written to the output argument
 * 'exec_addr'; nothing is written if the load was unsuccessful.
 * Return 0 if a regular kernel was loaded, 1 if an alternative kernel
 * was loaded or a negative number on error.
 */
int mmc_load_kernel(unsigned int id, void *ld_addr, int alt, void **exec_addr);

#endif
