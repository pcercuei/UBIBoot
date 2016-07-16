
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "jz.h"
#include "serial.h"
#include "mmc.h"
#include "fat.h"
#include "errorcodes.h"
#include "uimage.h"
#include "utils.h"

uint32_t lba_fat1;			/* sector of first FAT */
uint32_t lba_data;			/* sector of first cluster */
uint32_t root_cluster;		/* cluster where root dir starts */
uint8_t cluster_size;		/* sectors per cluster */

static int get_first_partition(unsigned int id, uint32_t *lba)
{
#ifdef MBR_PRELOAD_ADDR
	struct mbr *mbr = (struct mbr *) MBR_PRELOAD_ADDR;
#else
	uint8_t mbr_data[MMC_SECTOR_SIZE];
	struct mbr *mbr = (struct mbr *) &mbr_data;

	if (mmc_block_read(id, (uint32_t *) mbr, 0, 1)) {
		SERIAL_PUTI(ERR_FAT_IO_BOOT);
		return -1;
	}
#endif

	if (mbr->signature != 0xAA55) {
		SERIAL_PUTI(ERR_FAT_NO_MBR);
		return -1;
	}

	if (mbr->partitions[0].status && mbr->partitions[0].status != 0x80) {
		SERIAL_PUTI(ERR_FAT_NO_PART);
		return -1;
	}

	*lba =  mbr->partitions[0].lba;
	return 0;
}

static int process_boot_sector(unsigned int id, uint32_t lba)
{
	uint32_t sector[FAT_BLOCK_SIZE >> 2];
	struct boot_sector *bs;
	struct volume_info *vinfo;

	if (mmc_block_read(id, sector, lba, 1)) {
		SERIAL_PUTI(ERR_FAT_IO_PART);
		return -1;
	}

	bs = (void *)sector;
	lba_fat1 = lba + bs->reserved;
	lba_data = lba_fat1 + bs->fat32_length * bs->fats;
	root_cluster = bs->root_cluster;
	cluster_size = bs->cluster_size;

	vinfo = (void *) sector + sizeof(struct boot_sector);
	if (strncmp(vinfo->fs_type, "FAT32", 5)) {
		SERIAL_PUTI(ERR_FAT_NO_FAT32);
		return -1;
	}

	SERIAL_PUTS("MMC: FAT32 filesystem detected.\n");
	return 0;
}

/*
 * Given a cluster, follow the cluster chain while the cluster numbers are
 * consecutive. Outputs the next cluster number and the number of clusters
 * advanced along the chain.
 * Since every MMC command has a significant overhead, loading more data
 * at once gives a big speed boost.
 */
static int cluster_span(
		unsigned int id, uint32_t cluster, uint32_t *next, uint32_t *count)
{
	uint32_t sector[FAT_BLOCK_SIZE >> 2];
	uint32_t cached_fat_sector = -1;
	uint32_t start_cluster = cluster;

	while (1) {
		uint32_t fat_sector = lba_fat1 + cluster / (FAT_BLOCK_SIZE >> 2);

		/* Read FAT */
		if (fat_sector != cached_fat_sector) {
			if (mmc_block_read(id, sector, fat_sector, 1)) {
				SERIAL_PUTI(ERR_FAT_IO_FAT);
				return -1;
			}
			cached_fat_sector = fat_sector;
		}

		uint32_t prev_cluster = cluster;
		cluster = sector[cluster % (FAT_BLOCK_SIZE >> 2)] & 0x0fffffff;
		if (cluster != prev_cluster + 1) {
			*next = cluster;
			*count = prev_cluster + 1 - start_cluster;
			return 0;
		}
	}
}

static int check_uimage(struct uimage_header *header)
{
	if (swap_be32(header->magic) != UIMAGE_MAGIC)
		return -1;

	if (header->os != UIMAGE_OS_LINUX)
		return -1;

	if (header->arch != UIMAGE_ARCH_MIPS)
		return -1;

	if (header->type != UIMAGE_TYPE_KERNEL)
		return -1;

	if (header->comp != UIMAGE_COMP_NONE)
		return -1;

	return 0;
}

