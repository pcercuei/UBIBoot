#ifndef __JZ4740_GPIO_H__
#define __JZ4740_GPIO_H__

#include "jz.h"

#define	GPIO_BASE	0xB0010000

enum gpio_port {
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
};

#define GPIO_PXPIN(n)	(GPIO_BASE + (0x00 + (n)*0x100)) /* PIN Level Register */
#define GPIO_PXDAT(n)	(GPIO_BASE + (0x10 + (n)*0x100)) /* Port Data Register */
#define GPIO_PXDATS(n)	(GPIO_BASE + (0x14 + (n)*0x100)) /* Port Data Set Register */
#define GPIO_PXDATC(n)	(GPIO_BASE + (0x18 + (n)*0x100)) /* Port Data Clear Register */
#define GPIO_PXIM(n)	(GPIO_BASE + (0x20 + (n)*0x100)) /* Interrupt Mask Register */
#define GPIO_PXIMS(n)	(GPIO_BASE + (0x24 + (n)*0x100)) /* Interrupt Mask Set Reg */
#define GPIO_PXIMC(n)	(GPIO_BASE + (0x28 + (n)*0x100)) /* Interrupt Mask Clear Reg */
#define GPIO_PXPE(n)	(GPIO_BASE + (0x30 + (n)*0x100)) /* Pull Enable Register */
#define GPIO_PXPES(n)	(GPIO_BASE + (0x34 + (n)*0x100)) /* Pull Enable Set Reg. */
#define GPIO_PXPEC(n)	(GPIO_BASE + (0x38 + (n)*0x100)) /* Pull Enable Clear Reg. */
#define GPIO_PXFUN(n)	(GPIO_BASE + (0x40 + (n)*0x100)) /* Function Register */
#define GPIO_PXFUNS(n)	(GPIO_BASE + (0x44 + (n)*0x100)) /* Function Set Register */
#define GPIO_PXFUNC(n)	(GPIO_BASE + (0x48 + (n)*0x100)) /* Function Clear Register */
#define GPIO_PXSEL(n)	(GPIO_BASE + (0x50 + (n)*0x100)) /* Select Register */
#define GPIO_PXSELS(n)	(GPIO_BASE + (0x54 + (n)*0x100)) /* Select Set Register */
#define GPIO_PXSELC(n)	(GPIO_BASE + (0x58 + (n)*0x100)) /* Select Clear Register */
#define GPIO_PXDIR(n)	(GPIO_BASE + (0x60 + (n)*0x100)) /* Direction Register */
#define GPIO_PXDIRS(n)	(GPIO_BASE + (0x64 + (n)*0x100)) /* Direction Set Register */
#define GPIO_PXDIRC(n)	(GPIO_BASE + (0x68 + (n)*0x100)) /* Direction Clear Register */
#define GPIO_PXTRG(n)	(GPIO_BASE + (0x70 + (n)*0x100)) /* Trigger Register */
#define GPIO_PXTRGS(n)	(GPIO_BASE + (0x74 + (n)*0x100)) /* Trigger Set Register */
#define GPIO_PXTRGC(n)	(GPIO_BASE + (0x78 + (n)*0x100)) /* Trigger Set Register */
#define GPIO_PXFLG(n)	(GPIO_BASE + (0x80 + (n)*0x100)) /* Port Flag Register */
#define GPIO_PXFLGC(n)	(GPIO_BASE + (0x14 + (n)*0x100)) /* Port Flag clear Register */

#define __gpio_as_output_mask(p, m)		\
do {						\
	REG32(GPIO_PXFUNC(p)) = m;		\
	REG32(GPIO_PXSELC(p)) = m;		\
	REG32(GPIO_PXDIRS(p)) = m;		\
} while (0)

#define __gpio_as_input_mask(p, m)		\
do {						\
	REG32(GPIO_PXFUNC(p)) = m;		\
	REG32(GPIO_PXSELC(p)) = m;		\
	REG32(GPIO_PXDIRC(p)) = m;		\
} while (0)

#define __gpio_as_func_mask(p, m, f)		\
do {						\
	REG32(GPIO_PXFUNS(p)) = m;		\
	if (f & 2)				\
		REG32(GPIO_PXTRGS(p)) = m;	\
	else					\
		REG32(GPIO_PXTRGC(p)) = m;	\
	if (f & 1)				\
		REG32(GPIO_PXSELS(p)) = m;	\
	else					\
		REG32(GPIO_PXSELC(p)) = m;	\
} while (0)

#define __gpio_set_pin_mask(p, m)		\
do {						\
	REG32(GPIO_PXDATS(p)) = m;		\
} while (0)

#define __gpio_clear_pin_mask(p, m)		\
do {						\
	REG32(GPIO_PXDATC(p)) = m;		\
} while (0)

#define __gpio_get_pin_mask(p, m)		\
	!!(REG32(GPIO_PXPIN(p)) & (m))

#define __gpio_enable_pull_mask(p, m)		\
do {						\
	REG32(GPIO_PXPEC(p)) = m;		\
} while (0)

#define __gpio_disable_pull_mask(p, m)		\
do {						\
	REG32(GPIO_PXPES(p)) = m;		\
} while (0)

#define __gpio_as_output(p, o)			\
	__gpio_as_output_mask(p, BIT(o))

#define __gpio_as_input(p, o)			\
	__gpio_as_input_mask(p, BIT(o))

#define __gpio_as_func(p, o, f)			\
	__gpio_as_func_mask(p, BIT(o), f)

#define __gpio_set_pin(p, o)			\
	__gpio_set_pin_mask(p, BIT(o))

#define __gpio_clear_pin(p, o)			\
	__gpio_clear_pin_mask(p, BIT(o))

#define __gpio_get_pin(p, o)			\
	__gpio_get_pin_mask(p, BIT(o))

#define __gpio_enable_pull(p, o)		\
	__gpio_enable_pull_mask(p, BIT(o))

#define __gpio_disable_pull(p, o)		\
	__gpio_disable_pull_mask(p, BIT(o))

#endif /* __JZ4740_GPIO_H__ */
