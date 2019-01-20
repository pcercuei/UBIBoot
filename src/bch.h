/*
 * Copyright (C) 2019 Paul Cercueil <paul@crapouillou.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __UBIBOOT_BCH_H__
#define __UBIBOOT_BCH_H__

#include <stdint.h>

int bch_correct_block(uint8_t *dst, uint8_t *oobbuf);

#endif /* __UBIBOOT_BCH_H__ */
