#ifndef __JZ4760_EMC_H__
#define __JZ4760_EMC_H__

#include "config.h"

#define	EMC_BASE	0xB3010000

#define EMC_LOW_SDRAM_SPACE_SIZE 	0x10000000 /* 256M */
#define EMC_MEM_PHY_BASE 		0x20000000
#define EMC_MEM_PHY_BASE_SHIFT 		24


#define EMC_BCR		(EMC_BASE + 0x0)  /* BCR */

#define EMC_DMCR	(EMC_BASE + 0x80)  /* DRAM Control Register */
#define EMC_RTCSR	(EMC_BASE + 0x84)  /* Refresh Time Control/Status Register */
#define EMC_RTCNT	(EMC_BASE + 0x88)  /* Refresh Timer Counter */
#define EMC_RTCOR	(EMC_BASE + 0x8c)  /* Refresh Time Constant Register */

#define EMC_DMAR0	(EMC_BASE + 0x90)  /* SDRAM Bank 0 Addr Config Register */
#define EMC_DMAR1	(EMC_BASE + 0x94)  /* SDRAM Bank 1 Addr Config Register */
#define EMC_SDMR0	(EMC_BASE + 0x8000) /* Mode Register of SDRAM bank 0 */

#define REG_EMC_BCR	REG32(EMC_BCR)

#define REG_EMC_DMCR	REG32(EMC_DMCR)
#define REG_EMC_RTCSR	REG16(EMC_RTCSR)
#define REG_EMC_RTCNT	REG16(EMC_RTCNT)
#define REG_EMC_RTCOR	REG16(EMC_RTCOR)
#define REG_EMC_DMAR0	REG32(EMC_DMAR0)
#define REG_EMC_DMAR1	REG32(EMC_DMAR1)

#define EMC_PMEMPS0	(EMC_BASE + 0x6000)
#define EMC_PMEMPS1	(EMC_BASE + 0x6004)
#define EMC_PMEMPS2	(EMC_BASE + 0x6008)
#define EMC_PMEMPS3	(EMC_BASE + 0x600c)

#define REG_EMC_PMEMPS0	REG32(EMC_PMEMPS0)
#define REG_EMC_PMEMPS1	REG32(EMC_PMEMPS1)
#define REG_EMC_PMEMPS2	REG32(EMC_PMEMPS2)
#define REG_EMC_PMEMPS3	REG32(EMC_PMEMPS3)

/* DRAM Control Register */
#define EMC_DMCR_BW_BIT		31
#define EMC_DMCR_BW		(1 << EMC_DMCR_BW_BIT)
#define EMC_DMCR_CA_BIT		26
#define EMC_DMCR_CA_MASK	(0x07 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_8		(0 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_9		(1 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_10	(2 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_11	(3 << EMC_DMCR_CA_BIT)
  #define EMC_DMCR_CA_12	(4 << EMC_DMCR_CA_BIT)
#define EMC_DMCR_RMODE		(1 << 25)
#define EMC_DMCR_RFSH		(1 << 24)
#define EMC_DMCR_MRSET		(1 << 23)
#define EMC_DMCR_RA_BIT		20
#define EMC_DMCR_RA_MASK	(0x03 << EMC_DMCR_RA_BIT)
  #define EMC_DMCR_RA_11	(0 << EMC_DMCR_RA_BIT)
  #define EMC_DMCR_RA_12	(1 << EMC_DMCR_RA_BIT)
  #define EMC_DMCR_RA_13	(2 << EMC_DMCR_RA_BIT)
#define EMC_DMCR_BA_BIT		19
#define EMC_DMCR_BA		(1 << EMC_DMCR_BA_BIT)
#define EMC_DMCR_PDM		(1 << 18)
#define EMC_DMCR_EPIN		(1 << 17)
#define EMC_DMCR_MBSEL_BIT	16
  #define EMC_DMCR_MBSEL_B0	(0 << 16)
  #define EMC_DMCR_MBSEL_B1	(1 << 16)
#define EMC_DMCR_TRAS_BIT	13
#define EMC_DMCR_TRAS_MASK	(0x07 << EMC_DMCR_TRAS_BIT)
#define EMC_DMCR_RCD_BIT	11
#define EMC_DMCR_RCD_MASK	(0x03 << EMC_DMCR_RCD_BIT)
#define EMC_DMCR_TPC_BIT	8
#define EMC_DMCR_TPC_MASK	(0x07 << EMC_DMCR_TPC_BIT)
#define EMC_DMCR_TRWL_BIT	5
#define EMC_DMCR_TRWL_MASK	(0x03 << EMC_DMCR_TRWL_BIT)
#define EMC_DMCR_TRC_BIT	2
#define EMC_DMCR_TRC_MASK	(0x07 << EMC_DMCR_TRC_BIT)
#define EMC_DMCR_TCL_BIT	0
#define EMC_DMCR_TCL_MASK	(0x03 << EMC_DMCR_TCL_BIT)

/* Refresh Time Control/Status Register */
#define EMC_RTCSR_CMF		(1 << 7)
#define EMC_RTCSR_CKS_BIT	0
#define EMC_RTCSR_CKS_MASK	(0x07 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_DISABLE	(0 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_4	(1 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_16	(2 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_64	(3 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_256	(4 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_1024	(5 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_2048	(6 << EMC_RTCSR_CKS_BIT)
  #define EMC_RTCSR_CKS_4096	(7 << EMC_RTCSR_CKS_BIT)

