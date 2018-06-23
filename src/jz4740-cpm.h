#ifndef __JZ4740_CPM_H__
#define __JZ4740_CPM_H__

#include "jz.h"

#define	CPM_BASE	0xB0000000

#define CPM_CPCCR	(CPM_BASE+0x00)
#define CPM_CPPCR	(CPM_BASE+0x10)
#define CPM_I2SCDR	(CPM_BASE+0x60)
#define CPM_LPCDR	(CPM_BASE+0x64)
#define CPM_MSCCDR	(CPM_BASE+0x68)
#define CPM_UHCCDR	(CPM_BASE+0x6C)

#define CPM_LCR		(CPM_BASE+0x04)
#define CPM_CLKGR	(CPM_BASE+0x20)
#define CPM_SCR		(CPM_BASE+0x24)

#define CPM_HCR		(CPM_BASE+0x30)
#define CPM_HWFCR	(CPM_BASE+0x34)
#define CPM_HRCR	(CPM_BASE+0x38)
#define CPM_HWCR	(CPM_BASE+0x3c)
#define CPM_HWSR	(CPM_BASE+0x40)
#define CPM_HSPR	(CPM_BASE+0x44)

#define CPM_RSR		(CPM_BASE+0x08)


#define REG_CPM_CPCCR	REG32(CPM_CPCCR)
#define REG_CPM_CPPCR	REG32(CPM_CPPCR)
#define REG_CPM_I2SCDR	REG32(CPM_I2SCDR)
#define REG_CPM_LPCDR	REG32(CPM_LPCDR)
#define REG_CPM_MSCCDR	REG32(CPM_MSCCDR)
#define REG_CPM_UHCCDR	REG32(CPM_UHCCDR)

#define REG_CPM_LCR	REG32(CPM_LCR)
#define REG_CPM_CLKGR	REG32(CPM_CLKGR)
#define REG_CPM_SCR	REG32(CPM_SCR)
#define REG_CPM_HCR	REG32(CPM_HCR)
#define REG_CPM_HWFCR	REG32(CPM_HWFCR)
#define REG_CPM_HRCR	REG32(CPM_HRCR)
#define REG_CPM_HWCR	REG32(CPM_HWCR)
#define REG_CPM_HWSR	REG32(CPM_HWSR)
#define REG_CPM_HSPR	REG32(CPM_HSPR)

#define REG_CPM_RSR	REG32(CPM_RSR)


/* Clock Control Register */
#define CPM_CPCCR_I2CS		(1 << 31)
#define CPM_CPCCR_CLKOEN	(1 << 30)
#define CPM_CPCCR_UCS		(1 << 29)
#define CPM_CPCCR_UDIV_BIT	23
#define CPM_CPCCR_UDIV_MASK	(0x3f << CPM_CPCCR_UDIV_BIT)
#define CPM_CPCCR_CE		(1 << 22)
#define CPM_CPCCR_PCS		(1 << 21)
#define CPM_CPCCR_LDIV_BIT	16
#define CPM_CPCCR_LDIV_MASK	(0x1f << CPM_CPCCR_LDIV_BIT)
#define CPM_CPCCR_MDIV_BIT	12
#define CPM_CPCCR_MDIV_MASK	(0x0f << CPM_CPCCR_MDIV_BIT)
#define CPM_CPCCR_PDIV_BIT	8
#define CPM_CPCCR_PDIV_MASK	(0x0f << CPM_CPCCR_PDIV_BIT)
#define CPM_CPCCR_HDIV_BIT	4
#define CPM_CPCCR_HDIV_MASK	(0x0f << CPM_CPCCR_HDIV_BIT)
#define CPM_CPCCR_CDIV_BIT	0
#define CPM_CPCCR_CDIV_MASK	(0x0f << CPM_CPCCR_CDIV_BIT)

/* I2S Clock Divider Register */
#define CPM_I2SCDR_I2SDIV_BIT	0
#define CPM_I2SCDR_I2SDIV_MASK	(0x1ff << CPM_I2SCDR_I2SDIV_BIT)

/* LCD Pixel Clock Divider Register */
#define CPM_LPCDR_PIXDIV_BIT	0
#define CPM_LPCDR_PIXDIV_MASK	(0x1ff << CPM_LPCDR_PIXDIV_BIT)

/* MSC Clock Divider Register */
#define CPM_MSCCDR_MSCDIV_BIT	0
#define CPM_MSCCDR_MSCDIV_MASK	(0x1f << CPM_MSCCDR_MSCDIV_BIT)

