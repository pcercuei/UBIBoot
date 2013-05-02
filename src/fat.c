
#include <stdint.h>
#include <string.h>

#include "serial.h"
#include "mmc.h"
#include "fat.h"

static struct boot_sector bs;

static int get_first_partition(uint32_t *lba)
{
	struct mbr mbr;

	if (mmc_block_read((uint32_t *) &mbr, 0, 1)) {
		/* Unable to read bootsector. */
		SERIAL_PUTI(0x00);
		return -1;
	}

	if (mbr.signature != 0xAA55) {
		/* No MBR detected. */
		SERIAL_PUTI(0x01);
		return -1;
	}

	if (mbr.partitions[0].status
				&& mbr.partitions[0].status != 0x80) {
		/* Unable to detect first physical partition. */
		SERIAL_PUTI(0x02);
		return -1;
	}

	*lba =  mbr.partitions[0].lba;
	return 0;
}

static int load_from_cluster(uint32_t lba, uint32_t cluster, void *ld_addr)
{
	uint32_t old_fat_sector = 0;

	while(1) {
		uint32_t sector[FAT_BLOCK_SIZE >> 2];
		uint32_t fat_sector = lba + bs.reserved + (cluster / (FAT_BLOCK_SIZE >> 2));

		/* Read file data */
		if (mmc_block_read(ld_addr, lba + bs.reserved + bs.fat32_length * bs.fats
						+ (cluster-2) * bs.cluster_size, bs.cluster_size)) {
			/* Unable to read from first partition. */
			SERIAL_PUTI(0x03);
			return -1;
		}
		ld_addr += bs.cluster_size * FAT_BLOCK_SIZE;

		/* Read FAT */
		if (fat_sector != old_fat_sector) {
			if (mmc_block_read(sector, fat_sector, 1)) {
				/* Unable to read the FAT table. */
				SERIAL_PUTI(0x04);
				return -1;
			}
			old_fat_sector = fat_sector;
		}

		cluster = sector[cluster % (FAT_BLOCK_SIZE >> 2)] & 0x0fffffff;
		if ((cluster >= 0x0ffffff0) || (cluster <= 1))
			break;
	}

	return 0;
}

static int load_kernel_lba(uint32_t lba, void *ld_addr,
						   const char *name, const char *ext)
{
	uint32_t sector[FAT_BLOCK_SIZE >> 2];
	uint32_t cur_sect;
	size_t i, j;
	struct volume_info vinfo;
	size_t name_len = strlen(name);
	size_t ext_len = strlen(ext);

	if (mmc_block_read(sector, lba, 1)) {
		/* Unable to read from first partition. */
		SERIAL_PUTI(0x03);
		return -1;
	}

	memcpy(&bs, sector, sizeof(struct boot_sector));
	memcpy(&vinfo, (void *) sector + sizeof(struct boot_sector),
				sizeof(struct volume_info));

	if (strncmp(vinfo.fs_type, "FAT32", 5)) {
		/* No FAT32 filesystem detected. */
		SERIAL_PUTI(0x05);
		return -1;
	}

	SERIAL_PUTS("MMC: FAT32 filesystem detected.\n");

	for (cur_sect = lba + bs.reserved + bs.fat32_length * bs.fats, i = 0;
				i < bs.cluster_size; cur_sect++, i++) {
		struct dir_entry *entry;

		/* Read one sector */
		if (mmc_block_read(sector, cur_sect, 1)) {
			/* Unable to read rootdir sector. */
			SERIAL_PUTI(0x06);
			return -1;
		}

		/* Read all file handles from this sector */
		for (entry = (struct dir_entry *) sector, j = 0;
					j < FAT_BLOCK_SIZE / sizeof(struct dir_entry);
					entry++, j++) {
			char c;

			if (entry->attr & (ATTR_VOLUME | ATTR_DIR))
				continue;

			if (entry->name[0] == 0xe5)
				continue;

			if (!entry->name[0]) {
				/* Kernel file not found. */
				SERIAL_PUTI(0x07);
				return -1;
			}

			if (strncmp(entry->name, name, name_len) ||
						strncmp(entry->ext, ext, ext_len))
				continue;

			c = entry->name[name_len];
			if (c && c != ' ')
				continue;

			SERIAL_PUTS("MMC: Loading kernel file...\n");
			return load_from_cluster(lba,
						entry->starthi << 16
						| entry->start, ld_addr);
		}
	}

	/* Kernel file not found. */
	SERIAL_PUTI(0x07);
	return -1;
}

int mmc_load_kernel(void *ld_addr, const char *name, const char *ext)
{
	uint32_t lba;
	int err;

	err = get_first_partition(&lba);
	if (err)
		return err;

	return load_kernel_lba(lba, ld_addr, name, ext);
}

