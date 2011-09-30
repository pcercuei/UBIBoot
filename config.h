#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * Failsafe SDRAM configuration values
 *
 * If you want to live on the edge, the Dingoo Hynix HY57V281620FTP-6
 * chips should work with these accoring to the datasheet:
 *
 *   TRAS 42
 *   RCD  18
 *   TPC  18
 *   
 */

#define SDRAM_CASL		2	/* CAS latency: 2 or 3 */
#define SDRAM_TRAS		45	/* RAS# Active Time (ns) */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay (ns) */
#define SDRAM_TPC		20	/* RAS# Precharge Time (ns) */
#define SDRAM_TRWL		7	/* Write Latency Time (ns) */
#define SDRAM_TREF	        15625	/* Refresh period (ns): 4096 refresh cycles/64ms */

#define CPU_ID				0x4740
#define CFG_EXTAL			12000000
#define CFG_CPU_SPEED		336000000
#define UART_BASE			UART0_BASE
#define CONFIG_BAUDRATE		57600
#define SDRAM_BW16			0
#define SDRAM_BANK4			1
#define SDRAM_ROW			12
#define SDRAM_COL			9
#define CONFIG_MOBILE_SDRAM	0
#define IS_SHARE			1

#define BOOT_SELECT_PIN		113	/* SELECT button */

/* cache parameters */
#define CFG_ICACHE_SIZE		0x4000
#define CFG_DCACHE_SIZE		0x4000
#define CFG_CACHELINE_SIZE	0x20

/* NAND flash parameters */
#define BUS_WIDTH	8
#define ROW_CYCLE	3
#define PAGE_SIZE	4096
#define OOB_SIZE 	(PAGE_SIZE/32)
#define PAGE_PER_BLOCK	128
#define BLOCK_SIZE 		(PAGE_PER_BLOCK * PAGE_SIZE)
#define BAD_BLOCK_POS	((PAGE_SIZE==512) ? 5 : 0)
#define BAD_BLOCK_PAGE	(PAGE_PER_BLOCK -1)
#define ECC_BLOCK	512
#define ECC_POS		4
#define PAR_SIZE	9
#define PAR_SIZE2	12

/* kernel loading parameters */
#define UBI_MTD_EB_START	5
#define UBI_MTD_NB_EB		127
#define LD_ADDR				0x80600000

#endif

