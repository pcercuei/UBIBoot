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
#include "nand.h"
#include "serial.h"
#include "config.h"

static int load_kernel(uint32_t eb_start, uint32_t count,
			unsigned char *ld_addr)
{
	struct ubi_ec_hdr ec_hdr;
	struct ubi_vid_hdr vid_hdr;
	unsigned char eb_copy[PAGE_SIZE];
	uint32_t i, vid_hdr_offset, kernel_vol_id = 0xffffffff;

	SLIST_HEAD(EraseBlockList, EraseBlock) eb_list[UBI_NB_VOLUMES];

	for (i=0; i<UBI_NB_VOLUMES; i++) SLIST_INIT(&eb_list[i]);

	nand_read_page(eb_start * BLOCK_SIZE, eb_copy);
	memcpy(&ec_hdr, eb_copy, sizeof(struct ubi_ec_hdr));

	if (ec_hdr.magic != UBI_EC_HDR_MAGIC) {
		SERIAL_PUTS("No UBI partition detected!\n");
		return -1;
	}

	SERIAL_PUTS("UBI partition detected.\n");

	vid_hdr_offset = be32toh(ec_hdr.vid_hdr_offset);

	for (i=eb_start; i<(eb_start+count); i++) {
		nand_read_page(i * BLOCK_SIZE + vid_hdr_offset, eb_copy);
		memcpy(&vid_hdr, eb_copy + (vid_hdr_offset % PAGE_SIZE), sizeof(struct ubi_vid_hdr));

		if (vid_hdr.magic == UBI_VID_HDR_MAGIC) {
			struct EraseBlock *eb = alloca(sizeof(struct EraseBlock));
			memcpy(&eb->vid_hdr, &vid_hdr, sizeof(struct ubi_vid_hdr));
			eb->data_addr = i*BLOCK_SIZE + be32toh(ec_hdr.data_offset);

			/* This eraseblock contains the volume table */
			if (be32toh(vid_hdr.vol_id) == UBI_VOL_TABLE_ID) {

				/* Skip if we have already read the volume table */
				if (kernel_vol_id < UBI_VOL_TABLE_ID) continue;

				struct ubi_vol_tbl_record records[UBI_NB_VOLUMES + PAGE_SIZE/sizeof(struct ubi_vol_tbl_record)];
				uint32_t nb;
				nand_load(eb->data_addr, 1+(UBI_NB_VOLUMES*sizeof(struct ubi_vol_tbl_record))/PAGE_SIZE, (uint8_t*) &records);

				for (nb=0; nb<UBI_NB_VOLUMES; nb++) {
					if (!records[nb].name[0]) continue;

					SERIAL_PUTS("Found volume: ");
					SERIAL_PUTS((const char*)records[nb].name);
					SERIAL_PUTC('\n');

					if (!strncmp((const char*)records[nb].name, UBI_KERNEL_VOLUME,
									sizeof(UBI_KERNEL_VOLUME))) {
						kernel_vol_id = nb;
					}
				}
			} else {
				SLIST_INSERT_HEAD(&eb_list[be32toh(vid_hdr.vol_id)], eb, next);
			}
		}
	}

	if (kernel_vol_id >= UBI_VOL_TABLE_ID) {
		SERIAL_PUTS("ERROR: Unable to locate kernel partition.\n");
		return -1;
	}

	for (i=0; ; i++) {
		struct EraseBlock *eb;
		int found=0;
		SLIST_FOREACH(eb, &eb_list[kernel_vol_id], next) {
			if (be32toh(eb->vid_hdr.lnum) == i) {
				nand_load(eb->data_addr, 1 + (be32toh(eb->vid_hdr.data_size) / PAGE_SIZE), ld_addr);
				ld_addr += be32toh(eb->vid_hdr.data_size);
				found = 1;
				break;
			}
		}
		if (!found) break;
	}

	return 0;
}

int ubi_load_kernel(unsigned char *ld_addr)
{
	return load_kernel(UBI_MTD_EB_START, UBI_MTD_NB_EB, ld_addr);
}
