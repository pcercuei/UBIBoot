#ifndef __JZ4770_CPM_H__
#define __JZ4770_CPM_H__

#include "config.h"

#define	CPM_BASE	0xB0000000

#define CPM_CPCCR		(CPM_BASE+0x00) /* Clock control register		*/
#define CPM_CPPCR		(CPM_BASE+0x10) /* PLL control register 0		*/
#define CPM_CPPSR		(CPM_BASE+0x14) /* PLL switch and status Register	*/
#define CPM_CPPCR1		(CPM_BASE+0x30) /* PLL control register 1		*/
#define CPM_CPSPR		(CPM_BASE+0x34) /* CPM scratch pad register		*/
#define CPM_CPSPPR		(CPM_BASE+0x38) /* CPM scratch protected register	*/
#define CPM_USBPCR		(CPM_BASE+0x3c) /* USB parameter control register	*/
#define CPM_USBRDT		(CPM_BASE+0x40) /* USB reset detect timer register	*/
#define CPM_USBVBFIL		(CPM_BASE+0x44) /* USB jitter filter register		*/
#define CPM_USBPCR1             (CPM_BASE+0x48) /* USB parameter control register       */
#define CPM_USBCDR		(CPM_BASE+0x50) /* USB OTG PHY clock divider register	*/
#define CPM_I2SCDR		(CPM_BASE+0x60) /* I2S device clock divider register	*/
#define CPM_LPCDR		(CPM_BASE+0x64) /* LCD pix clock divider register	*/
#define CPM_MSCCDR		(CPM_BASE+0x68) /* MSC clock divider register		*/
#define CPM_UHCCDR		(CPM_BASE+0x6C) /* UHC 48M clock divider register	*/
#define CPM_SSICDR		(CPM_BASE+0x74) /* SSI clock divider register		*/
#define CPM_CIMCDR		(CPM_BASE+0x7c) /* CIM MCLK clock divider register	*/
#define CPM_GPSCDR		(CPM_BASE+0x80) /* GPS clock divider register		*/
#define CPM_PCMCDR		(CPM_BASE+0x84) /* PCM device clock divider register	*/
#define CPM_GPUCDR		(CPM_BASE+0x88) /* GPU clock divider register		*/
#define CPM_MSC1CDR             (CPM_BASE+0xa4) /* MSC1 clock divider register		*/
#define CPM_MSC2CDR             (CPM_BASE+0xa8) /* MSC2 clock divider register		*/
#define CPM_BCHCDR		(CPM_BASE+0xac) /* BCH clock divider register           */
#define CPM_INTR                (CPM_BASE+0xb0) /* CPM interrupt register               */
#define CPM_INTRE               (CPM_BASE+0xb4) /* CPM interrupt enable register        */
#define CPM_LCR			(CPM_BASE+0x04)
#define CPM_PSWCST(n)		(CPM_BASE+0x4*(n)+0x90)
#define CPM_CLKGR0		(CPM_BASE+0x20) /* Clock Gate Register0 */
#define CPM_CLKGR1		(CPM_BASE+0x28) /* Clock Gate Register1 */
#define CPM_OPCR		(CPM_BASE+0x24) /* Oscillator and Power Control Register */
#define CPM_RSR			(CPM_BASE+0x08)



/* BCH clock divider register */
#define CPM_BCHCDR_BPCS			(1 << 31)
#define CPM_BCHCDR_BCHM			(1 << 30) //0: hardware as default, 1: software(must change)
#define CPM_BCHCDR_BCHDIV_BIT		0
#define CPM_BCHCDR_BCHDIV_MASK		(0x7 << CPM_BCHCDR_BCHDIV_BIT)

#define __cpm_get_bchdiv(n) \
	((REG_CPM_BCHCDR(n) & CPM_BCHCDR_BCHDIV_MASK) >> CPM_BCHCDR_BCHDIV_BIT)
#define __cpm_set_bchdiv(v) \
	(REG_CPM_BCHCDR = (REG_CPM_BCHCDR & ~CPM_BCHCDR_BCHDIV_MASK) | ((v) << (CPM_BCHCDR_BCHDIV_BIT)))
#define __cpm_sw_bchm()			\
        (REG_CPM_BCHCDR |= CPM_BCHCDR_BCHM)
#define __cpm_hw_bchm()			\
        (REG_CPM_BCHCDR &= ~CPM_BCHCDR_BCHM)


#define REG_CPM_CPCCR		REG32(CPM_CPCCR)
#define REG_CPM_CPPCR		REG32(CPM_CPPCR)
#define REG_CPM_CPPSR		REG32(CPM_CPPSR)
#define REG_CPM_CPPCR1		REG32(CPM_CPPCR1)
#define REG_CPM_CPSPR		REG32(CPM_CPSPR)
#define REG_CPM_CPSPPR		REG32(CPM_CPSPPR)
#define REG_CPM_USBPCR		REG32(CPM_USBPCR)
#define REG_CPM_USBRDT		REG32(CPM_USBRDT)
#define REG_CPM_USBVBFIL	REG32(CPM_USBVBFIL)
#define REG_CPM_USBPCR1         REG32(CPM_USBPCR1)
#define REG_CPM_USBCDR		REG32(CPM_USBCDR)
#define REG_CPM_I2SCDR		REG32(CPM_I2SCDR)
#define REG_CPM_LPCDR		REG32(CPM_LPCDR)
#define REG_CPM_MSCCDR		REG32(CPM_MSCCDR)
#define REG_CPM_UHCCDR		REG32(CPM_UHCCDR)
#define REG_CPM_SSICDR		REG32(CPM_SSICDR)
#define REG_CPM_CIMCDR		REG32(CPM_CIMCDR)
#define REG_CPM_GPSCDR		REG32(CPM_GPSCDR)
#define REG_CPM_PCMCDR		REG32(CPM_PCMCDR)
#define REG_CPM_GPUCDR		REG32(CPM_GPUCDR)
#define REG_CPM_MSC1CDR         REG32(CPM_MSC1CDR) 		
#define REG_CPM_MSC2CDR         REG32(CPM_MSC2CDR)
#define REG_CPM_BCHCDR          REG32(CPM_BCHCDR)
#define REG_CPM_INTR            REG32(CPM_INTR)
#define REG_CPM_INTRE           REG32(CPM_INTRE)
#define REG_CPM_LCR		REG32(CPM_LCR)
#define REG_CPM_CLKGR0		REG32(CPM_CLKGR0)
#define REG_CPM_CLKGR1		REG32(CPM_CLKGR1)
#define REG_CPM_OPCR		REG32(CPM_OPCR)
#define REG_CPM_RSR		REG32(CPM_RSR)

