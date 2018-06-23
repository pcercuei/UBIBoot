#ifndef __JZ4770_GPIO_H__
#define __JZ4770_GPIO_H__

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
#define GPIO_PXINT(n)	(GPIO_BASE + (0x10 + (n)*0x100)) /* Port Interrupt Register */
#define GPIO_PXINTS(n)	(GPIO_BASE + (0x14 + (n)*0x100)) /* Port Interrupt Set Register */
#define GPIO_PXINTC(n)	(GPIO_BASE + (0x18 + (n)*0x100)) /* Port Interrupt Clear Register */
#define GPIO_PXMASK(n)	(GPIO_BASE + (0x20 + (n)*0x100)) /* Port Interrupt Mask Register */
#define GPIO_PXMASKS(n)	(GPIO_BASE + (0x24 + (n)*0x100)) /* Port Interrupt Mask Set Reg */
#define GPIO_PXMASKC(n)	(GPIO_BASE + (0x28 + (n)*0x100)) /* Port Interrupt Mask Clear Reg */
#define GPIO_PXPAT1(n)	(GPIO_BASE + (0x30 + (n)*0x100)) /* Port Pattern 1 Register */
#define GPIO_PXPAT1S(n)	(GPIO_BASE + (0x34 + (n)*0x100)) /* Port Pattern 1 Set Reg. */
#define GPIO_PXPAT1C(n)	(GPIO_BASE + (0x38 + (n)*0x100)) /* Port Pattern 1 Clear Reg. */
#define GPIO_PXPAT0(n)	(GPIO_BASE + (0x40 + (n)*0x100)) /* Port Pattern 0 Register */
#define GPIO_PXPAT0S(n)	(GPIO_BASE + (0x44 + (n)*0x100)) /* Port Pattern 0 Set Register */
#define GPIO_PXPAT0C(n)	(GPIO_BASE + (0x48 + (n)*0x100)) /* Port Pattern 0 Clear Register */
#define GPIO_PXFLG(n)	(GPIO_BASE + (0x50 + (n)*0x100)) /* Port Flag Register */
#define GPIO_PXFLGC(n)	(GPIO_BASE + (0x58 + (n)*0x100)) /* Port Flag clear Register */
#define GPIO_PXPEN(n)	(GPIO_BASE + (0x70 + (n)*0x100)) /* Port Pull Disable Register */
#define GPIO_PXPENS(n)	(GPIO_BASE + (0x74 + (n)*0x100)) /* Port Pull Disable Set Register */
#define GPIO_PXPENC(n)	(GPIO_BASE + (0x78 + (n)*0x100)) /* Port Pull Disable Clear Register */
#define GPIO_PXDS(n)	(GPIO_BASE + (0x80 + (n)*0x100)) /* Port Drive Strength Register */
#define GPIO_PXDSS(n)	(GPIO_BASE + (0x84 + (n)*0x100)) /* Port Drive Strength set Register */
#define GPIO_PXDSC(n)	(GPIO_BASE + (0x88 + (n)*0x100)) /* Port Drive Strength clear Register */

#define __gpio_as_output_mask(p, m)		\
do {						\
    REG32(GPIO_PXINTC(p)) = m;			\
    REG32(GPIO_PXMASKS(p)) = m;			\
    REG32(GPIO_PXPAT1C(p)) = m;			\
} while (0)

#define __gpio_as_input_mask(p, m)		\
do {						\
    REG32(GPIO_PXINTC(p)) = m;			\
    REG32(GPIO_PXMASKS(p)) = m;			\
    REG32(GPIO_PXPAT1S(p)) = m;			\
    REG32(GPIO_PXPAT0C(p)) = m;			\
} while (0)

#define __gpio_as_func_mask(p, m, f)		\
do {						\
    REG32(GPIO_PXINTC(p)) = m;			\
    REG32(GPIO_PXMASKC(p)) = m;			\
    if (f & 0x2)				\
	REG32(GPIO_PXPAT1S(p)) = m;		\
    else					\
	REG32(GPIO_PXPAT1C(p)) = m;		\
    if (f & 0x1)				\
	REG32(GPIO_PXPAT0S(p)) = m;		\
    else					\
	REG32(GPIO_PXPAT0C(p)) = m;		\
} while (0)

#define __gpio_set_pin_mask(p, m)		\
do {						\
	REG32(GPIO_PXPAT0S(p)) = m;		\
} while (0)

#define __gpio_clear_pin_mask(p, m)		\
do {						\
	REG32(GPIO_PXPAT0C(p)) = m;		\
} while (0)

#define __gpio_get_pin_mask(p, m)		\
	!!(REG32(GPIO_PXPIN(p)) & (m))

#define __gpio_enable_pull_mask(p, m)		\
do {						\
	REG32(GPIO_PXPENC(p)) = m;		\
} while (0)

#define __gpio_disable_pull_mask(p, m)		\
do {						\
	REG32(GPIO_PXPENS(p)) = m;		\
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

#endif /* __JZ4770_GPIO_H__ */
