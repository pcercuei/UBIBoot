/*
 * Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <endian.h>
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

#define volume_name(id) \
    (id ? UBI_KERNEL_BAK_VOLUME : UBI_KERNEL_VOLUME)
#define volume_name_len(id) \
    (id ? sizeof(UBI_KERNEL_BAK_VOLUME) : sizeof(UBI_KERNEL_VOLUME))

SLIST_HEAD(EraseBlockList, EraseBlock);

static inline void *get_ptr(uint32_t eb_start, uint8_t *eb_copy, uint32_t page)
{
	page += eb_start * PAGE_PER_BLOCK;

	nand_read_page(page, eb_copy);

	return (void *)eb_copy;
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

static uint32_t get_kernel_vol_id(uint32_t eb, uint32_t data_page,
				  uint32_t nb_volumes, unsigned int id)
{
	unsigned long page = eb * PAGE_PER_BLOCK + data_page;
	struct ubi_vol_tbl_record *records;
	unsigned int nb = div_round_up(nb_volumes * sizeof(*records),
				       PAGE_SIZE);
	unsigned int i;

	records = alloca(PAGE_SIZE * nb);
	nand_load(page, nb, (uint8_t *)records);

	for (i = 0; i < nb_volumes; i++) {
		if (!records[i].name[0])
			continue;

		if (!strncmp((const char *)records[i].name,
			     volume_name(id), volume_name_len(id))) {
			return i;
		}
	}

	return (uint32_t)-1;
}

static int load_kernel(uint32_t eb_start, uint32_t nb_ebs, uint8_t *ld_addr,
		       void **exec_addr, unsigned int kernel_volume)
{
	uint32_t i, data_page, vid_hdr_page, kernel_vol_id;
	static uint8_t eb_copy[PAGE_SIZE];
	struct ubi_ec_hdr *ec_hdr;
	struct ubi_vid_hdr *vid_hdr;
	unsigned int loaded[3] = { 0, 0, 0 };

	struct EraseBlockList eb_list[3];

	for (i = 0; i < ARRAY_SIZE(eb_list); i++)
		SLIST_INIT(&eb_list[i]);

	ec_hdr = get_ptr(eb_start, eb_copy, 0);

	if (ec_hdr->magic != UBI_EC_HDR_MAGIC) {
		SERIAL_ERR(ERR_UBI_NO_PART);
		return -1;
	}

	SERIAL_PUTS("UBI partition detected.\n");

	vid_hdr_page = be32toh(ec_hdr->vid_hdr_offset) / PAGE_SIZE;
	data_page = be32toh(ec_hdr->data_offset) / PAGE_SIZE;

	for (i = eb_start; i < (eb_start + nb_ebs); i++) {
		uint32_t vol_id;
		struct EraseBlock *eb;
		uint32_t leb, peb = eb_start + i;
		uint64_t sqnum;

		vid_hdr = get_ptr(peb, eb_copy, vid_hdr_page);

		if (vid_hdr->magic != UBI_VID_HDR_MAGIC)
			continue;

		vol_id = be32toh(vid_hdr->vol_id);
		switch(vol_id) {
		case 0:
		case 1:
			break;
		case UBI_VOL_TABLE_ID:
			vol_id = 2;
			break;
		default:
			continue;
		}

		leb = be32toh(vid_hdr->lnum);
		sqnum = be64toh(vid_hdr->sqnum);

		eb = get_eb(&eb_list[vol_id], leb);
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
		eb->vol_id = vol_id;
		SLIST_INSERT_HEAD(&eb_list[vol_id], eb, next);
		loaded[vol_id]++;
	}

	if (loaded[2] == 0) {
		SERIAL_ERR(ERR_UBI_IO);
		return -1;
	}

	kernel_vol_id = get_kernel_vol_id(SLIST_FIRST(&eb_list[2])->peb,
					  data_page, 8, kernel_volume);
	if (kernel_vol_id >= UBI_VOL_TABLE_ID) {
               SERIAL_ERR(ERR_UBI_NO_KERNEL);
			   return -1;
	}

	SERIAL_PUTS("Requested volume ");
	SERIAL_PUTS(volume_name(kernel_volume));
	SERIAL_PUTS_ARGI(" was found at ID ", kernel_vol_id, ".\n");

	for (i = 0; i < loaded[kernel_vol_id]; i++) {
		unsigned int page_addr, nb_pages = PAGE_PER_BLOCK - data_page;
		struct EraseBlock *eb = get_eb(&eb_list[kernel_vol_id], i);

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
