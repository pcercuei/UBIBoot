#ifndef __JZ47XX_H__
#define __JZ47XX_H__

#if JZ_VERSION == 4740
#	include "jz4740.h"
#elif JZ_VERSION == 4770
#	include "jz4770.h"
#endif

#define	MSC_BASE(x)	(0xB0021000 + (x) * 0x1000)

#define	MSC_STRPCL(x)	(MSC_BASE(x) + 0x000)
#define	MSC_STAT(x)		(MSC_BASE(x) + 0x004)
#define	MSC_CLKRT(x)	(MSC_BASE(x) + 0x008)
#define	MSC_CMDAT(x)	(MSC_BASE(x) + 0x00C)
#define	MSC_RESTO(x)	(MSC_BASE(x) + 0x010)
#define	MSC_RDTO(x)		(MSC_BASE(x) + 0x014)
#define	MSC_BLKLEN(x)	(MSC_BASE(x) + 0x018)
#define	MSC_NOB(x)		(MSC_BASE(x) + 0x01C)
#define	MSC_SNOB(x)		(MSC_BASE(x) + 0x020)
#define	MSC_IMASK(x)	(MSC_BASE(x) + 0x024)
#define	MSC_IREG(x)		(MSC_BASE(x) + 0x028)
#define	MSC_CMD(x)		(MSC_BASE(x) + 0x02C)
#define	MSC_ARG(x)		(MSC_BASE(x) + 0x030)
#define	MSC_RES(x)		(MSC_BASE(x) + 0x034)
#define	MSC_RXFIFO(x)	(MSC_BASE(x) + 0x038)
#define	MSC_TXFIFO(x)	(MSC_BASE(x) + 0x03C)

#define	REG_MSC_STRPCL(x)	REG16(MSC_STRPCL(x))
#define	REG_MSC_STAT(x)		REG32(MSC_STAT(x))
#define	REG_MSC_CLKRT(x)	REG16(MSC_CLKRT(x))
#define	REG_MSC_CMDAT(x)	REG32(MSC_CMDAT(x))
#define	REG_MSC_RESTO(x)	REG16(MSC_RESTO(x))
#define	REG_MSC_RDTO(x)		REG16(MSC_RDTO(x))
#define	REG_MSC_BLKLEN(x)	REG16(MSC_BLKLEN(x))
#define	REG_MSC_NOB(x)		REG16(MSC_NOB(x))
#define	REG_MSC_SNOB(x)		REG16(MSC_SNOB(x))
#define	REG_MSC_IMASK(x)	REG16(MSC_IMASK(x))
#define	REG_MSC_IREG(x)		REG16(MSC_IREG(x))
#define	REG_MSC_CMD(x)		REG8(MSC_CMD(x))
#define	REG_MSC_ARG(x)		REG32(MSC_ARG(x))
#define	REG_MSC_RES(x)		REG16(MSC_RES(x))
#define	REG_MSC_RXFIFO(x)	REG32(MSC_RXFIFO(x))
#define	REG_MSC_TXFIFO(x)	REG32(MSC_TXFIFO(x))

#if JZ_VERSION >= 4770
#define	MSC_LPM(x)		(MSC_BASE(x) + 0x040)
#define REG_MSC_LPM(x)		REG32(MSC_LPM(x))
#endif

/* MSC Clock and Control Register (MSC_STRPCL) */

#define MSC_STRPCL_EXIT_MULTIPLE	(1 << 7)
#define MSC_STRPCL_EXIT_TRANSFER	(1 << 6)
#define MSC_STRPCL_START_READWAIT	(1 << 5)
#define MSC_STRPCL_STOP_READWAIT	(1 << 4)
#define MSC_STRPCL_RESET		(1 << 3)
#define MSC_STRPCL_START_OP		(1 << 2)
#define MSC_STRPCL_CLOCK_CONTROL_BIT	0
#define MSC_STRPCL_CLOCK_CONTROL_MASK	(0x3 << MSC_STRPCL_CLOCK_CONTROL_BIT)
  #define MSC_STRPCL_CLOCK_CONTROL_STOP	  (0x1 << MSC_STRPCL_CLOCK_CONTROL_BIT) /* Stop MMC/SD clock */
  #define MSC_STRPCL_CLOCK_CONTROL_START  (0x2 << MSC_STRPCL_CLOCK_CONTROL_BIT) /* Start MMC/SD clock */