/* SDRAM Bank Address Configuration Register */
#define EMC_DMAR_BASE_BIT	8
#define EMC_DMAR_BASE_MASK	(0xff << EMC_DMAR_BASE_BIT)
#define EMC_DMAR_MASK_BIT	0
#define EMC_DMAR_MASK_MASK	(0xff << EMC_DMAR_MASK_BIT)

/* Mode Register of SDRAM bank 0 */
#define EMC_SDMR_BM		(1 << 9) /* Write Single Mode */
#define EMC_SDMR_OM_BIT		7        /* Operating Mode */
#define EMC_SDMR_OM_MASK	(3 << EMC_SDMR_OM_BIT)
  #define EMC_SDMR_OM_NORMAL	(0 << EMC_SDMR_OM_BIT)
#define EMC_SDMR_CAS_BIT	4        /* CAS Latency */
#define EMC_SDMR_CAS_MASK	(7 << EMC_SDMR_CAS_BIT)
  #define EMC_SDMR_CAS_1	(1 << EMC_SDMR_CAS_BIT)
  #define EMC_SDMR_CAS_2	(2 << EMC_SDMR_CAS_BIT)
  #define EMC_SDMR_CAS_3	(3 << EMC_SDMR_CAS_BIT)
#define EMC_SDMR_BT_BIT		3        /* Burst Type */
#define EMC_SDMR_BT_MASK	(1 << EMC_SDMR_BT_BIT)
  #define EMC_SDMR_BT_SEQ	(0 << EMC_SDMR_BT_BIT) /* Sequential */
  #define EMC_SDMR_BT_INT	(1 << EMC_SDMR_BT_BIT) /* Interleave */
#define EMC_SDMR_BL_BIT		0        /* Burst Length */
#define EMC_SDMR_BL_MASK	(7 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_1		(0 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_2		(1 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_4		(2 << EMC_SDMR_BL_BIT)
  #define EMC_SDMR_BL_8		(3 << EMC_SDMR_BL_BIT)

#define EMC_SDMR_CAS2_16BIT \
  (EMC_SDMR_CAS_2 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_2)
#define EMC_SDMR_CAS2_32BIT \
  (EMC_SDMR_CAS_2 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_4)
#define EMC_SDMR_CAS3_16BIT \
  (EMC_SDMR_CAS_3 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_2)
#define EMC_SDMR_CAS3_32BIT \
  (EMC_SDMR_CAS_3 | EMC_SDMR_BT_SEQ | EMC_SDMR_BL_4)

/* Extended Mode Register of Mobile SDRAM*/
#define EMC_SDMR_SET_BA1		(1 << 14)	/*BA1*/
#define EMC_SDMR_SET_BA0		(1 << 13)	/*BA0*/

#define EMC_SDMR_DS_BIT		5	/* Driver strength */
#define EMC_SDMR_DS_MASK	(3 << EMC_SDMR_DS_BIT)
  #define EMC_SDMR_DS_FULL	(0 << EMC_SDMR_DS_BIT)	/*Full*/
  #define EMC_SDMR_DS_HALF	(1 << EMC_SDMR_DS_BIT)	/*1/2 Strength*/
  #define EMC_SDMR_DS_QUTR	(2 << EMC_SDMR_DS_BIT)	/*1/4 Strength*/

#define EMC_SDMR_PRSR_BIT	0	/* Partial Array Self Refresh */
#define EMC_SDMR_PRSR_MASK	(7 << EMC_SDMR_PRSR_BIT)
  #define EMC_SDMR_PRSR_ALL	(0 << EMC_SDMR_PRSR_BIT) /*All Banks*/
  #define EMC_SDMR_PRSR_HALF_TL	(1 << EMC_SDMR_PRSR_BIT) /*Half of Total Bank*/
  #define EMC_SDMR_PRSR_QUTR_TL	(2 << EMC_SDMR_PRSR_BIT) /*Quarter of Total Bank*/
  #define EMC_SDMR_PRSR_HALF_B0	(5 << EMC_SDMR_PRSR_BIT) /*Half of Bank0*/
  #define EMC_SDMR_PRSR_QUTR_B0	(6 << EMC_SDMR_PRSR_BIT) /*Quarter of Bank0*/

#define EMC_DMAR_BASE_BIT	8
#define EMC_DMAR_MASK_BIT	0

#define EMC_DMAR_BASE_MASK	(0xff << EMC_DMAR_BASE_BIT)
#define EMC_DMAR_MASK_MASK	(0xff << EMC_DMAR_MASK_BIT)

#define EMC_DMAR0_BASE		(0x20 << EMC_DMAR_BASE_BIT)
#define EMC_DMAR1_BASE_64M	(0x24 << EMC_DMAR_BASE_BIT) /*when bank0 is 64M*/
#define EMC_DMAR1_BASE_128M	(0x28 << EMC_DMAR_BASE_BIT) /*when bank0 is 128M*/

#define EMC_DMAR_MASK_64_64	(0xfc << EMC_DMAR_MASK_BIT)  /*mask for two 64M SDRAM*/
#define EMC_DMAR_MASK_128_128	(0xf8 << EMC_DMAR_MASK_BIT)  /*mask for two 128M SDRAM*/

#endif /* __JZ4760_EMC_H__ */
