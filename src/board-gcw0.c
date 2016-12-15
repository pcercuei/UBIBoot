
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "config.h"
#include "sdram.h"
#include "serial.h"
#include "utils.h"
#include "jz.h"

#define PIN_A (32*4 + 29)		/* Port 4 pin 29: A button */
#define PIN_X (32*4 + 28)		/* Port 4 pin 28: X button */
#define PIN_Y (32*4 + 27)		/* Port 4 pin 27: Y button */
#define PIN_BKLIGHT	(32*4+1)	/* Port 4 pin 1: Backlight PWM  */

/* Authorized values: 1 2 3 4 6 8 12 */
#define CFG_CDIV  1
#define CFG_H0DIV 6
#define CFG_H1DIV 6
#define CFG_H2DIV 6
#define CFG_PDIV  6
#define CFG_C1DIV 6


#define FVCO_MIN  300*1000*1000	/* 300 MHz */
#define FVCO_MAX  1000*1000*1000	/* 1 GHz */

/* The frequency after the input divider must be between 10 and 50 MHz.
The highest divider yields the best resolution. */
#define INDIV(ext) (ext / 10000000)

#define OUTDIV(f) \
  (((f) >= FVCO_MIN) ? 1 : \
   ((f) >= (FVCO_MIN / 2)) ? 2 : \
   ((f) >= (FVCO_MIN / 4)) ? 4 : 8)

#define RATE(f) \
  ((f) < (FVCO_MIN / 8) ? FVCO_MIN : \
	(((f) > FVCO_MAX) ? FVCO_MAX : (f)))

#define FEEDBACK(f, ext) \
  ((((RATE(f) * OUTDIV(f)) / 1000) * INDIV(ext)) / (ext / 1000))

#define BS(f) ((f) > (FVCO_MAX / 2))

static void pll_init(void)
{
	unsigned int reg, i;
	static const uint8_t n2FR[13] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5, 0, 0, 0, 6,
	};

	/* write REG_DDRC_CTRL 8 times to clear ddr fifo */
	for (i = 0; i < 8; i++)
		REG_DDRC_CTRL = 0;

	REG_CPM_CPCCR = ((CFG_CDIV - 1) << CPM_CPCCR_CDIV_BIT) |
	  ((unsigned int) n2FR[CFG_H0DIV] << CPM_CPCCR_H0DIV_BIT) |
	  ((unsigned int) n2FR[CFG_H1DIV] << CPM_CPCCR_H1DIV_BIT) |
	  ((unsigned int) n2FR[CFG_H2DIV] << CPM_CPCCR_H2DIV_BIT) |
	  ((unsigned int) n2FR[CFG_PDIV]  << CPM_CPCCR_PDIV_BIT)  |
	  ((unsigned int) n2FR[CFG_C1DIV] << CPM_CPCCR_C1DIV_BIT) |
	  CPM_CPCCR_MEM | CPM_CPCCR_CE;

	reg = ((FEEDBACK(CFG_CPU_SPEED, CFG_EXTAL) - 1) << CPM_CPPCR_PLLM_BIT) |
	  ((INDIV(CFG_EXTAL) - 1) << CPM_CPPCR_PLLN_BIT) |
	  ((OUTDIV(CFG_CPU_SPEED) - 1) << CPM_CPPCR_PLLOD_BIT) |
	  CPM_CPPCR_PLLEN;
	if (BS(CFG_CPU_SPEED))
		reg |= CPM_CPPCR_PLL_BS_BIT;
	REG_CPM_CPPCR = reg;

	/* Wait for a stable output */
	while (!__cpm_pll_is_on());
	while (!(REG_CPM_CPPCR & CPM_CPPCR_PLLS));

	/* Disable pll0/2 divider */
	REG_CPM_CPCCR &= ~CPM_CPCCR_PCS;
}

