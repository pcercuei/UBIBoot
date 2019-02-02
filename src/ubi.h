
#ifndef UBI_H
#define UBI_H

#include <stdint.h>
#include <sys/queue.h>
#include <arpa/inet.h>

#define UBI_EC_HDR_MAGIC	ntohl(0x55424923)
#define UBI_VID_HDR_MAGIC	ntohl(0x55424921)
#define UBI_FM_SB_MAGIC		ntohl(0x7B11D69F)
#define UBI_FM_HDR_MAGIC	ntohl(0xD4B82EF7)
#define UBI_FM_VHDR_MAGIC	ntohl(0xFA370ED1)
#define UBI_FM_POOL_MAGIC	ntohl(0x67AF4D08)
#define UBI_FM_EBA_MAGIC	ntohl(0xf0c040a8)

#define UBI_VOL_TABLE_ID	0x7fffefff

#define UBI_FM_SB_VOL_ID	0x7ffff000
#define UBI_FM_DATA_VOL_ID	0x7ffff001

/* The maximum volume name length */
#define UBI_VOL_NAME_MAX 127

/* A fastmap super block can be located between PEB 0 and
 * UBI_FM_MAX_START */
#define UBI_FM_MAX_START	64

/* A fastmap can use up to UBI_FM_MAX_BLOCKS PEBs */
#define UBI_FM_MAX_BLOCKS	32

/* 5% of the total number of PEBs have to be scanned while attaching
 * from a fastmap.
 * But the size of this pool is limited to be between UBI_FM_MIN_POOL_SIZE and
 * UBI_FM_MAX_POOL_SIZE */
#define UBI_FM_MIN_POOL_SIZE	8
#define UBI_FM_MAX_POOL_SIZE	256

struct ubi_ec_hdr {
	uint32_t magic;
	uint8_t version;
	uint8_t padding[3];
	uint64_t ec;
	uint32_t vid_hdr_offset;
	uint32_t data_offset;
	uint32_t image_seq;
	uint8_t padding2[32];
	uint32_t hdr_crc;
} __attribute__ ((packed));

struct ubi_vid_hdr {
	uint32_t magic;
	uint8_t version;
	uint8_t vol_type;
	uint8_t copy_flag;
	uint8_t compat;
	uint32_t vol_id;
	uint32_t lnum;
	uint32_t leb_ver;
	uint32_t data_size;
	uint32_t used_ebs;
	uint32_t data_pad;
	uint32_t data_crc;
	uint8_t padding1[4];
	uint64_t sqnum;
	uint8_t padding2[12];
	uint32_t hdr_crc;
} __attribute__ ((packed));

struct ubi_vol_tbl_record {
	uint32_t reserved_pebs;
	uint32_t alignment;
	uint32_t data_pad;
	uint8_t vol_type;
	uint8_t padding1;
	uint16_t name_len;
	uint8_t name[UBI_VOL_NAME_MAX + 1];
	uint8_t padding2[24];
	uint32_t crc;
} __attribute__ ((packed));

struct ubi_fm_sb {
	uint32_t magic;
	uint8_t version;
	uint8_t padding1[3];
	uint32_t data_crc;
	uint32_t used_blocks;
	uint32_t block_loc[UBI_FM_MAX_BLOCKS];
	uint32_t block_ec[UBI_FM_MAX_BLOCKS];
	uint64_t sqnum;
	uint8_t padding2[32];
} __attribute__ ((packed));

struct ubi_fm_hdr {
	uint32_t magic;
	uint32_t free_peb_count;
	uint32_t used_peb_count;
	uint32_t scrub_peb_count;
	uint32_t bad_peb_count;
	uint32_t erase_peb_count;
	uint32_t vol_count;
	uint8_t padding[4];
} __attribute__ ((packed));

struct ubi_fm_scan_pool {
	uint32_t magic;
	uint16_t size;
	uint16_t max_size;
	uint32_t pebs[UBI_FM_MAX_POOL_SIZE];
	uint32_t padding[4];
} __attribute__ ((packed));

struct ubi_fm_ec {
	uint32_t pnum;
	uint32_t ec;
} __attribute__ ((packed));

struct ubi_fm_volhdr {
	uint32_t magic;
	uint32_t vol_id;
	uint8_t vol_type;
	uint8_t padding1[3];
	uint32_t data_pad;
	uint32_t used_ebs;
	uint32_t last_eb_bytes;
	uint8_t padding2[8];
} __attribute__ ((packed));

struct ubi_fm_eba {
	uint32_t magic;
	uint32_t reserved_pebs;
	uint32_t pnum[0];
} __attribute__ ((packed));

struct EraseBlock {
	uint32_t data_addr;
	uint32_t lnum;
	uint32_t data_size;
	SLIST_ENTRY(EraseBlock) next;
};

int ubi_load_kernel(unsigned char *ld_addr, void **exec_addr, uint32_t vol_id);

#endif /* UBI_H */

