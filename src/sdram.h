#ifndef __SDRAM_CONFIG_H
#define __SDRAM_CONFIG_H

#if defined(SDRAM_H5PS1G63EFR_S6C)
#include "DDR2_H5PS1G63EFR-S6C.h"
#elif defined(SDRAM_H5PS1G63EFR_Y5C)
#include "DDR2_H5PS1G63EFR-Y5C.h"
#elif defined(SDRAM_MT47H128M16RT_25E)
#include "DDR2_MT47H128M16RT-25E.h"
#elif defined(SDRAM_MT46H128M16LFCK_6)
#include "MDDR_MT46H128M16LFCK-6.h"
#elif defined(SDRAM_W972GG6KB_25I)
#include "DDR2_W972GG6KB-25I.h"
#elif defined(SDRAM_H2A35121656BB6C)
#include "DDR2_H2A35121656BB6C.h"
#elif defined(SDRAM_H2A301G1656B)
#include "DDR2_H2A301G1656B.h"

#else
#error SDRAM type not specified or unknown; check config-<device>.mk
#endif

#endif // __SDRAM_CONFIG_H
