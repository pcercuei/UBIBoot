
#ifndef MMC_H
#define MMC_H

#include <stdlib.h>
#include <stdint.h>

#define MMC_SECTOR_SIZE 512

int mmc_init(unsigned int id);
int mmc_block_read(unsigned int id, uint32_t *dst,
			uint32_t src, uint32_t num_blocks);

#endif