/* MSC Status Register (MSC_STAT) */

#if JZ_VERSION >= 4770
#define MSC_STAT_AUTO_CMD_DONE		(1 << 31)
#endif
#define MSC_STAT_IS_RESETTING		(1 << 15)
#define MSC_STAT_SDIO_INT_ACTIVE	(1 << 14)
#define MSC_STAT_PRG_DONE		(1 << 13)
#define MSC_STAT_DATA_TRAN_DONE		(1 << 12)
#define MSC_STAT_END_CMD_RES		(1 << 11)
#define MSC_STAT_DATA_FIFO_AFULL	(1 << 10)
#define MSC_STAT_IS_READWAIT		(1 << 9)
#define MSC_STAT_CLK_EN			(1 << 8)
#define MSC_STAT_DATA_FIFO_FULL		(1 << 7)
#define MSC_STAT_DATA_FIFO_EMPTY	(1 << 6)
#define MSC_STAT_CRC_RES_ERR		(1 << 5)
#define MSC_STAT_CRC_READ_ERROR		(1 << 4)
#define MSC_STAT_CRC_WRITE_ERROR_BIT	2
#define MSC_STAT_CRC_WRITE_ERROR_MASK	(0x3 << MSC_STAT_CRC_WRITE_ERROR_BIT)
  #define MSC_STAT_CRC_WRITE_ERROR_NO		(0 << MSC_STAT_CRC_WRITE_ERROR_BIT) /* No error on transmission of data */
  #define MSC_STAT_CRC_WRITE_ERROR		(1 << MSC_STAT_CRC_WRITE_ERROR_BIT) /* Card observed erroneous transmission of data */
  #define MSC_STAT_CRC_WRITE_ERROR_NOSTS	(2 << MSC_STAT_CRC_WRITE_ERROR_BIT) /* No CRC status is sent back */
#define MSC_STAT_TIME_OUT_RES		(1 << 1)
#define MSC_STAT_TIME_OUT_READ		(1 << 0)

/* MSC Bus Clock Control Register (MSC_CLKRT) */

#define	MSC_CLKRT_CLK_RATE_BIT		0
#define	MSC_CLKRT_CLK_RATE_MASK		(0x7 << MSC_CLKRT_CLK_RATE_BIT)
  #define MSC_CLKRT_CLK_RATE_DIV_1	  (0x0 << MSC_CLKRT_CLK_RATE_BIT) /* CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_2	  (0x1 << MSC_CLKRT_CLK_RATE_BIT) /* 1/2 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_4	  (0x2 << MSC_CLKRT_CLK_RATE_BIT) /* 1/4 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_8	  (0x3 << MSC_CLKRT_CLK_RATE_BIT) /* 1/8 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_16	  (0x4 << MSC_CLKRT_CLK_RATE_BIT) /* 1/16 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_32	  (0x5 << MSC_CLKRT_CLK_RATE_BIT) /* 1/32 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_64	  (0x6 << MSC_CLKRT_CLK_RATE_BIT) /* 1/64 of CLK_SRC */
  #define MSC_CLKRT_CLK_RATE_DIV_128	  (0x7 << MSC_CLKRT_CLK_RATE_BIT) /* 1/128 of CLK_SRC */

/* MSC Command Sequence Control Register (MSC_CMDAT) */

#define	MSC_CMDAT_IO_ABORT		(1 << 11)
#define	MSC_CMDAT_BUS_WIDTH_BIT		9
#define	MSC_CMDAT_BUS_WIDTH_MASK	(0x3 << MSC_CMDAT_BUS_WIDTH_BIT)
  #define MSC_CMDAT_BUS_WIDTH_1BIT	  (0x0 << MSC_CMDAT_BUS_WIDTH_BIT) /* 1-bit data bus */
  #define MSC_CMDAT_BUS_WIDTH_4BIT	  (0x2 << MSC_CMDAT_BUS_WIDTH_BIT) /* 4-bit data bus */
  #define CMDAT_BUS_WIDTH1	  (0x0 << MSC_CMDAT_BUS_WIDTH_BIT)
  #define CMDAT_BUS_WIDTH4	  (0x2 << MSC_CMDAT_BUS_WIDTH_BIT)