/* Clock control register */
#define CPM_CPCCR_MEM			(0x01 << 30)
#define CPM_CPCCR_H1DIV_BIT		24
#define CPM_CPCCR_H1DIV_MASK		(0x0f << CPM_CPCCR_H1DIV_BIT)
#define CPM_CPCCR_CE			(0x01 << 22)
#define CPM_CPCCR_PCS			(0x01 << 21)
#define CPM_CPCCR_H2DIV_BIT		16
#define CPM_CPCCR_H2DIV_MASK		(0x0f << CPM_CPCCR_H2DIV_BIT)
#define CPM_CPCCR_C1DIV_BIT		12
#define CPM_CPCCR_C1DIV_MASK		(0x0f << CPM_CPCCR_C1DIV_BIT)
#define CPM_CPCCR_PDIV_BIT		8
#define CPM_CPCCR_PDIV_MASK		(0x0f << CPM_CPCCR_PDIV_BIT)
#define CPM_CPCCR_H0DIV_BIT		4
#define CPM_CPCCR_HDIV_MASK		(0x0f << CPM_CPCCR_H0DIV_BIT)
#define CPM_CPCCR_H0DIV_MASK		(0x0f << CPM_CPCCR_H0DIV_BIT)
#define CPM_CPCCR_CDIV_BIT		0
#define CPM_CPCCR_CDIV_MASK		(0x0f << CPM_CPCCR_CDIV_BIT)

/* PLL control register 0 */
#define CPM_CPPCR_PLL_BS_BIT            31 /* 1:high band 0:low band*/ 
#define CPM_CPPCR_PLLM_BS               (1 << 31) /* 1:high band 0:low band*/ 
#define CPM_CPPCR_PLLM_BIT		24
#define CPM_CPPCR_PLLM_MASK		(0x7f << CPM_CPPCR_PLLM_BIT)
#define CPM_CPPCR_PLLN_BIT		18
#define CPM_CPPCR_PLLN_MASK		(0x1f << CPM_CPPCR_PLLN_BIT)
#define CPM_CPPCR_PLLOD_BIT		16
#define CPM_CPPCR_PLLOD_MASK		(0x03 << CPM_CPPCR_PLLOD_BIT)
#define CPM_CPPCR_LOCK0			(1 << 15)
#define CPM_CPPCR_ENLOCK		(1 << 14)
#define CPM_CPPCR_PLLS			(1 << 10)
#define CPM_CPPCR_PLLBP			(1 << 9)
#define CPM_CPPCR_PLLEN			(1 << 8)
#define CPM_CPPCR_PLLST_BIT		0
#define CPM_CPPCR_PLLST_MASK		(0xff << CPM_CPPCR_PLLST_BIT)

/* PLL control register 1 */
#define CPM_CPPCR_PLL1M_BS               (1 << 31) /* 1:high band 0:low band*/
#define CPM_CPPCR1_PLL1M_BIT		24
#define CPM_CPPCR1_PLL1M_MASK		(0x7f << CPM_CPPCR1_PLL1M_BIT)
#define CPM_CPPCR1_PLL1N_BIT		18
#define CPM_CPPCR1_PLL1N_MASK		(0x1f << CPM_CPPCR1_PLL1N_BIT)
#define CPM_CPPCR1_PLL1OD_BIT		16
#define CPM_CPPCR1_PLL1OD_MASK		(0x03 << CPM_CPPCR1_PLL1OD_BIT)
#define CPM_CPPCR1_P1SCS		(1 << 15)
#define CPM_CPPCR1_P1SDIV_BIT		8
#define CPM_CPPCR1_P1SDIV_MASK		(0x3f << CPM_CPPCR1_P1SDIV_BIT)
#define CPM_CPPCR1_PLL1EN		(1 << 7)
#define CPM_CPPCR1_PLL1S		(1 << 6)
#define CPM_CPPCR1_LOCK1		(1 << 2)
#define CPM_CPPCR1_PLL1OFF		(1 << 1)
#define CPM_CPPCR1_PLL1ON		(1 << 0)

/* PLL switch and status Register */
#define CPM_CPPSR_PLLOFF		(1 << 31)
#define CPM_CPPSR_PLLBP			(1 << 30)
#define CPM_CPPSR_PLLON			(1 << 29)
#define CPM_CPPSR_PS			(1 << 28)
#define CPM_CPPSR_FS			(1 << 27)
#define CPM_CPPSR_CS			(1 << 26)
#define CPM_CPPSR_SM			(1 << 2)
#define CPM_CPPSR_PM			(1 << 1)
#define CPM_CPPSR_FM			(1 << 0)

