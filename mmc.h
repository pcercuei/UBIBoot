
#ifndef MMC_H
#define MMC_H

#include <stdlib.h>
#include <stdint.h>

#define MMC_SECTOR_SIZE 512

int mmc_init(void);
int mmc_block_read(uint8_t *dst, uint32_t src, size_t nb);

#endif