#define	MSC_CMDAT_DMA_EN		(1 << 8)
#define	MSC_CMDAT_INIT			(1 << 7)
#define	MSC_CMDAT_BUSY			(1 << 6)
#define	MSC_CMDAT_STREAM_BLOCK		(1 << 5)
#define	MSC_CMDAT_WRITE			(1 << 4)
#define	MSC_CMDAT_READ			(0 << 4)
#define	MSC_CMDAT_DATA_EN		(1 << 3)
#define	MSC_CMDAT_RESPONSE_BIT	0
#define	MSC_CMDAT_RESPONSE_MASK	(0x7 << MSC_CMDAT_RESPONSE_BIT)
  #define MSC_CMDAT_RESPONSE_NONE  (0x0 << MSC_CMDAT_RESPONSE_BIT) /* No response */
  #define MSC_CMDAT_RESPONSE_R1	  (0x1 << MSC_CMDAT_RESPONSE_BIT) /* Format R1 and R1b */
  #define MSC_CMDAT_RESPONSE_R2	  (0x2 << MSC_CMDAT_RESPONSE_BIT) /* Format R2 */
  #define MSC_CMDAT_RESPONSE_R3	  (0x3 << MSC_CMDAT_RESPONSE_BIT) /* Format R3 */
  #define MSC_CMDAT_RESPONSE_R4	  (0x4 << MSC_CMDAT_RESPONSE_BIT) /* Format R4 */
  #define MSC_CMDAT_RESPONSE_R5	  (0x5 << MSC_CMDAT_RESPONSE_BIT) /* Format R5 */
  #define MSC_CMDAT_RESPONSE_R6	  (0x6 << MSC_CMDAT_RESPONSE_BIT) /* Format R6 */

#define	CMDAT_DMA_EN	(1 << 8)
#define	CMDAT_INIT	(1 << 7)
#define	CMDAT_BUSY	(1 << 6)
#define	CMDAT_STREAM	(1 << 5)
#define	CMDAT_WRITE	(1 << 4)
#define	CMDAT_DATA_EN	(1 << 3)

/* MSC Interrupts Mask Register (MSC_IMASK) */

#define	MSC_IMASK_SDIO			(1 << 7)
#define	MSC_IMASK_TXFIFO_WR_REQ		(1 << 6)
#define	MSC_IMASK_RXFIFO_RD_REQ		(1 << 5)
#define	MSC_IMASK_END_CMD_RES		(1 << 2)
#define	MSC_IMASK_PRG_DONE		(1 << 1)
#define	MSC_IMASK_DATA_TRAN_DONE	(1 << 0)


/* MSC Interrupts Status Register (MSC_IREG) */

#define	MSC_IREG_SDIO			(1 << 7)
#define	MSC_IREG_TXFIFO_WR_REQ		(1 << 6)
#define	MSC_IREG_RXFIFO_RD_REQ		(1 << 5)
#define	MSC_IREG_END_CMD_RES		(1 << 2)
#define	MSC_IREG_PRG_DONE		(1 << 1)
#define	MSC_IREG_DATA_TRAN_DONE		(1 << 0)

#define __msc_start_op(x) \
do { \
  REG_MSC_STRPCL(x) = MSC_STRPCL_START_OP | MSC_STRPCL_CLOCK_CONTROL_START; \
} while (0)

#define __msc_set_resto(x, to) 	( REG_MSC_RESTO(x) = to )
#define __msc_set_rdto(x, to) 	( REG_MSC_RDTO(x) = to )
#define __msc_set_cmd(x, cmd) 	( REG_MSC_CMD(x) = cmd )
#define __msc_set_arg(x, arg) 	( REG_MSC_ARG(x) = arg )
#define __msc_set_nob(x, nob) 	( REG_MSC_NOB(x) = nob )
#define __msc_get_nob(x) 	( REG_MSC_NOB(x) )
#define __msc_set_blklen(x, len) 	( REG_MSC_BLKLEN(x) = len )
#define __msc_set_cmdat(x, cmdat) 	( REG_MSC_CMDAT(x) = cmdat )
#define __msc_set_cmdat_ioabort(x) 	( REG_MSC_CMDAT(x) |= MSC_CMDAT_IO_ABORT )
#define __msc_clear_cmdat_ioabort(x) 	( REG_MSC_CMDAT(x) &= ~MSC_CMDAT_IO_ABORT )