/* CPM scratch protected register */
#define CPM_CPSPPR_BIT			0
#define CPM_CPSPPR_MASK			(0xffff << CPM_CPSPPR_BIT)

/* USB parameter control register */
#define CPM_USBPCR_USB_MODE		(1 << 31)  /* 1: OTG, 0: UDC*/
#define CPM_USBPCR_AVLD_REG		(1 << 30)  
#define CPM_USBPCR_IDPULLUP_MASK_BIT	28  
#define CPM_USBPCR_IDPULLUP_MASK_MASK	(0x03 << IDPULLUP_MASK_BIT)
#define CPM_USBPCR_INCR_MASK		(1 << 27)
#define CPM_USBPCR_CLK12_EN		(1 << 26)  
#define CPM_USBPCR_COMMONONN		(1 << 25)  
#define CPM_USBPCR_VBUSVLDEXT		(1 << 24)  
#define CPM_USBPCR_VBUSVLDEXTSEL	(1 << 23)  
#define CPM_USBPCR_POR			(1 << 22)  
#define CPM_USBPCR_SIDDQ		(1 << 21)  
#define CPM_USBPCR_OTG_DISABLE		(1 << 20)  
#define CPM_USBPCR_COMPDISTUNE_BIT	17  
#define CPM_USBPCR_COMPDISTUNE_MASK	(0x07 << COMPDISTUNE_BIT)
#define CPM_USBPCR_OTGTUNE_BIT		14  
#define CPM_USBPCR_OTGTUNE_MASK		(0x07 << OTGTUNE_BIT)
#define CPM_USBPCR_SQRXTUNE_BIT		11  
#define CPM_USBPCR_SQRXTUNE_MASK	(0x07 << SQRXTUNE_BIT)
#define CPM_USBPCR_TXFSLSTUNE_BIT	7  
#define CPM_USBPCR_TXFSLSTUNE_MASK	(0x0f << TXFSLSTUNE_BIT)
#define CPM_USBPCR_TXPREEMPHTUNE	(1 << 6)  
#define CPM_USBPCR_TXRISETUNE_BIT	4  
#define CPM_USBPCR_TXRISETUNE_MASK	(0x03 << TXRISETUNE_BIT)
#define CPM_USBPCR_TXVREFTUNE_BIT	0  
#define CPM_USBPCR_TXVREFTUNE_MASK	(0x0f << TXVREFTUNE_BIT)

/* USB reset detect timer register */
#define USBRDT_HB_MASK                  (1 << 26) /* halfword/byte transfer mask 1:mask 0:enable */
#define CPM_USBRDT_VBFIL_LD_EN		(1 << 25)  
#define CPM_USBRDT_IDDIG_EN		(1 << 24)  
#define CPM_USBRDT_IDDIG_REG		(1 << 23)  
#define CPM_USBRDT_USBRDT_BIT		0  
#define CPM_USBRDT_USBRDT_MASK		(0x7fffff << CPM_USBRDT_USBRDT_BIT)

/* USB OTG PHY clock divider register */
#define CPM_USBCDR_UCS			(1 << 31)
#define CPM_USBCDR_UPCS			(1 << 30)
#define CPM_USBCDR_OTGDIV_BIT		0
#define CPM_USBCDR_OTGDIV_MASK		(0xff << CPM_USBCDR_OTGDIV_BIT)

/* I2S device clock divider register */
#define CPM_I2SCDR_I2CS			(1 << 31)
#define CPM_I2SCDR_I2PCS		(1 << 30)
#define CPM_I2SCDR_I2SDIV_BIT		0
#define CPM_I2SCDR_I2SDIV_MASK		(0x1ff << CPM_I2SCDR_I2SDIV_BIT)

/* LCD pix clock divider register */
#define CPM_LPCDR_LSCS			(1 << 31)
#define CPM_LPCDR_LTCS			(1 << 30)
#define CPM_LPCDR_LPCS			(1 << 29)
#define CPM_LPCDR_PIXDIV_BIT		0
#define CPM_LPCDR_PIXDIV_MASK		(0x7ff << CPM_LPCDR_PIXDIV_BIT)

/* MSC clock divider register */
#define CPM_MSCCDR_MCS			(1 << 31)
#define CPM_MSCCDR_MPCS			(1 << 30) /* 0:PLL0 1:PLL1 clock output */
#define CPM_MSCCDR_MSCDIV_BIT		0
#define CPM_MSCCDR_MSCDIV_MASK		(0x3f << CPM_MSCCDR_MSCDIV_BIT)

/* UHC 48M clock divider register */
#define CPM_UHCCDR_UHCS_BIT		30
#define CPM_UHCCDR_UHCS_MASK		(0x03 << CPM_UHCCDR_UHCS_BIT)
#define CPM_UHCCDR_UHPCS                (1 << 29) /* 0:PLL0 1:PLL1 clock output */
#define CPM_UHCCDR_UHCDIV_BIT		0
#define CPM_UHCCDR_UHCDIV_MASK		(0xf << CPM_UHCCDR_UHCDIV_BIT)

/* SSI clock divider register */
#define CPM_SSICDR_SCS			(1 << 31)
#define CPM_SSICDR_SPCS                 (1 << 30) /* 0:PLL0 1:PLL1 clock output */
#define CPM_SSICDR_SSIDIV_BIT		0
#define CPM_SSICDR_SSIDIV_MASK		(0x3f << CPM_SSICDR_SSIDIV_BIT)

