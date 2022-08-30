# Ritmix RZX-27 handheld
# - Ingenic JZ4725b
# - 32Mb SDRAM
# - 4Gb SD-card as main storage (might have NAND instead)
# - 320x240 smart LCD (UC8230 based, might have another screens)
GC_FUNCTIONS = True
USE_SERIAL = True
#BKLIGHT_ON = True
#USE_NAND = False
#USE_UBI = False

BOARD := rzx27

VARIANTS := mmc
JZ_VERSION = 4725

CFLAGS_all := -mips32
CFLAGS_mmc := -DHAVE_MBR
