/* Board-specific config for GCW Zero. */

#ifndef _CONFIG_H
#error Include "config.h" instead
#endif

#define CFG_CPU_SPEED		1020000000
#define CFG_EXTAL			12000000

#include "jz4770.h"

/* serial parameters */
#define UART_BASE			UART2_BASE
#define CONFIG_BAUDRATE		57600