/* CIM MCLK clock divider register */
#define CPM_CIMCDR_CIMDIV_BIT		0
#define CPM_CIMCDR_CIMDIV_MASK		(0xff << CPM_CIMCDR_CIMDIV_BIT)

/* GPS clock divider register */
#define CPM_GPSCDR_GPCS			(1 << 31)
#define CPM_GPSCDR_GPSDIV_BIT		0
#define CPM_GSPCDR_GPSDIV_MASK		(0xf << CPM_GPSCDR_GPSDIV_BIT)

/* PCM device clock divider register */
#define CPM_PCMCDR_PCMS			(1 << 31)
#define CPM_PCMCDR_PCMPCS		(1 << 30)
#define CPM_PCMCDR_PCMDIV_BIT		0
#define CPM_PCMCDR_PCMDIV_MASK		(0x1ff << CPM_PCMCDR_PCMDIV_BIT)

/* GPU clock divider register */
#define CPM_GPUCDR_GPCS			(1 << 31)
#define CPM_GPUCDR_GPUDIV_BIT		0
#define CPM_GPUCDR_GPUDIV_MASK		(0x1f << CPM_GPUCDR_GPUDIV_BIT)

/* Low Power Control Register */
#define CPM_LCR_PD_AHB1		(1 << 30)
#define CPM_LCR_VBAT_IR		(1 << 29)
#define CPM_LCR_PD_GPS		(1 << 28)
#define CPM_LCR_PD_AHB1S	(1 << 26)
#define CPM_LCR_PD_GPSS		(1 << 24)
#define CPM_LCR_PST_BIT 	8
#define CPM_LCR_PST_MASK 	(0xfff << CPM_LCR_PST_BIT)
#define CPM_LCR_DOZE_DUTY_BIT 	3
#define CPM_LCR_DOZE_DUTY_MASK 	(0x1f << CPM_LCR_DOZE_DUTY_BIT)
#define CPM_LCR_DOZE_ON		(1 << 2)
#define CPM_LCR_LPM_BIT		0
#define CPM_LCR_LPM_MASK	(0x3 << CPM_LCR_LPM_BIT)
#define CPM_LCR_LPM_IDLE	(0x0 << CPM_LCR_LPM_BIT)
#define CPM_LCR_LPM_SLEEP	(0x1 << CPM_LCR_LPM_BIT)

/* Clock Gate Register0 */
#define CPM_CLKGR0_AHB_MON	(1 << 31)
#define CPM_CLKGR0_DDR  	(1 << 30)
#define CPM_CLKGR0_IPU    	(1 << 29)
#define CPM_CLKGR0_LCD		(1 << 28)
#define CPM_CLKGR0_TVE  	(1 << 27)
#define CPM_CLKGR0_CIM    	(1 << 26)
#define CPM_CLKGR0_MDMA    	(1 << 25)
#define CPM_CLKGR0_UHC    	(1 << 24)
#define CPM_CLKGR0_MAC    	(1 << 23)
#define CPM_CLKGR0_GPS    	(1 << 22)
#define CPM_CLKGR0_DMAC    	(1 << 21)
#define CPM_CLKGR0_SSI2    	(1 << 20)
#define CPM_CLKGR0_SSI1    	(1 << 19)
#define CPM_CLKGR0_UART3    	(1 << 18)
#define CPM_CLKGR0_UART2    	(1 << 17)
#define CPM_CLKGR0_UART1    	(1 << 16)
#define CPM_CLKGR0_UART0	(1 << 15)
#define CPM_CLKGR0_SADC		(1 << 14)
#define CPM_CLKGR0_KBC		(1 << 13)
#define CPM_CLKGR0_MSC2		(1 << 12)
#define CPM_CLKGR0_MSC1		(1 << 11)
#define CPM_CLKGR0_OWI		(1 << 10)
#define CPM_CLKGR0_TSSI		(1 << 9)
#define CPM_CLKGR0_AIC		(1 << 8)
#define CPM_CLKGR0_SCC		(1 << 7)
#define CPM_CLKGR0_I2C1		(1 << 6)
#define CPM_CLKGR0_I2C0		(1 << 5)
#define CPM_CLKGR0_SSI0		(1 << 4)
#define CPM_CLKGR0_MSC0		(1 << 3)
#define CPM_CLKGR0_OTG		(1 << 2)
#define CPM_CLKGR0_BCH		(1 << 1)
#define CPM_CLKGR0_NEMC		(1 << 0)

/* Clock Gate Register1 */
#define CPM_CLKGR1_I2C2		(1 << 15)
#define CPM_CLKGR1_AUX		(1 << 14)
#define CPM_CLKGR1_I2S2CH	(1 << 13)
#define CPM_CLKGR1_OSD		(1 << 12)
#define CPM_CLKGR1_UART4	(1 << 11)
#define CPM_CLKGR1_PCM1		(1 << 10)
#define CPM_CLKGR1_GPU		(1 << 9)
#define CPM_CLKGR1_PCM0		(1 << 8)
#define CPM_CLKGR1_VPU		(1 << 7)
#define CPM_CLKGR1_CABAC	(1 << 6)
#define CPM_CLKGR1_SRAM		(1 << 5)
#define CPM_CLKGR1_DCT		(1 << 4)
#define CPM_CLKGR1_ME		(1 << 3)
#define CPM_CLKGR1_DBLK		(1 << 2)
#define CPM_CLKGR1_MC		(1 << 1)
#define CPM_CLKGR1_BDMA		(1 << 0)

