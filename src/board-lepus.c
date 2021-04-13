
#include <string.h>
#include <stdint.h>

#include "board.h"
#include "config.h"
#include "sdram.h"
#include "serial.h"
#include "utils.h"
#include "jz.h"

#include "jz4760-cpm.h"
#include "jz4760-ddrc.h"
#include "jz4740-gpio.h"

#define PIN_BKLIGHT (32*4+1)	/* Port 4 pin 1: Backlight PWM */

#define CDIV  2
#define HDIV  6
#define H2DIV 6
#define PDIV  6
#define MDIV  6
#define SDIV  6

#define FVCO_MIN  500*1000*1000		/* 500 MHz */
#define FVCO_MAX  1500*1000*1000	/* 1.5 GHz */

/* The frequency after the input divider must be between 1 and 50 MHz.
   The highest divider yields the best resolution. */
#define INDIV(ext) \
	(((ext / 10000000) <= 2) ? 2 : (ext / 10000000)) /* INDIV must be >= 2 according to the manual */

#define OUTDIV(f) \
	(((f) >= FVCO_MIN) ? 0 : \
	 ((f) >= (FVCO_MIN / 2)) ? 1 : \
	 ((f) >= (FVCO_MIN / 4)) ? 2 : 3)

#define RATE(f) \
	((f) < (FVCO_MIN / 8) ? FVCO_MIN : \
	 (((f) > FVCO_MAX) ? FVCO_MAX : (f)))

#define FEEDBACK(f, ext) \
	((((RATE(f) << OUTDIV(f)) / 1000) * INDIV(ext)) / (ext / 1000))

static void pll_init(void)
{
	unsigned int i;
	static const uint8_t n2FR[9] = {
		0, 0, 1, 2, 3, 0, 4, 0, 5,
	};

	/* write REG_DDRC_CTRL 8 times to clear ddr fifo */
	for (i = 0; i < 8; i++)
		REG_DDRC_CTRL = 0;

	REG_CPM_CPCCR = ((unsigned int) n2FR[CDIV] << CPM_CPCCR_CDIV_BIT) |
		((unsigned int) n2FR[HDIV]  << CPM_CPCCR_HDIV_BIT)  |
		((unsigned int) n2FR[H2DIV] << CPM_CPCCR_H2DIV_BIT) |
		((unsigned int) n2FR[PDIV]  << CPM_CPCCR_PDIV_BIT)  |
		((unsigned int) n2FR[MDIV]  << CPM_CPCCR_MDIV_BIT)  |
		((unsigned int) n2FR[SDIV]  << CPM_CPCCR_SDIV_BIT)  |
		CPM_CPCCR_MEM | CPM_CPCCR_CE;

	REG_CPM_CPPCR = ((FEEDBACK(CFG_CPU_SPEED, CFG_EXTAL) >> 1) << CPM_CPPCR_PLLM_BIT) |
		((INDIV(CFG_EXTAL)) << CPM_CPPCR_PLLN_BIT) |
		(OUTDIV(CFG_CPU_SPEED) << CPM_CPPCR_PLLOD_BIT) |
		CPM_CPPCR_PLLEN;

	/* Wait for a stable output */
	while (!__cpm_pll_is_on());
	while (!(REG_CPM_CPPCR & CPM_CPPCR_PLLS));
}

static void write_lmr(uint32_t value)
{
	REG_DDRC_LMR = value | DDRC_LMR_START;

	while (REG_DDRC_LMR & DDRC_LMR_START);
}