/* PLL Control Register */
#define CPM_CPPCR_PLLM_BIT	23
#define CPM_CPPCR_PLLM_MASK	(0x1ff << CPM_CPPCR_PLLM_BIT)
#define CPM_CPPCR_PLLN_BIT	18
#define CPM_CPPCR_PLLN_MASK	(0x1f << CPM_CPPCR_PLLN_BIT)
#define CPM_CPPCR_PLLOD_BIT	16
#define CPM_CPPCR_PLLOD_MASK	(0x03 << CPM_CPPCR_PLLOD_BIT)
#define CPM_CPPCR_PLLS		(1 << 10)
#define CPM_CPPCR_PLLBP		(1 << 9)
#define CPM_CPPCR_PLLEN		(1 << 8)
#define CPM_CPPCR_PLLST_BIT	0
#define CPM_CPPCR_PLLST_MASK	(0xff << CPM_CPPCR_PLLST_BIT)

/* Low Power Control Register */
#define CPM_LCR_DOZE_DUTY_BIT 	3
#define CPM_LCR_DOZE_DUTY_MASK 	(0x1f << CPM_LCR_DOZE_DUTY_BIT)
#define CPM_LCR_DOZE_ON		(1 << 2)
#define CPM_LCR_LPM_BIT		0
#define CPM_LCR_LPM_MASK	(0x3 << CPM_LCR_LPM_BIT)
  #define CPM_LCR_LPM_IDLE	(0x0 << CPM_LCR_LPM_BIT)
  #define CPM_LCR_LPM_SLEEP	(0x1 << CPM_LCR_LPM_BIT)

/* Clock Gate Register */
#define CPM_CLKGR_UART1		(1 << 15)
#define CPM_CLKGR_UHC		(1 << 14)
#define CPM_CLKGR_IPU		(1 << 13)
#define CPM_CLKGR_DMAC		(1 << 12)
#define CPM_CLKGR_UDC		(1 << 11)
#define CPM_CLKGR_LCD		(1 << 10)
#define CPM_CLKGR_CIM		(1 << 9)
#define CPM_CLKGR_SADC		(1 << 8)
#define CPM_CLKGR_MSC		(1 << 7)
#define CPM_CLKGR_AIC1		(1 << 6)
#define CPM_CLKGR_AIC2		(1 << 5)
#define CPM_CLKGR_SSI		(1 << 4)
#define CPM_CLKGR_I2C		(1 << 3)
#define CPM_CLKGR_RTC		(1 << 2)
#define CPM_CLKGR_TCU		(1 << 1)
#define CPM_CLKGR_UART0		(1 << 0)

/* Sleep Control Register */
#define CPM_SCR_O1ST_BIT	8
#define CPM_SCR_O1ST_MASK	(0xff << CPM_SCR_O1ST_BIT)
#define CPM_SCR_USBPHY_ENABLE	(1 << 6)
#define CPM_SCR_OSC_ENABLE	(1 << 4)

/* Hibernate Control Register */
#define CPM_HCR_PD		(1 << 0)

/* Wakeup Filter Counter Register in Hibernate Mode */
#define CPM_HWFCR_TIME_BIT	0
#define CPM_HWFCR_TIME_MASK	(0x3ff << CPM_HWFCR_TIME_BIT)

/* Reset Counter Register in Hibernate Mode */
#define CPM_HRCR_TIME_BIT	0
#define CPM_HRCR_TIME_MASK	(0x7f << CPM_HRCR_TIME_BIT)

/* Wakeup Control Register in Hibernate Mode */
#define CPM_HWCR_WLE_LOW	(0 << 2)
#define CPM_HWCR_WLE_HIGH	(1 << 2)
#define CPM_HWCR_PIN_WAKEUP	(1 << 1)
#define CPM_HWCR_RTC_WAKEUP	(1 << 0)

/* Wakeup Status Register in Hibernate Mode */
#define CPM_HWSR_WSR_PIN	(1 << 1)
#define CPM_HWSR_WSR_RTC	(1 << 0)

/* Reset Status Register */
#define CPM_RSR_HR		(1 << 2)
#define CPM_RSR_WR		(1 << 1)
#define CPM_RSR_PR		(1 << 0)

#define __cpm_get_pllm() \
	((REG_CPM_CPPCR & CPM_CPPCR_PLLM_MASK) >> CPM_CPPCR_PLLM_BIT)
#define __cpm_get_plln() \
	((REG_CPM_CPPCR & CPM_CPPCR_PLLN_MASK) >> CPM_CPPCR_PLLN_BIT)
