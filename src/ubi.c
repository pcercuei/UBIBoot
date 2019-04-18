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
	uint32_t i, data_page, vid_hdr_page;
	static uint8_t eb_copy[PAGE_SIZE];
	struct ubi_ec_hdr *ec_hdr;
	struct ubi_vid_hdr *vid_hdr;
	unsigned int loaded = 0;

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