/* Oscillator and Power Control Register */
#define CPM_OPCR_O1ST_BIT	8
#define CPM_OPCR_O1ST_MASK	(0xff << CPM_OPCR_O1ST_BIT)
#define CPM_OPCR_SPENDN		(1 << 7)
#define CPM_OPCR_GPSEN		(1 << 6)
#define CPM_OPCR_SPENDH		(1 << 5)
#define CPM_OPCR_O1SE		(1 << 4) /* 1:enable EXCLK osillator in sleep mode  */
#define CPM_OPCR_PD		(1 << 3) /* 1: the P0 power down in sleep mode */
#define CPM_OPCR_ERCS           (1 << 2) /* 0: select EXCLK/512 clock, 1: RTCLK clock */
#define CPM_OPCR_USBM           (1 << 0) /* 0: select EXCLK/512 clock, 1: RTCLK clock */

/* Reset Status Register */
#define CPM_RSR_P0R		(1 << 2)
#define CPM_RSR_WR		(1 << 1)
#define CPM_RSR_PR		(1 << 0)

/* CPM scratch pad protected register(CPSPPR) */
#define CPSPPR_CPSPR_WRITABLE   (0x00005a5a)
#define RECOVERY_SIGNATURE      0x52454359      /* means "RECY" */
#define RECOVERY_SIGNATURE_SEC  0x800      /* means "RECY" */

#define cpm_get_scrpad()	REG_CPM_CPSPR
#define cpm_set_scrpad(data)				\
do {							\
	REG_CPM_CPSPPR = CPSPPR_CPSPR_WRITABLE;		\
	REG_CPM_CPSPR = data;				\
	REG_CPM_CPSPPR = ~CPSPPR_CPSPR_WRITABLE;	\
} while (0)

#define __cpm_get_pllm() \
	((REG_CPM_CPPCR & CPM_CPPCR_PLLM_MASK) >> CPM_CPPCR_PLLM_BIT)
#define __cpm_get_plln() \
	((REG_CPM_CPPCR & CPM_CPPCR_PLLN_MASK) >> CPM_CPPCR_PLLN_BIT)
#define __cpm_get_pllod() \
	((REG_CPM_CPPCR & CPM_CPPCR_PLLOD_MASK) >> CPM_CPPCR_PLLOD_BIT)

#define __cpm_get_pll1m() \
	((REG_CPM_CPPCR1 & CPM_CPPCR1_PLL1M_MASK) >> CPM_CPPCR1_PLL1M_BIT)
#define __cpm_get_pll1n() \
	((REG_CPM_CPPCR1 & CPM_CPPCR1_PLL1N_MASK) >> CPM_CPPCR1_PLL1N_BIT)
#define __cpm_get_pll1od() \
	((REG_CPM_CPPCR1 & CPM_CPPCR1_PLL1OD_MASK) >> CPM_CPPCR1_PLL1OD_BIT)

#define __cpm_get_cdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_CDIV_MASK) >> CPM_CPCCR_CDIV_BIT)
#define __cpm_get_hdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_HDIV_MASK) >> CPM_CPCCR_H0DIV_BIT)
#define __cpm_get_h2div() \
	((REG_CPM_CPCCR & CPM_CPCCR_H2DIV_MASK) >> CPM_CPCCR_H2DIV_BIT)
#define __cpm_get_h1div() \
	((REG_CPM_CPCCR & CPM_CPCCR_H1DIV_MASK) >> CPM_CPCCR_H1DIV_BIT)
#define __cpm_get_pdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_PDIV_MASK) >> CPM_CPCCR_PDIV_BIT)
#define __cpm_get_mdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_H0DIV_MASK) >> CPM_CPCCR_H0DIV_BIT)
#define __cpm_get_sdiv() \
	((REG_CPM_CPCCR & CPM_CPCCR_SDIV_MASK) >> CPM_CPCCR_SDIV_BIT)
#define __cpm_get_i2sdiv() \
	((REG_CPM_I2SCDR & CPM_I2SCDR_I2SDIV_MASK) >> CPM_I2SCDR_I2SDIV_BIT)
#define __cpm_get_pixdiv() \
	((REG_CPM_LPCDR & CPM_LPCDR_PIXDIV_MASK) >> CPM_LPCDR_PIXDIV_BIT)
#define __cpm_get_mscdiv(n) \
	((REG_CPM_MSCCDR(n) & CPM_MSCCDR_MSCDIV_MASK) >> CPM_MSCCDR_MSCDIV_BIT)
#define __cpm_get_ssidiv() \
	((REG_CPM_SSICCDR & CPM_SSICDR_SSICDIV_MASK) >> CPM_SSICDR_SSIDIV_BIT)
#define __cpm_get_pcmdiv() \
	((REG_CPM_PCMCDR & CPM_PCMCDR_PCMCD_MASK) >> CPM_PCMCDR_PCMCD_BIT)
#define __cpm_get_pll1div() \
	((REG_CPM_CPPCR1 & CPM_CPCCR1_P1SDIV_MASK) >> CPM_CPCCR1_P1SDIV_BIT)

#define __cpm_set_cdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_CDIV_MASK) | ((v) << (CPM_CPCCR_CDIV_BIT)))
#define __cpm_set_hdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_HDIV_MASK) | ((v) << (CPM_CPCCR_HDIV_BIT)))
#define __cpm_set_pdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_PDIV_MASK) | ((v) << (CPM_CPCCR_PDIV_BIT)))
#define __cpm_set_mdiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_MDIV_MASK) | ((v) << (CPM_CPCCR_MDIV_BIT)))
#define __cpm_set_h1div(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_H1DIV_MASK) | ((v) << (CPM_CPCCR_H1DIV_BIT)))
#define __cpm_set_udiv(v) \
	(REG_CPM_CPCCR = (REG_CPM_CPCCR & ~CPM_CPCCR_UDIV_MASK) | ((v) << (CPM_CPCCR_UDIV_BIT)))