#define __cpm_get_pllod() \
	((REG_CPM_CPPCR & CPM_CPPCR_PLLOD_MASK) >> CPM_CPPCR_PLLOD_BIT)

#define __cpm_get_cdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_CDIV_MASK) >> CPM_CPCCR_CDIV_BIT)
#define __cpm_get_hdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_HDIV_MASK) >> CPM_CPCCR_HDIV_BIT)
#define __cpm_get_pdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_PDIV_MASK) >> CPM_CPCCR_PDIV_BIT)
#define __cpm_get_mdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_MDIV_MASK) >> CPM_CPCCR_MDIV_BIT)
#define __cpm_get_ldiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_LDIV_MASK) >> CPM_CPCCR_LDIV_BIT)
#define __cpm_get_udiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_UDIV_MASK) >> CPM_CPCCR_UDIV_BIT)
#define __cpm_get_i2sdiv() \
	((REG_CPM_I2SCDR & CPM_I2SCDR_I2SDIV_MASK) >> CPM_I2SCDR_I2SDIV_BIT)
#define __cpm_get_pixdiv() \
	((REG_CPM_LPCDR & CPM_LPCDR_PIXDIV_MASK) >> CPM_LPCDR_PIXDIV_BIT)
#define __cpm_get_mscdiv() \
	((REG_CPM_MSCCDR & CPM_MSCCDR_MSCDIV_MASK) >> CPM_MSCCDR_MSCDIV_BIT)

#define __cpm_set_cdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_CDIV_MASK) | ((v) << (CPM_CPCCR_CDIV_BIT)))
#define __cpm_set_hdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_HDIV_MASK) | ((v) << (CPM_CPCCR_HDIV_BIT)))
#define __cpm_set_pdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_PDIV_MASK) | ((v) << (CPM_CPCCR_PDIV_BIT)))
#define __cpm_set_mdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_MDIV_MASK) | ((v) << (CPM_CPCCR_MDIV_BIT)))
#define __cpm_set_ldiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_LDIV_MASK) | ((v) << (CPM_CPCCR_LDIV_BIT)))
#define __cpm_set_udiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_UDIV_MASK) | ((v) << (CPM_CPCCR_UDIV_BIT)))
#define __cpm_set_i2sdiv(v) \
	(REG_CPM_I2SCDR = (REG_CPM_I2SCDR & ~CPM_I2SCDR_I2SDIV_MASK) | ((v) << (CPM_I2SCDR_I2SDIV_BIT)))
#define __cpm_set_pixdiv(v) \
	(REG_CPM_LPCDR = (REG_CPM_LPCDR & ~CPM_LPCDR_PIXDIV_MASK) | ((v) << (CPM_LPCDR_PIXDIV_BIT)))
#define __cpm_set_mscdiv(v) \
	(REG_CPM_MSCCDR = (REG_CPM_MSCCDR & ~CPM_MSCCDR_MSCDIV_MASK) | ((v) << (CPM_MSCCDR_MSCDIV_BIT)))

#define __cpm_select_i2sclk_exclk()	(REG_CPM_CPCCR &= ~CPM_CPCCR_I2CS)
#define __cpm_select_i2sclk_pll()	(REG_CPM_CPCCR |= CPM_CPCCR_I2CS)
#define __cpm_enable_cko()		(REG_CPM_CPCCR |= CPM_CPCCR_CLKOEN)
#define __cpm_select_usbclk_exclk()	(REG_CPM_CPCCR &= ~CPM_CPCCR_UCS)
#define __cpm_select_usbclk_pll()	(REG_CPM_CPCCR |= CPM_CPCCR_UCS)
#define __cpm_enable_pll_change()	(REG_CPM_CPCCR |= CPM_CPCCR_CE)
#define __cpm_pllout_direct()		(REG_CPM_CPCCR |= CPM_CPCCR_PCS)
#define __cpm_pllout_div2()		(REG_CPM_CPCCR &= ~CPM_CPCCR_PCS)

#define __cpm_pll_is_on()		(REG_CPM_CPPCR & CPM_CPPCR_PLLS)
#define __cpm_pll_bypass()		(REG_CPM_CPPCR |= CPM_CPPCR_PLLBP)
#define __cpm_pll_enable()		(REG_CPM_CPPCR |= CPM_CPPCR_PLLEN)

#define __cpm_get_cclk_doze_duty() \
	((REG_CPM_LCR & CPM_LCR_DOZE_DUTY_MASK) >> CPM_LCR_DOZE_DUTY_BIT)
#define __cpm_set_cclk_doze_duty(v) \
	(REG_CPM_LCR = (REG_CPM_LCR & ~CPM_LCR_DOZE_DUTY_MASK) | ((v) << (CPM_LCR_DOZE_DUTY_BIT)))

