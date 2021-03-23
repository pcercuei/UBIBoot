#ifndef __JZ47XX_H__
#define __JZ47XX_H__

typedef __signed char s8;
typedef unsigned char u8;

typedef __signed short s16;
typedef unsigned short u16;

typedef __signed int s32;
typedef unsigned int u32;

typedef unsigned long ulong;

#ifndef __ASSEMBLER__

#include "asm/cacheops.h"
#include "asm/addrspace.h"

/* cache parameters */
#define CFG_ICACHE_SIZE		0x4000
#define CFG_DCACHE_SIZE		0x4000
#define CFG_CACHELINE_SIZE	0x20

#define cache_unroll(base,op)	        	\
	__asm__ __volatile__("	         	\
		.set noreorder;		        \
		.set mips3;		        \
		cache %1, (%0);	                \
		.set mips0;			\
		.set reorder"			\
		:				\
		: "r" (base),			\
		  "i" (op));

static inline void jz_flush_dcache(void)
{
	unsigned long start;
	unsigned long end;

	start = KSEG0;
	end = start + CFG_DCACHE_SIZE;
	while (start < end) {
		cache_unroll(start,Index_Writeback_Inv_D);
		start += CFG_CACHELINE_SIZE;
	}
}

static inline void jz_flush_icache(void)
{
	unsigned long start;
	unsigned long end;

	start = KSEG0;
	end = start + CFG_ICACHE_SIZE;
	while(start < end) {
		cache_unroll(start,Index_Invalidate_I);
		start += CFG_CACHELINE_SIZE;
	}
}

/* cpu pipeline flush */
static inline void jz_sync(void)
{
	__asm__ volatile ("sync");
}

static inline void jz_writeb(u32 address, u8 value)
{
	*((volatile u8 *)address) = value;
}

static inline void jz_writew(u32 address, u16 value)
{
	*((volatile u16 *)address) = value;
}

static inline void jz_writel(u32 address, u32 value)
{
	*((volatile u32 *)address) = value;
}

static inline u8 jz_readb(u32 address)
{
	return *((volatile u8 *)address);
}

static inline u16 jz_readw(u32 address)
{
	return *((volatile u16 *)address);
}

static inline u32 jz_readl(u32 address)
{
	return *((volatile u32 *)address);
}

#define REG8(addr)	*((volatile u8 *)(addr))
#define REG16(addr)	*((volatile u16 *)(addr))
#define REG32(addr)	*((volatile u32 *)(addr))

#else

#define REG8(addr)	(addr)
#define REG16(addr)	(addr)
#define REG32(addr)	(addr)

#endif /* !ASSEMBLER */

#define BIT(x) (1 << (x))

#endif /*__JZ47XX_H__ */
