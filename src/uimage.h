/*
 * uImage: U-Boot image format
 *
 * http://www.denx.de/wiki/U-Boot/
 */

#ifndef UIMAGE_H
#define UIMAGE_H

#define UIMAGE_MAGIC	0x27051956	/* Image magic number */

#define UIMAGE_OS_LINUX			5	/* Linux */

#define UIMAGE_ARCH_MIPS		5	/* MIPS (32-bit) */

#define UIMAGE_TYPE_KERNEL		2	/* OS kernel image */

#define UIMAGE_COMP_NONE		0	/*  No compression */

struct uimage_header {
	/* Note: All fields are big endian. */
	uint32_t	magic;				/* Magic number (UIMAGE_MAGIC) */
	uint32_t	hcrc;				/* Image header CRC */
	uint32_t	time;				/* Image creation timestamp */
	uint32_t	size;				/* Image data size */
	uint32_t	load;				/* Data load address */
	uint32_t	ep;					/* Entry point address */
	uint32_t	dcrc;				/* Image data CRC */
	uint8_t		os;					/* Operating system */
	uint8_t		arch;				/* CPU architecture */
	uint8_t		type;				/* Image type */
	uint8_t		comp;				/* Compression type */
	uint8_t		name[32];			/* Image name */
};

#endif
