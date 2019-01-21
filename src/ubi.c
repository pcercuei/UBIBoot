/*
 * Copyright (C) 2011 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <string.h>
#include <alloca.h>

#include "ubi.h"
#include "errorcodes.h"
#include "nand.h"
#include "serial.h"
#include "config.h"
#include "utils.h"

#define BLOCK_SIZE (PAGE_PER_BLOCK * PAGE_SIZE)

static int load_kernel(uint32_t eb_start, uint32_t count,
			unsigned char *ld_addr)
{
	struct ubi_ec_hdr *ec_hdr;
	struct ubi_vid_hdr *vid_hdr;
	unsigned char eb_copy[PAGE_SIZE];
	uint32_t i, vid_hdr_offset, data_offset, kernel_vol_id = 0xffffffff;

	SLIST_HEAD(EraseBlockList, EraseBlock) eb_list;
	SLIST_INIT(&eb_list);

	nand_read_page(eb_start * PAGE_PER_BLOCK, eb_copy);
	ec_hdr = (struct ubi_ec_hdr *) eb_copy;

	if (ec_hdr->magic != UBI_EC_HDR_MAGIC) {
		SERIAL_ERR(ERR_UBI_NO_PART);
		return -1;
	}

	SERIAL_PUTS("UBI partition detected.\n");

	vid_hdr_offset = swap_be32(ec_hdr->vid_hdr_offset);
	data_offset = swap_be32(ec_hdr->data_offset);

	for (i=eb_start; i<(eb_start+count); i++) {
		nand_read_page(i * PAGE_PER_BLOCK + vid_hdr_offset / PAGE_SIZE, eb_copy);

		vid_hdr = (struct ubi_vid_hdr *) ((uintptr_t) eb_copy + (vid_hdr_offset % PAGE_SIZE));

		if (vid_hdr->magic != UBI_VID_HDR_MAGIC)
			continue;

		if (swap_be32(vid_hdr->vol_id) != UBI_VOL_TABLE_ID)
			continue;

		struct ubi_vol_tbl_record records[UBI_NB_VOLUMES + PAGE_SIZE/sizeof(struct ubi_vol_tbl_record)];
		uint32_t nb;
		nand_load((i * BLOCK_SIZE + data_offset) / PAGE_SIZE, 1+(UBI_NB_VOLUMES*sizeof(struct ubi_vol_tbl_record))/PAGE_SIZE, (uint8_t*) &records);

		for (nb=0; nb<UBI_NB_VOLUMES; nb++) {
			if (!records[nb].name[0]) continue;

			if (!strncmp((const char*)records[nb].name, UBI_KERNEL_VOLUME,
							sizeof(UBI_KERNEL_VOLUME))) {
				kernel_vol_id = nb;
				break;
			}
		}

		break;
	}

	if (kernel_vol_id >= UBI_VOL_TABLE_ID) {
		SERIAL_ERR(ERR_UBI_NO_KERNEL);
		return -1;
	}

	for (i=eb_start; i<(eb_start+count); i++) {
		nand_read_page(i * PAGE_PER_BLOCK + vid_hdr_offset / PAGE_SIZE, eb_copy);

		vid_hdr = (struct ubi_vid_hdr *) ((uintptr_t) eb_copy + (vid_hdr_offset % PAGE_SIZE));

		if (vid_hdr->magic != UBI_VID_HDR_MAGIC)
			continue;

		if (swap_be32(vid_hdr->vol_id) != kernel_vol_id)
			continue;

		struct EraseBlock *eb = alloca(sizeof(struct EraseBlock));

		eb->data_addr = i*BLOCK_SIZE + data_offset;
		eb->lnum = swap_be32(vid_hdr->lnum);
		eb->data_size = swap_be32(vid_hdr->data_size);

		SLIST_INSERT_HEAD(&eb_list, eb, next);
	}

	for (i = 0; ; i++) {
		struct EraseBlock *eb;
		int found = 0;

		SLIST_FOREACH(eb, &eb_list, next) {
			unsigned int nb_pages;

			found = eb->lnum == i;
			if (!found)
				continue;

			nb_pages = div_round_up(eb->data_size, PAGE_SIZE);
			nand_load(eb->data_addr / PAGE_SIZE, nb_pages, ld_addr);
			ld_addr += eb->data_size;

			break;
		}

		if (!found)
			break;
	}

	return 0;
}

int ubi_load_kernel(unsigned char *ld_addr)
{
	return load_kernel(UBI_MTD_EB_START, UBI_MTD_NB_EB, ld_addr);
}