#define __cpm_doze_mode()		(REG_CPM_LCR |= CPM_LCR_DOZE_ON)
#define __cpm_idle_mode() \
	(REG_CPM_LCR = (REG_CPM_LCR & ~CPM_LCR_LPM_MASK) | CPM_LCR_LPM_IDLE)
#define __cpm_sleep_mode() \
	(REG_CPM_LCR = (REG_CPM_LCR & ~CPM_LCR_LPM_MASK) | CPM_LCR_LPM_SLEEP)

#define __cpm_stop_all() 	(REG_CPM_CLKGR = 0x7fff)
#define __cpm_stop_uart1()	(REG_CPM_CLKGR |= CPM_CLKGR_UART1)
#define __cpm_stop_uhc()	(REG_CPM_CLKGR |= CPM_CLKGR_UHC)
#define __cpm_stop_ipu()	(REG_CPM_CLKGR |= CPM_CLKGR_IPU)
#define __cpm_stop_dmac()	(REG_CPM_CLKGR |= CPM_CLKGR_DMAC)
#define __cpm_stop_udc()	(REG_CPM_CLKGR |= CPM_CLKGR_UDC)
#define __cpm_stop_lcd()	(REG_CPM_CLKGR |= CPM_CLKGR_LCD)
#define __cpm_stop_cim()	(REG_CPM_CLKGR |= CPM_CLKGR_CIM)
#define __cpm_stop_sadc()	(REG_CPM_CLKGR |= CPM_CLKGR_SADC)
#define __cpm_stop_msc()	(REG_CPM_CLKGR |= CPM_CLKGR_MSC)
#define __cpm_stop_aic1()	(REG_CPM_CLKGR |= CPM_CLKGR_AIC1)
#define __cpm_stop_aic2()	(REG_CPM_CLKGR |= CPM_CLKGR_AIC2)
#define __cpm_stop_ssi()	(REG_CPM_CLKGR |= CPM_CLKGR_SSI)
#define __cpm_stop_i2c()	(REG_CPM_CLKGR |= CPM_CLKGR_I2C)
#define __cpm_stop_rtc()	(REG_CPM_CLKGR |= CPM_CLKGR_RTC)
#define __cpm_stop_tcu()	(REG_CPM_CLKGR |= CPM_CLKGR_TCU)
#define __cpm_stop_uart0()	(REG_CPM_CLKGR |= CPM_CLKGR_UART0)

#define __cpm_start_all() 	(REG_CPM_CLKGR = 0x0)
#define __cpm_start_uart1()	(REG_CPM_CLKGR &= ~CPM_CLKGR_UART1)
#define __cpm_start_uhc()	(REG_CPM_CLKGR &= ~CPM_CLKGR_UHC)
#define __cpm_start_ipu()	(REG_CPM_CLKGR &= ~CPM_CLKGR_IPU)
#define __cpm_start_dmac()	(REG_CPM_CLKGR &= ~CPM_CLKGR_DMAC)
#define __cpm_start_udc()	(REG_CPM_CLKGR &= ~CPM_CLKGR_UDC)
#define __cpm_start_lcd()	(REG_CPM_CLKGR &= ~CPM_CLKGR_LCD)
#define __cpm_start_cim()	(REG_CPM_CLKGR &= ~CPM_CLKGR_CIM)
#define __cpm_start_sadc()	(REG_CPM_CLKGR &= ~CPM_CLKGR_SADC)
#define __cpm_start_msc()	(REG_CPM_CLKGR &= ~CPM_CLKGR_MSC)
#define __cpm_start_aic1()	(REG_CPM_CLKGR &= ~CPM_CLKGR_AIC1)
#define __cpm_start_aic2()	(REG_CPM_CLKGR &= ~CPM_CLKGR_AIC2)
#define __cpm_start_ssi()	(REG_CPM_CLKGR &= ~CPM_CLKGR_SSI)
#define __cpm_start_i2c()	(REG_CPM_CLKGR &= ~CPM_CLKGR_I2C)
#define __cpm_start_rtc()	(REG_CPM_CLKGR &= ~CPM_CLKGR_RTC)
#define __cpm_start_tcu()	(REG_CPM_CLKGR &= ~CPM_CLKGR_TCU)
#define __cpm_start_uart0()	(REG_CPM_CLKGR &= ~CPM_CLKGR_UART0)

#define __cpm_get_o1st() \
	((REG_CPM_SCR & CPM_SCR_O1ST_MASK) >> CPM_SCR_O1ST_BIT)