#define __msc_set_cmdat_bus_width1(x) 			\
do { 							\
	REG_MSC_CMDAT(x) &= ~MSC_CMDAT_BUS_WIDTH_MASK; 	\
	REG_MSC_CMDAT(x) |= MSC_CMDAT_BUS_WIDTH_1BIT; 	\
} while(0)

#define __msc_set_cmdat_bus_width4(x) 			\
do { 							\
	REG_MSC_CMDAT(x) &= ~MSC_CMDAT_BUS_WIDTH_MASK; 	\
	REG_MSC_CMDAT(x) |= MSC_CMDAT_BUS_WIDTH_4BIT; 	\
} while(0)

#define __msc_set_cmdat_dma_en(x) ( REG_MSC_CMDAT(x) |= MSC_CMDAT_DMA_EN )
#define __msc_set_cmdat_init(x) 	( REG_MSC_CMDAT(x) |= MSC_CMDAT_INIT )
#define __msc_set_cmdat_busy(x) 	( REG_MSC_CMDAT(x) |= MSC_CMDAT_BUSY )
#define __msc_set_cmdat_stream(x) ( REG_MSC_CMDAT(x) |= MSC_CMDAT_STREAM_BLOCK )
#define __msc_set_cmdat_block(x) ( REG_MSC_CMDAT(x) &= ~MSC_CMDAT_STREAM_BLOCK )
#define __msc_set_cmdat_read(x) 	( REG_MSC_CMDAT(x) &= ~MSC_CMDAT_WRITE_READ )
#define __msc_set_cmdat_write(x) ( REG_MSC_CMDAT(x) |= MSC_CMDAT_WRITE_READ )
#define __msc_set_cmdat_data_en(x) ( REG_MSC_CMDAT(x) |= MSC_CMDAT_DATA_EN )

/* r is MSC_CMDAT_RESPONSE_FORMAT_Rx or MSC_CMDAT_RESPONSE_FORMAT_NONE */
#define __msc_set_cmdat_res_format(x, r) 				\
do { 								\
	REG_MSC_CMDAT(x) &= ~MSC_CMDAT_RESPONSE_FORMAT_MASK; 	\
	REG_MSC_CMDAT(x) |= (r); 					\
} while(0)

#define __msc_clear_cmdat(x) \
  REG_MSC_CMDAT(x) &= ~( MSC_CMDAT_IO_ABORT | MSC_CMDAT_DMA_EN | MSC_CMDAT_INIT| \
  MSC_CMDAT_BUSY | MSC_CMDAT_STREAM_BLOCK | MSC_CMDAT_WRITE_READ | \
  MSC_CMDAT_DATA_EN | MSC_CMDAT_RESPONSE_FORMAT_MASK )

#define __msc_get_imask(x) 		( REG_MSC_IMASK(x) )
#define __msc_mask_all_intrs(x) 		( REG_MSC_IMASK(x) = 0xff )
#define __msc_unmask_all_intrs(x) 	( REG_MSC_IMASK(x) = 0x00 )
#define __msc_mask_rd(x) 		( REG_MSC_IMASK(x) |= MSC_IMASK_RXFIFO_RD_REQ )
#define __msc_unmask_rd(x) 		( REG_MSC_IMASK(x) &= ~MSC_IMASK_RXFIFO_RD_REQ )
#define __msc_mask_wr(x) 		( REG_MSC_IMASK(x) |= MSC_IMASK_TXFIFO_WR_REQ )
#define __msc_unmask_wr(x) 		( REG_MSC_IMASK(x) &= ~MSC_IMASK_TXFIFO_WR_REQ )
#define __msc_mask_endcmdres(x) 		( REG_MSC_IMASK(x) |= MSC_IMASK_END_CMD_RES )
#define __msc_unmask_endcmdres(x) 	( REG_MSC_IMASK(x) &= ~MSC_IMASK_END_CMD_RES )
#define __msc_mask_datatrandone(x) 	( REG_MSC_IMASK(x) |= MSC_IMASK_DATA_TRAN_DONE )
#define __msc_unmask_datatrandone(x) 	( REG_MSC_IMASK(x) &= ~MSC_IMASK_DATA_TRAN_DONE )
#define __msc_mask_prgdone(x) 		( REG_MSC_IMASK(x) |= MSC_IMASK_PRG_DONE )
#define __msc_unmask_prgdone(x) 		( REG_MSC_IMASK(x) &= ~MSC_IMASK_PRG_DONE )