#define __cpm_set_i2sdiv(v) \
	(REG_CPM_I2SCDR = (REG_CPM_I2SCDR & ~CPM_I2SCDR_I2SDIV_MASK) | ((v) << (CPM_I2SCDR_I2SDIV_BIT)))
#define __cpm_set_pixdiv(v) \
	(REG_CPM_LPCDR = (REG_CPM_LPCDR & ~CPM_LPCDR_PIXDIV_MASK) | ((v) << (CPM_LPCDR_PIXDIV_BIT)))
#define __cpm_set_mscdiv(v) \
	(REG_CPM_MSCCDR = (REG_CPM_MSCCDR & ~CPM_MSCCDR_MSCDIV_MASK) | ((v) << (CPM_MSCCDR_MSCDIV_BIT)))
#define __cpm_set_ssidiv(v) \
	(REG_CPM_SSICDR = (REG_CPM_SSICDR & ~CPM_SSICDR_SSIDIV_MASK) | ((v) << (CPM_SSICDR_SSIDIV_BIT)))
#define __cpm_set_pcmdiv(v) \
	(REG_CPM_PCMCDR = (REG_CPM_PCMCDR & ~CPM_PCMCDR_PCMCD_MASK) | ((v) << (CPM_PCMCDR_PCMCD_BIT)))
#define __cpm_set_pll1div(v) \
	(REG_CPM_CPPCR1 = (REG_CPM_CPPCR1 & ~CPM_CPCCR1_P1SDIV_MASK) | ((v) << (CPM_CPCCR1_P1SDIV_BIT)))

#define __cpm_select_i2sclk_pll1() 	(REG_CPM_I2SCDR |= CPM_I2SCDR_I2PCS)
#define __cpm_select_i2sclk_pll0()	(REG_CPM_I2SCDR &= ~CPM_I2SCDR_I2PCS)
#define __cpm_select_otgclk_pll1() 	(REG_CPM_USBCDR |= CPM_USBCDR_UPCS)
#define __cpm_select_otgclk_pll0()	(REG_CPM_USBCDR &= ~CPM_USBCDR_UPCS)
#define __cpm_select_lcdpclk_pll1() 	(REG_CPM_LPCDR |= CPM_LPCDR_LPCS)
#define __cpm_select_lcdpclk_pll0()	(REG_CPM_LPCDR &= ~CPM_LPCDR_LPCS)
#define __cpm_select_uhcclk_pll1() 	(REG_CPM_UHCCDR |= CPM_UHCCDR_UHPCS)
#define __cpm_select_uhcclk_pll0()	(REG_CPM_UHCCDR &= ~CPM_UHCCDR_UHPCS)
#define __cpm_select_gpsclk_pll1() 	(REG_CPM_GPSCDR |= CPM_GPSCDR_GPCS)
#define __cpm_select_gpsclk_pll0()	(REG_CPM_GPSCDR &= ~CPM_GPSCDR_GPCS)
#define __cpm_select_pcmclk_pll1() 	(REG_CPM_PCMCDR |= CPM_PCMCDR_PCMPCS)
#define __cpm_select_pcmclk_pll0()	(REG_CPM_PCMCDR &= ~CPM_PCMCDR_PCMPCS)
#define __cpm_select_gpuclk_pll1() 	(REG_CPM_GPUCDR |= CPM_GPUCDR_GPCS)
#define __cpm_select_gpuclk_pll0()	(REG_CPM_GPUCDR &= ~CPM_GPUCDR_GPCS)
#define __cpm_select_clk_pll1() 	(REG_CPM_CDR |= CPM_CDR_PCS)
#define __cpm_select_clk_pll0()	(REG_CPM_CDR &= ~CPM_CDR_PCS)


#define __cpm_select_pcmclk_pll() 	(REG_CPM_PCMCDR |= CPM_PCMCDR_PCMS)
#define __cpm_select_pcmclk_exclk() 	(REG_CPM_PCMCDR &= ~CPM_PCMCDR_PCMS)
#define __cpm_select_pixclk_ext()	(REG_CPM_LPCDR |= CPM_LPCDR_LPCS)
#define __cpm_select_pixclk_pll()	(REG_CPM_LPCDR &= ~CPM_LPCDR_LPCS)
#define __cpm_select_tveclk_exclk()	(REG_CPM_LPCDR |= CPM_CPCCR_LSCS)
#define __cpm_select_tveclk_pll()	(REG_CPM_LPCDR &= ~CPM_LPCDR_LSCS)
#define __cpm_select_pixclk_lcd()	(REG_CPM_LPCDR &= ~CPM_LPCDR_LTCS)
#define __cpm_select_pixclk_tve()	(REG_CPM_LPCDR |= CPM_LPCDR_LTCS)
#define __cpm_select_i2sclk_exclk()	(REG_CPM_I2SCDR &= ~CPM_I2SCDR_I2CS)
#define __cpm_select_i2sclk_pll()	(REG_CPM_I2SCDR |= CPM_I2SCDR_I2CS)
//#define __cpm_select_usbclk_exclk()	(REG_CPM_CPCCR &= ~CPM_CPCCR_UCS)
//#define __cpm_select_usbclk_pll()	(REG_CPM_CPCCR |= CPM_CPCCR_UCS)