static void *process_uimage_header(
		struct uimage_header *header, void **exec_addr)
{
	if (check_uimage(header)) {
		return NULL;
	} else {
		void *ld_addr = (void *) KSEG1ADDR(swap_be32(header->load));
		void *body = (void *) header + sizeof(struct uimage_header);
		size_t move_size = MMC_SECTOR_SIZE - sizeof(struct uimage_header);
		*exec_addr = (void *) swap_be32(header->ep);
		memmove(ld_addr, body, move_size);
		return ld_addr + move_size;
	}
}

/*
 * Loads data from the cluster chain starting at the given cluster number.
 * When 'exec_addr' is not NULL, it indicates that an uImage is being loaded
 * and the execution address (entry point) should be extracted from the
 * uImage header and written via that pointer. Also the image body should
 * be loaded to the load address from the uImage header instead of 'ld_addr'.
 */
static void *load_cluster_chain(unsigned int id, uint32_t cluster,
		void *ld_addr, void **exec_addr)
{
	int err = ERR_FAT_BAD_IMAGE;

	while (cluster > 1 && cluster < 0x0ffffff0) {
		uint32_t data_sector, num_data_sectors;
		uint32_t next_cluster, num_clusters;

		if (cluster_span(id, cluster, &next_cluster, &num_clusters))
			return NULL;

		/* Start read command. */
		data_sector = lba_data + (cluster - 2) * cluster_size;
		num_data_sectors = num_clusters * cluster_size;
		mmc_start_block(id, data_sector, num_data_sectors);

		/* Receive data. */
		err = 0;
		while (num_data_sectors--) {
			if (mmc_receive_block(id, ld_addr)) {
				err = ERR_FAT_IO_PART;
				break;
			}
			if (exec_addr) {
				ld_addr = process_uimage_header(ld_addr, exec_addr);
				if (!ld_addr) {
					err = ERR_FAT_BAD_IMAGE;
					break;
				}
				exec_addr = NULL;
			} else {
				ld_addr += MMC_SECTOR_SIZE;
			}
		}

		mmc_stop_block(id);

		if (err)
			break;

		cluster = next_cluster;
	}

	if (err) {
		SERIAL_PUTI(err);
		return NULL;
	} else {
		return ld_addr;
	}
}

static struct dir_entry *find_file(
		struct dir_entry *first, struct dir_entry *end, const char *name)
{
	struct dir_entry *entry;

	for (entry = first; entry != end && entry->name[0]; entry++) {

		if (entry->attr & (ATTR_VOLUME | ATTR_DIR))
			continue;

		/*
		 * Entries starting with 0xE5 are deleted and should be ignored,
		 * but they won't match the name we're searching for anyway.
		 */

		if (!strncmp(entry->name, name, 8 + 3))
			return entry;
	}

	return NULL;
}

static const char *kernel_names[] = {
	FAT_BOOTIMAGE_NAME,
	FAT_BOOTFILE_NAME,
	FAT_BOOTIMAGE_ALT_NAME,
	FAT_BOOTFILE_ALT_NAME,
};

int mmc_load_kernel(unsigned int id, void *ld_addr, int alt, void **exec_addr)
{
	struct dir_entry *dir_start, *dir_end;
	uint32_t lba;
	int err, i;

	err = get_first_partition(id, &lba);
	if (err)
		return err;

	err = process_boot_sector(id, lba);
	if (err)
		return err;

	dir_start = NULL;
	err = 0;
	for (i = 0; i < 4; i++) {
		const int kernel = i ^ (alt ? 2 : 0);
		struct dir_entry *entry;

		if (!dir_start) {
			/* Load root directory. */
			dir_start = ld_addr;
			dir_end = load_cluster_chain(id, root_cluster, dir_start, NULL);
			if (!dir_end)
				return -1;
		}

		entry = find_file(dir_start, dir_end, kernel_names[kernel]);

		if (entry) {
			dir_start = NULL;
			*exec_addr = ld_addr;

			SERIAL_PUTS("MMC: Loading kernel file ");
			SERIAL_PUTS(kernel_names[kernel]);
			SERIAL_PUTC('\n');

			if (load_cluster_chain(
						id, entry->starthi << 16 | entry->start,
						ld_addr, (kernel & 1) ? NULL : exec_addr))
				return kernel >> 1;
			err = -1;
		}
	}

	if (err) {
		return err;
	} else {
		SERIAL_PUTI(ERR_FAT_NO_KERNEL);
		return -1;
	}
}
