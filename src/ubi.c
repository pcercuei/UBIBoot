/*
 * Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <string.h>
#include <alloca.h>

#include "board.h"
#include "ubi.h"
#include "errorcodes.h"
#include "jz.h"
#include "nand.h"
#include "serial.h"
#include "config.h"
#include "utils.h"
#include "uimage.h"

#define BLOCK_SIZE (PAGE_PER_BLOCK * PAGE_SIZE)

SLIST_HEAD(EraseBlockList, EraseBlock);

static inline void *get_ptr(uint32_t eb_start, uint8_t *eb_copy, uint32_t page)
{
	page += eb_start * PAGE_PER_BLOCK;

	nand_read_page(page, eb_copy);

	return (void *)eb_copy;
}

static struct ubi_fm_volhdr *find_volume(struct ubi_fm_volhdr *vol_hdr_ptr,
		       unsigned int nb_volumes, unsigned int vol_id)
{
	unsigned int i;

	for (i = 0; i < nb_volumes; i++) {
		struct ubi_fm_eba *eba;
		uint32_t id;

		if (vol_hdr_ptr->magic != UBI_FM_VHDR_MAGIC)
			break;

		id = swap_be32(vol_hdr_ptr->vol_id);

		if (id == vol_id)
			return vol_hdr_ptr;

		eba = (void *)vol_hdr_ptr + sizeof(*vol_hdr_ptr);

		vol_hdr_ptr = (void *)eba
			    + sizeof(*eba)
			    + sizeof(u32) * swap_be32(eba->reserved_pebs);
	}

	return NULL;
}

static struct ubi_vid_hdr *get_vid_hdr(uint32_t peb, uint8_t *eb_copy,
				       uint32_t vol_id, uint32_t vid_hdr_page)
{
	struct ubi_vid_hdr *vid_hdr = get_ptr(peb, eb_copy, vid_hdr_page);

	if (vid_hdr->magic != UBI_VID_HDR_MAGIC)
		return NULL;

	if (swap_be32(vid_hdr->vol_id) != vol_id)
		return NULL;

	return vid_hdr;
}

static struct EraseBlock *get_eb(struct EraseBlockList *eb_list, uint32_t leb)
{
	struct EraseBlock *eb;

	SLIST_FOREACH(eb, eb_list, next) {
		if (eb->leb == leb)
			return eb;
	}

	return NULL;
}

static int load_kernel(uint32_t eb_start, uint32_t nb_ebs, uint8_t *ld_addr,
		       void **exec_addr, uint32_t kernel_vol_id)
{
	uint32_t i, fm_nb_pebs, fm_sb_eb, vol_count, leb_count,
		 data_page, vid_hdr_page;
	static uint8_t eb_copy[PAGE_SIZE];
	struct ubi_ec_hdr *ec_hdr;
	struct ubi_vid_hdr *vid_hdr;
	struct ubi_fm_sb *fm_sb;
	struct ubi_fm_hdr *fm_hdr;
	struct ubi_fm_volhdr *fm_vol_hdr, *kernel_vol_hdr, *vol_hdr_ptr;
	struct ubi_fm_eba *fm_eba;
	void *fastmap, *fastmap_ptr;
	unsigned int loaded = 0;
	bool fm_load_failed = false;

	struct EraseBlockList eb_list;
	SLIST_INIT(&eb_list);

	ec_hdr = get_ptr(eb_start, eb_copy, 0);

	if (ec_hdr->magic != UBI_EC_HDR_MAGIC) {
		SERIAL_ERR(ERR_UBI_NO_PART);
		return -1;
	}

	SERIAL_PUTS("UBI partition detected.\n");

	vid_hdr_page = swap_be32(ec_hdr->vid_hdr_offset) / PAGE_SIZE;
	data_page = swap_be32(ec_hdr->data_offset) / PAGE_SIZE;

	for (i = eb_start; i < (eb_start + UBI_FM_MAX_START); i++) {
		vid_hdr = get_vid_hdr(i, eb_copy, UBI_FM_SB_VOL_ID,
				      vid_hdr_page);
		if (vid_hdr) {
			fm_sb_eb = i;
			break;
		}
	}

	if (i == (eb_start + UBI_FM_MAX_START)) {
		SERIAL_ERR(ERR_UBI_NO_FASTMAP);
		return -1;
	}

	fm_sb = get_ptr(fm_sb_eb, eb_copy, data_page);

	if (fm_sb->magic != UBI_FM_SB_MAGIC) {
		SERIAL_ERR(ERR_UBI_IO);
		return -1;
	}

	fm_nb_pebs = swap_be32(fm_sb->used_blocks);

	/* Load the fastmap data in RAM */
	fastmap = (void *)(0xa0000000
			   + get_memory_size()
			   - BLOCK_SIZE * fm_nb_pebs);
	fastmap_ptr = fastmap;

	for (i = 0; i < fm_nb_pebs; i++) {
		uint32_t loc = eb_start + swap_be32(fm_sb->block_loc[i]);

		nand_load(loc * PAGE_PER_BLOCK + data_page,
			  PAGE_PER_BLOCK - data_page, fastmap_ptr);
		fastmap_ptr += BLOCK_SIZE - data_page * PAGE_SIZE;
	}

	fm_sb = fastmap;

	/* Check again, just in case */
	if (fm_sb->magic != UBI_FM_SB_MAGIC) {
		SERIAL_ERR(ERR_UBI_IO);
		return -1;
	}

	fm_hdr = (void *)fm_sb + sizeof(*fm_sb);

	if (fm_hdr->magic != UBI_FM_HDR_MAGIC) {
		SERIAL_ERR(ERR_UBI_IO);
		return -1;
	}

	vol_count = swap_be32(fm_hdr->vol_count);

	vol_hdr_ptr = (void *)fm_hdr + sizeof(*fm_hdr)
		+ 2 * sizeof(struct ubi_fm_scan_pool)
		+ swap_be32(fm_hdr->free_peb_count) * sizeof(struct ubi_fm_ec)
		+ swap_be32(fm_hdr->used_peb_count) * sizeof(struct ubi_fm_ec)
		+ swap_be32(fm_hdr->scrub_peb_count) * sizeof(struct ubi_fm_ec)
		+ swap_be32(fm_hdr->erase_peb_count) * sizeof(struct ubi_fm_ec);

	fm_vol_hdr = find_volume(vol_hdr_ptr, vol_count, UBI_VOL_TABLE_ID);
	if (!fm_vol_hdr) {
		SERIAL_ERR(ERR_UBI_IO);
		return -1;
	}

	fm_eba = (void *)fm_vol_hdr + sizeof(*fm_vol_hdr);
	if (!fm_eba->reserved_pebs) {
		SERIAL_ERR(ERR_UBI_IO);
		return -1;
	}

	kernel_vol_hdr = find_volume(vol_hdr_ptr, vol_count, kernel_vol_id);
	if (!kernel_vol_hdr) {
		SERIAL_ERR(ERR_UBI_NO_KERNEL);
		return -1;
	}

	leb_count = swap_be32(kernel_vol_hdr->used_ebs);

	fm_eba = (void *)kernel_vol_hdr + sizeof(*kernel_vol_hdr);

	for (i = 0; i < leb_count; i++) {
		uint32_t peb = swap_be32(fm_eba->pnum[i]);
		struct EraseBlock *eb;

		if (peb == 0xffffffff)
			break;

		if (!get_vid_hdr(peb, eb_copy, kernel_vol_id, vid_hdr_page)) {
			fm_load_failed = true;
			break;
		}

		eb = alloca(sizeof(*eb));
		eb->peb = peb;
		eb->leb = i;
		SLIST_INSERT_HEAD(&eb_list, eb, next);
		loaded++;
	}

	if (fm_load_failed) {
		SERIAL_PUTS("Fastmap loading failed. Doing full scan.\n");
		SLIST_INIT(&eb_list);
		loaded = 0;

		/* Retry, with a full scan this time */
		for (i = eb_start; i < (eb_start + nb_ebs); i++) {
			struct EraseBlock *eb;
			uint32_t leb, peb = eb_start + i;
			uint64_t sqnum;

			vid_hdr = get_vid_hdr(peb, eb_copy, kernel_vol_id,
					      vid_hdr_page);
			if (!vid_hdr)
				continue;

			leb = swap_be32(vid_hdr->lnum);
			sqnum = swap_be64(vid_hdr->sqnum);

			eb = get_eb(&eb_list, leb);
			if (eb) {
				/* EB is already in the list. Use the sqnum
				 * field to find the newest */

				if (sqnum > eb->sqnum) {
					eb->peb = peb;
					eb->sqnum = sqnum;
				}

				continue;
			}

			eb = alloca(sizeof(*eb));
			eb->peb = peb;
			eb->leb = leb;
			eb->sqnum = sqnum;
			SLIST_INSERT_HEAD(&eb_list, eb, next);
			loaded++;
		}
	}

	for (i = 0; i < loaded; i++) {
		unsigned int page_addr, nb_pages = PAGE_PER_BLOCK - data_page;
		struct EraseBlock *eb = get_eb(&eb_list, i);

		if (!eb) {
			SERIAL_ERR(ERR_UBI_IO);
			return -1;
		}

		page_addr = eb->peb * PAGE_PER_BLOCK + data_page;

		if (i == 0) {
			nand_read_page(page_addr, ld_addr);

			ld_addr = process_uimage_header((void *)ld_addr,
					exec_addr, PAGE_SIZE);
			if (!ld_addr) {
				SERIAL_ERR(ERR_FAT_BAD_IMAGE);
				return -1;
			}

			nb_pages--;
			page_addr++;
		}

		nand_load(page_addr, nb_pages, ld_addr);
		ld_addr += nb_pages * PAGE_SIZE;
	}

	return 0;
}

int ubi_load_kernel(unsigned char *ld_addr, void **exec_addr, uint32_t vol_id)
{
	return load_kernel(UBI_MTD_EB_START, UBI_MTD_NB_EB,
			   ld_addr, exec_addr, vol_id);
}