#define __cpm_set_o1st(v) \
	(REG_CPM_SCR = (REG_CPM_SCR & ~CPM_SCR_O1ST_MASK) | ((v) << (CPM_SCR_O1ST_BIT)))
#define __cpm_suspend_usbphy()		(REG_CPM_SCR |= CPM_SCR_USBPHY_SUSPEND)
#define __cpm_enable_osc_in_sleep()	(REG_CPM_SCR |= CPM_SCR_OSC_ENABLE)


#ifdef CFG_EXTAL
#define JZ_EXTAL		CFG_EXTAL
#else
#define JZ_EXTAL		3686400
#endif
#define JZ_EXTAL2		32768 /* RTC clock */

/* PLL output frequency */
static __inline__ unsigned int __cpm_get_pllout(void)
{
	unsigned long m, n, no, pllout;
	unsigned long cppcr = REG_CPM_CPPCR;
	static const u8 od[4] = {1, 2, 2, 4};
	if ((cppcr & CPM_CPPCR_PLLEN) && !(cppcr & CPM_CPPCR_PLLBP)) {
		m = __cpm_get_pllm() + 2;
		n = __cpm_get_plln() + 2;
		no = od[__cpm_get_pllod()];
		pllout = ((JZ_EXTAL) / (n * no)) * m;
	} else
		pllout = JZ_EXTAL;
	return pllout;
}

/* PLL output frequency for MSC/I2S/LCD/USB */
static __inline__ unsigned int __cpm_get_pllout2(void)
{
	if (REG_CPM_CPCCR & CPM_CPCCR_PCS)
		return __cpm_get_pllout();
	else
		return __cpm_get_pllout()/2;
}

/* CPU core clock */
static __inline__ unsigned int __cpm_get_cclk(void)
{
	static const u8 div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	return __cpm_get_pllout() / div[__cpm_get_cdiv()];
}

/* AHB system bus clock */
static __inline__ unsigned int __cpm_get_hclk(void)
{
	static const u8 div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	return __cpm_get_pllout() / div[__cpm_get_hdiv()];
}

/* Memory bus clock */
static __inline__ unsigned int __cpm_get_mclk(void)
{
	static const u8 div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	return __cpm_get_pllout() / div[__cpm_get_mdiv()];
}

/* APB peripheral bus clock */
static __inline__ unsigned int __cpm_get_pclk(void)
{
	static const u8 div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	return __cpm_get_pllout() / div[__cpm_get_pdiv()];
}

/* LCDC module clock */
static __inline__ unsigned int __cpm_get_lcdclk(void)
{
	return __cpm_get_pllout2() / (__cpm_get_ldiv() + 1);
}

/* LCD pixel clock */
static __inline__ unsigned int __cpm_get_pixclk(void)
{
	return __cpm_get_pllout2() / (__cpm_get_pixdiv() + 1);
}

/* I2S clock */
static __inline__ unsigned int __cpm_get_i2sclk(void)
{
	if (REG_CPM_CPCCR & CPM_CPCCR_I2CS) {
		return __cpm_get_pllout2() / (__cpm_get_i2sdiv() + 1);
	}
	else {
		return JZ_EXTAL;
	}
}

/* USB clock */
static __inline__ unsigned int __cpm_get_usbclk(void)
{
	if (REG_CPM_CPCCR & CPM_CPCCR_UCS) {
		return __cpm_get_pllout2() / (__cpm_get_udiv() + 1);
	}
	else {
		return JZ_EXTAL;
	}
}

/* MSC clock */
static __inline__ unsigned int __cpm_get_mscclk(void)
{
	return __cpm_get_pllout2() / (__cpm_get_mscdiv() + 1);
}

/* EXTAL clock for UART,I2C,SSI,TCU,USB-PHY */
static __inline__ unsigned int __cpm_get_extalclk(void)
{
	return JZ_EXTAL;
}

/* RTC clock for CPM,INTC,RTC,TCU,WDT */
static __inline__ unsigned int __cpm_get_rtcclk(void)
{
	return JZ_EXTAL2;
}

/*
 * Output 24MHz for SD and 16MHz for MMC.
 */
static inline void __cpm_select_msc_clk(int sd)
{
	unsigned int pllout2 = __cpm_get_pllout2();
	unsigned int div = 0;

	if (sd) {
		div = pllout2 / 24000000;
	}
	else {
		div = pllout2 / 16000000;
	}

	REG_CPM_MSCCDR = div - 1;
}

#endif /* __JZ4740_CPM_H__ */
