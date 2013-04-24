#ifndef __SDRAM_CONFIG_H
#define __SDRAM_CONFIG_H

#if defined(SDRAM_H5PS1G63EFR_S6C)
#include "DDR2_H5PS1G63EFR-S6C.h"
#else
#error SDRAM type not specified or unknown; check config-<device>.mk
#endif

#endif // __SDRAM_CONFIG_H