static void ddr_mem_init(void)
{
	/* TODO: Add support for CL=1.5 CL=2.5 CL=3.5 CL=4.5 */
	static const uint8_t ddr_cl_lookup[7] = {
		0, 9, 10, 11, 12, 13, 14,
	};
	unsigned int ddr_twr = ((REG_DDRC_TIMING1 & DDRC_TIMING1_TWR_MASK) >> DDRC_TIMING1_TWR_BIT) + 1;

	REG_DDRC_CFG = DDRC_CFG_TYPE_DDR2 | DDRC_CFG_MPRT |
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

	/* Set CKE High */
	REG_DDRC_CTRL |= DDRC_CTRL_CKE;

	/* PREA */
	write_lmr(DDRC_LMR_CMD_PREC);

	/* EMR2: extend mode register2 */
	write_lmr(DDRC_LMR_BA_EMRS2 | DDRC_LMR_CMD_LMR);

	/* EMR3: extend mode register3 */
	write_lmr(DDRC_LMR_BA_EMRS3 | DDRC_LMR_CMD_LMR);

	/* EMR1: extend mode register1 */
	write_lmr((DDR_EMRS1_DIC_NORMAL | DDR_EMRS1_DQS_DIS) << 16 |
		  DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR);

	/* MR - DLL Reset A1A0 burst 2 */
	write_lmr(((ddr_twr - 1) << 9 | DDR2_MRS_DLL_RST | DDR_CL << 4 | DDR_MRS_BL_4) << 16 |
		  DDRC_LMR_BA_MRS | DDRC_LMR_CMD_LMR);

	/* PREA */
	write_lmr(DDRC_LMR_CMD_PREC);

	/* AR: auto refresh */
	write_lmr(DDRC_LMR_CMD_AUREF);
	write_lmr(DDRC_LMR_CMD_AUREF);

	/* MR - DLL Reset End */
	write_lmr(((ddr_twr - 1) << 9 | DDR_CL << 4 | DDR_MRS_BL_4) << 16 |
		  DDRC_LMR_BA_MRS | DDRC_LMR_CMD_LMR);

	/* EMR1 - OCD Default */
	write_lmr((DDR_EMRS1_DIC_NORMAL | DDR_EMRS1_DQS_DIS | DDR_EMRS1_OCD_DFLT) << 16 |
		  DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR);

	/* EMR1 - OCD Exit */
	write_lmr((DDR_EMRS1_DIC_NORMAL | DDR_EMRS1_DQS_DIS) << 16 |
		  DDRC_LMR_BA_EMRS1 | DDRC_LMR_CMD_LMR);
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

	REG_DDRC_PMEMCTRL0 =  0xaaaa; /* FIXME: ODT registers not configed */
	REG_DDRC_PMEMCTRL1 =  0;

	REG_DDRC_PMEMCTRL2 = 0xaaaaa;

	REG_DDRC_PMEMCTRL3 &= ~(1 << 16);
	REG_DDRC_PMEMCTRL3 |= (1 << 17);
	REG_DDRC_PMEMCTRL3 &= ~(1 << 15);

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

	timing = 0;

	if (DDR_TSEL > 0)
		timing = (DDR_TSEL - 1) << DDRC_TIMING2_RWCOV_BIT;

	/* AUTO-REFRESH command period. */
	tmp = DDR_GET_VALUE(DDR_tRFC, ps);
	tmp = constrain(tmp, 0, 31);
	timing |= (tmp / 2) << DDRC_JZ4760_TIMING2_TRFC_BIT;
	timing |= (tmp / 2) << DDRC_JZ4760B_TIMING2_TRFC_BIT;

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
	write_lmr(DDRC_LMR_CMD_AUREF);
	udelay(500);
}

void board_init(void)
{

#ifdef USE_SERIAL
	__gpio_as_func_mask(GPIOD, 0x14000000, 0);

	__cpm_start_uart1();

	serial_init();
#endif

	pll_init();
	SERIAL_PUTS_ARGI("PLL running at ", __cpm_get_pllout() / 1000000, " MHz.\n");

	sdram_init();
	SERIAL_PUTS_ARGI("SDRAM running at ", __cpm_get_mclk() / 1000000, " MHz.\n");
	SERIAL_PUTS_ARGI("SDRAM size is ", get_memory_size() / 1048576, " MiB.\n");

	/* Set pin for backlight as output GPIO */
	__gpio_clear_pin(GPIOE, 1);
	__gpio_as_output(GPIOE, 1);
	//__gpio_set_pin(GPIOE, 1);

	/* MSC0 pins */
	__gpio_as_func_mask(GPIOA, 0x00ec0000, 1);
	__gpio_as_func_mask(GPIOA, 0x00100000, 0);

	/* Set pins for input buttons as input GPIOs */
	__gpio_as_input_mask(GPIOE, 0x38000000);

	/* MSC2 pins */
	__gpio_as_func_mask(GPIOE, 0x30f00000, 2);

	__cpm_start_msc0();
	__cpm_start_msc2();
	__cpm_select_msc_clk();
}


void light(int i)
{
	if (i)
		__gpio_set_pin(GPIOE, 1);
	else
		__gpio_clear_pin(GPIOE, 1);
}

int alt_key_pressed(void)
{
	return !__gpio_get_pin(4, 7);	/* Port 4 pin 7: X button */
}

int alt2_key_pressed(void)
{
	return !__gpio_get_pin(4, 11); /* Port 4 pin: 11 Y button */
}

int alt3_key_pressed(void)
{
	return !__gpio_get_pin(3, 22); /* Port 3 pin 22: A button */
}

unsigned int get_memory_size(void)
{
	return (1 << (DDR_ROW + DDR_COL + DDR_DW32 + DDR_BANK8 + 3))
		* (DDR_CS1EN + DDR_CS0EN);
}