/* n=0,1,2,3,4,5,6,7 */
#define __msc_set_clkrt(x, n) 	\
do { 				\
	REG_MSC_CLKRT(x) = n;	\
} while(0)

#define __msc_get_ireg(x) 		( REG_MSC_IREG(x) )
#define __msc_ireg_rd(x) 		( REG_MSC_IREG(x) & MSC_IREG_RXFIFO_RD_REQ )
#define __msc_ireg_wr(x) 		( REG_MSC_IREG(x) & MSC_IREG_TXFIFO_WR_REQ )
#define __msc_ireg_end_cmd_res(x) 	( REG_MSC_IREG(x) & MSC_IREG_END_CMD_RES )
#define __msc_ireg_data_tran_done(x) 	( REG_MSC_IREG(x) & MSC_IREG_DATA_TRAN_DONE )
#define __msc_ireg_prg_done(x) 		( REG_MSC_IREG(x) & MSC_IREG_PRG_DONE )
#define __msc_ireg_clear_end_cmd_res(x) 	( REG_MSC_IREG(x) = MSC_IREG_END_CMD_RES )
#define __msc_ireg_clear_data_tran_done(x) ( REG_MSC_IREG(x) = MSC_IREG_DATA_TRAN_DONE )
#define __msc_ireg_clear_prg_done(x) 	( REG_MSC_IREG(x) = MSC_IREG_PRG_DONE )

#define __msc_get_stat(x) 		( REG_MSC_STAT(x) )
#define __msc_stat_not_end_cmd_res(x) 	( (REG_MSC_STAT(x) & MSC_STAT_END_CMD_RES) == 0)
#define __msc_stat_crc_err(x) \
  ( REG_MSC_STAT(x) & (MSC_STAT_CRC_RES_ERR | MSC_STAT_CRC_READ_ERROR | MSC_STAT_CRC_WRITE_ERROR_YES) )
#define __msc_stat_res_crc_err(x) 	( REG_MSC_STAT(x) & MSC_STAT_CRC_RES_ERR )
#define __msc_stat_rd_crc_err(x) 	( REG_MSC_STAT(x) & MSC_STAT_CRC_READ_ERROR )
#define __msc_stat_wr_crc_err(x) 	( REG_MSC_STAT(x) & MSC_STAT_CRC_WRITE_ERROR_YES )
#define __msc_stat_resto_err(x)		( REG_MSC_STAT(x) & MSC_STAT_TIME_OUT_RES )
#define __msc_stat_rdto_err(x) 		( REG_MSC_STAT(x) & MSC_STAT_TIME_OUT_READ )

#define __msc_rd_resfifo(x) 		( REG_MSC_RES(x) )
#define __msc_rd_rxfifo(x)  		( REG_MSC_RXFIFO(x) )
#define __msc_wr_txfifo(x, v)  		( REG_MSC_TXFIFO(x) = v )

#define __msc_reset(x) 						\
do { 								\
	REG_MSC_STRPCL(x) = MSC_STRPCL_RESET;			\
	while (REG_MSC_STAT(x) & MSC_STAT_IS_RESETTING);		\
} while (0)

#define __msc_start_clk(x) 					\
do { 								\
	REG_MSC_STRPCL(x) = MSC_STRPCL_CLOCK_CONTROL_START;	\
} while (0)

#define __msc_stop_clk(x) 					\
do { 								\
	REG_MSC_STRPCL(x) = MSC_STRPCL_CLOCK_CONTROL_STOP;	\
} while (0)

#endif /*__JZ47XX_H__ */
