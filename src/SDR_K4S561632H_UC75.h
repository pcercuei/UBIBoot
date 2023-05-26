#pragma once

#ifndef __SDRAM_CONFIG_H
#error Include "sdram.h" instead
#endif

#define SDRAM_CAS_LATENCY    3 /* 2 and 3 are supported */
#define SDRAM_TRAS          45 /* RAS# Active Time (ns) */
#define SDRAM_TRCD          20 /* RAS# to CAS# Delay (ns) */
#define SDRAM_TRP           20 /* RAS# Precharge Time (ns) */
#define SDRAM_TWR            2 /* Write Recovery Time (cycles) also called as tDPL, tRWL, tRDL  */
#define SDRAM_REFRESH 64000000 /* Full chip refresh period (ns) */
#define SDRAM_BANKS          4 /* Number of bank select signals per one chip select: 2 or 4 */
#define SDRAM_ROW           13 /* Row Address Width: 11-13 */
#define SDRAM_COL            9 /* Column Address Width: 8-12 */