#define __cpm_enable_cko()
#define __cpm_exclk_direct()		(REG_CPM_CPCCR &= ~CPM_CPCCR_ECS)
#define __cpm_exclk_div2()             	(REG_CPM_CPCCR |= CPM_CPCCR_ECS)
#define __cpm_enable_pll_change()	(REG_CPM_CPCCR |= CPM_CPCCR_CE)

#define __cpm_pllout_div2()		(REG_CPM_CPCCR &= ~CPM_CPCCR_PCS)
#define __cpm_pll_enable()		(REG_CPM_CPPCR |= CPM_CPPCR_PLLEN)

#define __cpm_pll1_enable()		(REG_CPM_CPPCR1 |= CPM_CPPCR1_PLL1EN)

#define __cpm_pll_is_off()		(REG_CPM_CPPSR & CPM_CPPSR_PLLOFF)
#define __cpm_pll_is_on()		(REG_CPM_CPPSR & CPM_CPPSR_PLLON)
#define __cpm_pll_bypass()		(REG_CPM_CPPSR |= CPM_CPPSR_PLLBP)

#define __cpm_get_cclk_doze_duty() \
	((REG_CPM_LCR & CPM_LCR_DOZE_DUTY_MASK) >> CPM_LCR_DOZE_DUTY_BIT)
#define __cpm_set_cclk_doze_duty(v) \
	(REG_CPM_LCR = (REG_CPM_LCR & ~CPM_LCR_DOZE_DUTY_MASK) | ((v) << (CPM_LCR_DOZE_DUTY_BIT)))

#define __cpm_doze_mode()		(REG_CPM_LCR |= CPM_LCR_DOZE_ON)
#define __cpm_idle_mode() \
	(REG_CPM_LCR = (REG_CPM_LCR & ~CPM_LCR_LPM_MASK) | CPM_LCR_LPM_IDLE)
#define __cpm_sleep_mode() \
	(REG_CPM_LCR = (REG_CPM_LCR & ~CPM_LCR_LPM_MASK) | CPM_LCR_LPM_SLEEP)

#define __cpm_stop_all() 	\
	do {\
		(REG_CPM_CLKGR0 = 0xffffffff);\
		(REG_CPM_CLKGR1 = 0x3ff);\
	}while(0)
#define __cpm_stop_emc()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_EMC)
#define __cpm_stop_ddr()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_DDR)
#define __cpm_stop_ipu()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_IPU)
#define __cpm_stop_lcd()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_LCD)
#define __cpm_stop_tve()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_TVE)
#define __cpm_stop_Cim()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_CIM)
#define __cpm_stop_mdma()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_MDMA)
#define __cpm_stop_uhc()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_UHC)
#define __cpm_stop_mac()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_MAC)
#define __cpm_stop_gps()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_GPS)
#define __cpm_stop_dmac()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_DMAC)
#define __cpm_stop_ssi2()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_SSI2)
#define __cpm_stop_ssi1()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_SSI1)
#define __cpm_stop_uart3()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_UART3)
#define __cpm_stop_uart2()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_UART2)
#define __cpm_stop_uart1()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_UART1)
#define __cpm_stop_uart0()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_UART0)
#define __cpm_stop_sadc()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_SADC)
#define __cpm_stop_kbc()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_KBC)
#define __cpm_stop_msc2()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_MSC2)
#define __cpm_stop_msc1()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_MSC1)
#define __cpm_stop_owi()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_OWI)
#define __cpm_stop_tssi()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_TSSI)
#define __cpm_stop_aic()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_AIC)
#define __cpm_stop_scc()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_SCC)
#define __cpm_stop_i2c0()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_I2C1)
#define __cpm_stop_i2c1()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_I2C0)
#define __cpm_stop_ssi0()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_SSI0)
#define __cpm_stop_msc0()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_MSC0)
#define __cpm_stop_otg()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_OTG)
#define __cpm_stop_bch()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_BCH)
#define __cpm_stop_nemc()	(REG_CPM_CLKGR0 |= CPM_CLKGR0_NEMC)
#define __cpm_stop_gpu()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_GPU)
#define __cpm_stop_pcm()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_PCM)
#define __cpm_stop_ahb1()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_AHB1)
#define __cpm_stop_cabac()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_CABAC)
#define __cpm_stop_sram()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_SRAM)
#define __cpm_stop_dct()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_DCT)
#define __cpm_stop_me()		(REG_CPM_CLKGR1 |= CPM_CLKGR0_ME)
#define __cpm_stop_dblk()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_DBLK)
#define __cpm_stop_mc()		(REG_CPM_CLKGR1 |= CPM_CLKGR0_MC)
#define __cpm_stop_bdma()	(REG_CPM_CLKGR1 |= CPM_CLKGR0_BDMA)

#define __cpm_start_all() 	\
	do {\
		REG_CPM_CLKGR0 = 0x0;\
		REG_CPM_CLKGR1 = 0x0;\
	} while(0)
