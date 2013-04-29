#ifndef __SDRAM_CONFIG_H
#error Include "sdram.h" instead
#endif

#define CONFIG_SDRAM_MDDR

/* Chip Select */
#define DDR_CS1EN  0 /* CSEN : whether a ddr chip exists 0 - un-used, 1 - used */
#define DDR_CS0EN  1
#define DDR_DW32   1 /* 0 - 16-bit data width, 1 - 32-bit data width */

/* DDR parameters */
#define DDR_ROW   14 /* ROW : 12 to 14 row address */
#define DDR_COL   11 /* COL :  8 to 10 column address */
#define DDR_BANK8  0 /* Banks each chip: 0-4bank, 1-8bank */
#define DDR_CL     3 /* CAS latency: 1 to 5 */

/*
 * DDR controller timing1 register, unit: ps
 */
#define DDR_tRAS  42000  /* ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP  12000  /* READ to PRECHARGE command period. */
#define DDR_tRP   18000  /* PRECHARGE command period to the same bank */
#define DDR_tRCD  18000  /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC   60000  /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD  12000  /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR   15000  /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR      1  /* tCK, WRITE to READ command delay. */

/*
 * DDR controller timing2 register
 */
#define DDR_tRFC    72000  /* ps,  AUTO-REFRESH command period. */
#define DDR_tMINSR    113 /* tCK, Minimum Self-Refresh / Deep-Power-Down */
#define DDR_tXP         1  /* tCK, EXIT-POWER-DOWN to next valid command period: 1 to 8 tCK. */
#define DDR_tMRD        2  /* tCK, Load-Mode-Register to next valid command period: 1 to 4 tCK */

/*
 * DDR controller mdelay register
 */
#define DDR_TSEL   2  /* PHY to DDR transfer delay */
#define DDR_MSEL   1  /* Delay DQS by [0-3] ticks */
#define DDR_HL     1  /* Delay reduced by [0-1] 1/2 ticks */
#define DDR_QUAR   0  /* Delay augmented by [0-1] 1/4 ticks */

#if (DDR_TSEL == 3) && (DDR_CL == 5)
#	error "TSEL=3 is only valid when CL<5"
#endif

/*
 * DDR controller refcnt register
 */
#define DDR_tREFI	        7800	/* Refresh period: ns */

#define DDR_CLK_DIV 1    /* Clock Divider. auto refresh
						  *	cnt_clk = memclk/(16*(2^DDR_CLK_DIV))
						  */
