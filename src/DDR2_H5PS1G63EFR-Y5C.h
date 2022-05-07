#ifndef __SDRAM_CONFIG_H
#error Include "sdram.h" instead
#endif

#define CONFIG_SDRAM_DDR2
#define CONFIG_DDR2_DIFFERENTIAL

/* Chip Select */
#define DDR_CS1EN  0 /* CSEN : whether a ddr chip exists 0 - un-used, 1 - used */
#define DDR_CS0EN  1
#ifndef DDR_DW32
#define DDR_DW32   1 /* 0 - 16-bit data width, 1 - 32-bit data width */
#endif

/* DDR2 parameters */
#define DDR_ROW   13 /* ROW : 12 to 14 row address */
#define DDR_COL   10 /* COL :  8 to 10 column address */
#define DDR_BANK8  1 /* Banks each chip: 0-4bank, 1-8bank */
#define DDR_CL     4 /* CAS latency: 1 to 6 */

/*
 * DDR2 controller timing1 register, unit: ps
 */
#define DDR_tRAS  40000  /* ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP   7500  /* READ to PRECHARGE command period. */
#define DDR_tRP   15000  /* PRECHARGE command period to the same bank */
#define DDR_tRCD  15000  /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC   55000  /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD   7500  /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR   15000  /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR   7500  /* WRITE to READ command delay. */

/*
 * DDR2 controller timing2 register
 */
#define DDR_tRFC    75000 /* ps,  AUTO-REFRESH command period. */
#define DDR_tMINSR    140 /* tCK, Minimum Self-Refresh / Deep-Power-Down */
#define DDR_tXP         2 /* tCK, EXIT-POWER-DOWN to next valid command period: 1 to 8 tCK. */
#define DDR_tMRD        4 /* tCK, Load-Mode-Register to next valid command period: 1 to 4 tCK */

/* 
 * DDR2 controller mdelay register
 */
#define DDR_TSEL   2 /* PHY to DDR transfer delay */
#define DDR_MSEL   0 /* Delay DQS by [0-3] ticks */
#define DDR_HL     0 /* Delay reduced by [0-1] 1/2 ticks */
#define DDR_QUAR   0 /* Delay augmented by [0-1] 1/4 ticks */

#if (DDR_TSEL == 3) && (DDR_CL == 5)
#	error "TSEL=3 is only valid when CL<5"
#endif

/*
 * DDR2 controller refcnt register
 */
#define DDR_tREFI	        7800	/* Refresh period: ns */

#define DDR_CLK_DIV 1    /* Clock Divider. auto refresh
						  *	cnt_clk = memclk/(16*(2^DDR_CLK_DIV))
						  */