#define __cpm_start_emc()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_EMC)
#define __cpm_start_ddr()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_DDR)
#define __cpm_start_ipu()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_IPU)
#define __cpm_start_lcd()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_LCD)
#define __cpm_start_tve()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_TVE)
#define __cpm_start_Cim()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_CIM)
#define __cpm_start_mdma()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_MDMA)
#define __cpm_start_uhc()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_UHC)
#define __cpm_start_mac()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_MAC)
#define __cpm_start_gps()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_GPS)
#define __cpm_start_dmac()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_DMAC)
#define __cpm_start_ssi2()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_SSI2)
#define __cpm_start_ssi1()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_SSI1)
#define __cpm_start_uart3()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_UART3)
#define __cpm_start_uart2()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_UART2)
#define __cpm_start_uart1()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_UART1)
#define __cpm_start_uart0()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_UART0)
#define __cpm_start_sadc()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_SADC)
#define __cpm_start_kbc()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_KBC)
#define __cpm_start_msc2()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_MSC2)
#define __cpm_start_msc1()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_MSC1)
#define __cpm_start_owi()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_OWI)
#define __cpm_start_tssi()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_TSSI)
#define __cpm_start_aic()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_AIC)
#define __cpm_start_scc()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_SCC)
#define __cpm_start_i2c0()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_I2C1)
#define __cpm_start_i2c1()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_I2C0)
#define __cpm_start_ssi0()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_SSI0)
#define __cpm_start_msc0()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_MSC0)
#define __cpm_start_otg()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_OTG)
#define __cpm_start_bch()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_BCH)
#define __cpm_start_nemc()	(REG_CPM_CLKGR0 &= ~CPM_CLKGR0_NEMC)
#define __cpm_start_gpu()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_GPU)
#define __cpm_start_pcm()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_PCM)
#define __cpm_start_ahb1()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_AHB1)
#define __cpm_start_cabac()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_CABAC)
#define __cpm_start_sram()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_SRAM)
#define __cpm_start_dct()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_DCT)
#define __cpm_start_me()		(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_ME)
#define __cpm_start_dblk()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_DBLK)
#define __cpm_start_mc()		(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_MC)
#define __cpm_start_bdma()	(REG_CPM_CLKGR1 &= ~CPM_CLKGR0_BDMA)

#define __cpm_get_o1st() \
	((REG_CPM_OPCR & CPM_OPCR_O1ST_MASK) >> CPM_OPCR_O1ST_BIT)
#define __cpm_set_o1st(v) \
	(REG_CPM_OPCR = (REG_CPM_OPCR & ~CPM_OPCR_O1ST_MASK) | ((v) << (CPM_OPCR_O1ST_BIT)))
#define __cpm_suspend_udcphy()		(REG_CPM_OPCR &= ~CPM_OPCR_UDCPHY_ENABLE)
#define __cpm_enable_osc_in_sleep()	(REG_CPM_OPCR |= CPM_OPCR_OSC_ENABLE)
#define __cpm_select_rtcclk_rtc()	(REG_CPM_OPCR |= CPM_OPCR_ERCS)
#define __cpm_select_rtcclk_exclk()	(REG_CPM_OPCR &= ~CPM_OPCR_ERCS)

#ifdef CFG_EXTAL
#define JZ_EXTAL		CFG_EXTAL
#else
#define JZ_EXTAL		12000000
#endif
#define JZ_EXTAL2		32768 /* RTC clock */

/* PLL output frequency */
static __inline__ unsigned int __cpm_get_pllout(void)
{
	unsigned long m, n, no, pllout;
	unsigned long cppcr = REG_CPM_CPPCR;
	static const u8 od[4] = {1, 2, 4, 8};
	if ((cppcr & CPM_CPPCR_PLLEN) && (!(cppcr & CPM_CPPCR_PLLBP))) {
		m = __cpm_get_pllm() + 1;
		n = __cpm_get_plln() + 1;
		no = od[__cpm_get_pllod()];
		pllout = ((JZ_EXTAL) * m / (n * no));
	} else
		pllout = JZ_EXTAL;
	return pllout;
}

/* PLL output frequency */
static __inline__ unsigned int __cpm_get_pll1out(void)
{
	unsigned long m, n, no, pllout;
	unsigned long cppcr1 = REG_CPM_CPPCR1;
	static const u8 od[4] = {1, 2, 4, 8};
	if (cppcr1 & CPM_CPPCR1_PLL1EN)
	{
		m = __cpm_get_pll1m() + 1;
		n = __cpm_get_pll1n() + 1;
		no = od[__cpm_get_pll1od()];
		if (cppcr1 & CPM_CPPCR1_P1SCS)
			pllout = ((__cpm_get_pllout()) * m / (n * no));
		else
			pllout = ((JZ_EXTAL) * m / (n * no));

	} else
		pllout = JZ_EXTAL;
	return pllout;
}

/* PLL output frequency for MSC/I2S/LCD/USB */
static __inline__ unsigned int __cpm_get_pllout2(void)
{
	if (REG_CPM_CPCCR & CPM_CPCCR_PCS)
		return __cpm_get_pllout()/2;
	else
		return __cpm_get_pllout();
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

/* AHB1 module clock */
static __inline__ unsigned int __cpm_get_h2clk(void)
{
	static const u8 div[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

	return __cpm_get_pllout() / div[__cpm_get_h2div()];
}

/* LCD pixel clock */
static __inline__ unsigned int __cpm_get_pixclk(void)
{
	return __cpm_get_pllout2() / (__cpm_get_pixdiv() + 1);
}

/* I2S clock */
static __inline__ unsigned int __cpm_get_i2sclk(void)
{
	if (REG_CPM_I2SCDR & CPM_I2SCDR_I2CS) {
		return __cpm_get_pllout2() / (__cpm_get_i2sdiv() + 1);
	}
	else {
		return JZ_EXTAL;
	}
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
static inline void __cpm_select_msc_clk(int n, int sd)
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
	REG_CPM_CPCCR |= CPM_CPCCR_CE;
}

#endif /* __JZ4770_CPM_H__ */