static void ddr_mem_init(void)
{
	/* TODO: Add support for CL=1.5 CL=2.5 CL=3.5 CL=4.5 */
	static const uint8_t ddr_cl_lookup[5] = {
		0, 9, 10, 11, 12,
	};

	unsigned int ddr_twr =
#if defined(CONFIG_SDRAM_DDR2)
	  ((REG_DDRC_TIMING1 & DDRC_TIMING1_TWR_MASK) >> DDRC_TIMING1_TWR_BIT) + 1;
#else
	  1;
#endif

	REG_DDRC_CFG =
#if defined(CONFIG_SDRAM_DDR1)
	  DDRC_CFG_TYPE_DDR1 | DDRC_CFG_BTRUN |
#elif defined(CONFIG_SDRAM_MDDR)
	  DDRC_CFG_TYPE_MDDR | DDRC_CFG_BTRUN |
#elif defined(CONFIG_SDRAM_DDR2)
	  DDRC_CFG_TYPE_DDR2 |
#else
#error No supported SDRAM type defined
#endif
	  DDRC_CFG_MPRT |
	  ((DDR_ROW - 12) << DDRC_CFG_ROW_BIT) |
	  ((DDR_COL - 8) << DDRC_CFG_COL_BIT) |
	  (DDR_CS1EN << 7) |
	  (DDR_CS0EN << 6) |
	  ((unsigned int) ddr_cl_lookup[DDR_CL - 1] << DDRC_CFG_CL_BIT) |
	  (DDR_BANK8 << 1) |
	  DDR_DW32;

	REG_DDRC_MDELAY = DDRC_MDELAY_MAUTO |
	  (DDR_TSEL << DDRC_CFG_TSEL_BIT) |
	  (DDR_MSEL << DDRC_CFG_MSEL_BIT) |
	  (DDR_HL << 15) | (DDR_QUAR << 14);
	udelay(10);

#if defined(CONFIG_SDRAM_DDR2)
	/* PREA */
	REG_DDRC_LMR =  DDRC_LMR_CMD_PREC | DDRC_LMR_START;
	udelay(1);

	/* EMR2: extend mode register2 */
	REG_DDRC_LMR = DDRC_LMR_BA_EMRS2 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* EMR3: extend mode register3 */
	REG_DDRC_LMR = DDRC_LMR_BA_EMRS3 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* EMR1: extend mode register1 */
	REG_DDRC_LMR = ((DDR_EMRS1_DIC_HALF) << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
	udelay(1);

	/* MR - DLL Reset A1A0 burst 2 */
	REG_DDRC_LMR = ((ddr_twr - 1) << 9 | DDR2_MRS_DLL_RST | DDR_CL << 4 | DDR_MRS_BL_4) << 16
		| DDRC_LMR_BA_MRS | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
	udelay(1);
#endif

	/* PREA */
	REG_DDRC_LMR =  DDRC_LMR_CMD_PREC | DDRC_LMR_START; //0x1;
	udelay(1);

	/* AR: auto refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;
	udelay(1);
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START; //0x11;
	udelay(1);

	/* MR - DLL Reset End */
	REG_DDRC_LMR = ((ddr_twr-1)<<9 | DDR_CL<<4 | DDR_MRS_BL_4)<< 16
		| DDRC_LMR_BA_MRS | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
	udelay(2);

#if defined(CONFIG_SDRAM_DDR2)
	/* EMR1 - OCD Default */
	REG_DDRC_LMR = (DDR_EMRS1_DIC_HALF | DDR_EMRS1_OCD_DFLT) << 16 | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;

	/* EMR1 - OCD Exit */
	REG_DDRC_LMR = ((DDR_EMRS1_DIC_HALF) << 16) | DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#else
	/* EMR: extend mode register */
	REG_DDRC_LMR = (DDR_EMRS_DS_FULL | DDR_EMRS_PRSR_ALL) << 16
		| DDRC_LMR_BA_M_EMRS | DDRC_LMR_CMD_LMR | DDRC_LMR_START;
#endif
	udelay(1);
}

static unsigned int constrain(unsigned int x,
			unsigned int min, unsigned int max)
{
	if (x < min)
		x = min;
	if (x > max)
		x = max;
	return x;
}

static void sdram_init(void)
{
	unsigned int mem_mask, mem_base, mem_clk, ps, timing, tmp, ns_int;
	unsigned int mem_size = get_memory_size();

	__cpm_start_ddr();

	mem_clk = __cpm_get_mclk();
	ps = 1000000000 / (mem_clk / 1000); /* ns per tck ns <= real value */
	ns_int = (1000000000 % mem_clk == 0) ?
		(1000000000 / mem_clk) : (1000000000 / mem_clk + 1);

	/* reset ddrc_controller */
	REG_DDRC_CTRL = DDRC_CTRL_RESET;
	udelay(300);

	REG_DDRC_CTRL = DDRC_CTRL_PDT_DIS | DDRC_CTRL_PRET_8 |
	  DDRC_CTRL_UNALIGN | DDRC_CTRL_CKE;

#if defined(CONFIG_SDRAM_DDR2)
	REG_DDRC_PMEMCTRL0 =  0xaaaa; /* FIXME: ODT registers not configed */
	REG_DDRC_PMEMCTRL1 =  0;

#if defined(CONFIG_DDR2_DIFFERENTIAL)
	REG_DDRC_PMEMCTRL2 = 0xaaaaa;
#else
	REG_DDRC_PMEMCTRL2 = 0x0;
#endif
	REG_DDRC_PMEMCTRL3 &= ~(1 << 16);
	REG_DDRC_PMEMCTRL3 |= (1 << 17);
#if defined(CONFIG_DDR2_DIFFERENTIAL)
	REG_DDRC_PMEMCTRL3 &= ~(1 << 15);
#endif
#elif defined(CONFIG_SDRAM_MDDR)
	REG_DDRC_PMEMCTRL3 |= (1 << 16);
#endif

	/* ACTIVE to PRECHARGE command period */
	tmp = DDR_GET_VALUE(DDR_tRAS, ps);
	tmp = constrain(tmp, 1, 31);
	timing = (((tmp) / 2) << DDRC_TIMING1_TRAS_BIT);

	/* READ to PRECHARGE command period. */
	tmp = DDR_GET_VALUE(DDR_tRTP, ps);
	tmp = constrain(tmp, 1, 4);
	timing |= ((tmp - 1) << DDRC_TIMING1_TRTP_BIT);

	/* PRECHARGE command period. */
	tmp = DDR_GET_VALUE(DDR_tRP, ps);
	tmp = constrain(tmp, 1, 8);
	timing |= ((tmp - 1) << DDRC_TIMING1_TRP_BIT);

	/* ACTIVE to READ or WRITE command period. */
	tmp = DDR_GET_VALUE(DDR_tRCD, ps);
	tmp = constrain(tmp, 1, 8);
	timing |= ((tmp - 1) << DDRC_TIMING1_TRCD_BIT);

	/* ACTIVE to ACTIVE command period. */
	tmp = DDR_GET_VALUE(DDR_tRC, ps);
	tmp = constrain(tmp, 3, 31);
	timing |= ((tmp / 2) << DDRC_TIMING1_TRC_BIT);

	/* ACTIVE bank A to ACTIVE bank B command period. */
	tmp = DDR_GET_VALUE(DDR_tRRD, ps);
	tmp = constrain(tmp, 2, 4);
	timing |= ((tmp - 1) << DDRC_TIMING1_TRRD_BIT);

	/* WRITE Recovery Time defined by register MR of DDR2 memory */
	tmp = DDR_GET_VALUE(DDR_tWR, ps);
	tmp = constrain(tmp, 2, 6);
	timing |= ((tmp - 1) << DDRC_TIMING1_TWR_BIT);

	/* WRITE to READ command delay. */
	tmp = DDR_tWTR < 5
		? DDR_tWTR /* unit is tCK */
		: DDR_GET_VALUE(DDR_tWTR, ps); /* unit is ps */
	tmp = constrain(tmp, 1, 4);
	timing |= ((tmp - 1) << DDRC_TIMING1_TWTR_BIT);

	REG_DDRC_TIMING1 = timing;

	if (DDR_TSEL > 0)
		timing = (DDR_TSEL - 1) << DDRC_TIMING2_RWCOV_BIT;
	else
		timing = 0;

	/* AUTO-REFRESH command period. */
	tmp = DDR_GET_VALUE(DDR_tRFC, ps);
	tmp = constrain(tmp, 0, 31);
	timing |= ((tmp / 2) << DDRC_TIMING2_TRFC_BIT);

	/* Minimum Self-Refresh / Deep-Power-Down time */
	tmp = DDR_tMINSR;
	tmp = constrain(tmp, 9, 129);
	tmp = ((tmp - 1) % 8 == 0) ? ((tmp - 1) / 8 - 1) : ((tmp - 1) / 8);
	timing |= (tmp << DDRC_TIMING2_TMINSR_BIT);
	timing |= (DDR_tXP - 1) << 4 | (DDR_tMRD - 1);

	REG_DDRC_TIMING2 = timing;
	REG_DDRC_DQS_ADJ = 0;

	ddr_mem_init();

	mem_mask = 0xff &
		~(((mem_size / (DDR_CS1EN + DDR_CS0EN) >> 24) - 1) &
		  DDRC_MMAP_MASK_MASK);

	mem_base = DDR_MEM_PHY_BASE >> 24;
	REG_DDRC_MMAP0 = mem_base << DDRC_MMAP_BASE_BIT | mem_mask;

	mem_base = (DDR_MEM_PHY_BASE + mem_size / (DDR_CS1EN + DDR_CS0EN)) >> 24;
	REG_DDRC_MMAP1 = mem_base << DDRC_MMAP_BASE_BIT | mem_mask;

	tmp = (DDR_tREFI / ns_int) / (16 * (1 << DDR_CLK_DIV)) - 1;
	tmp = constrain(tmp, 1, 0xfff);

	REG_DDRC_REFCNT = (tmp << DDRC_REFCNT_CON_BIT) |
	  (DDR_CLK_DIV << 1) | DDRC_REFCNT_REF_EN;

	/* Enable DLL Detect */
	REG_DDRC_DQS = DDRC_DQS_AUTO | DDRC_DQS_DET | DDRC_DQS_SRDET;
	while (!(REG_DDRC_DQS & DDRC_DQS_READY));

	/* Auto Refresh */
	REG_DDRC_LMR = DDRC_LMR_CMD_AUREF | DDRC_LMR_START;
	udelay(500);
}

void board_init(void)
{
	pll_init();
	sdram_init();

#ifdef USE_SERIAL
	__gpio_as_uart2();
	__cpm_start_uart2();
	serial_init();
#endif
	__gpio_as_msc0_boot();
	__cpm_start_msc0();
	__cpm_select_msc_clk(0, 1);

	/* This is magic. If you know what Cop0 register $5 config 4 does,
	 * please tell us. */
	asm volatile ("li $2, 0xa9000000\n\t"
			"mtc0 $2, $5, 4\n\t"
			"nop\n\t"
			::"r"(2));
}

void light(int i)
{
	if (i)
		__gpio_as_output1(PIN_BKLIGHT);
	else
		__gpio_as_output0(PIN_BKLIGHT);
}

int alt_key_pressed(void)
{
	__gpio_as_input(PIN_X);
	return !__gpio_get_pin(PIN_X);
}

int alt2_key_pressed(void)
{
	__gpio_as_input(PIN_Y);
	return !__gpio_get_pin(PIN_Y);
}

int alt3_key_pressed(void)
{
	__gpio_as_input(PIN_A);
	return !__gpio_get_pin(PIN_A);
}

unsigned int get_memory_size(void)
{
	return (1 << (DDR_ROW + DDR_COL + DDR_DW32 + DDR_BANK8 + 3))
			* (DDR_CS1EN + DDR_CS0EN);
}
