# Ritmix RZX-50 handheld
# - Ingenic JZ4755
# - 64Mb SDRAM
# - 4Gb SD-card as main storage
# - 480х272 LCD
# - have 2 variants, with or without analog joystick
GC_FUNCTIONS = True
USE_SERIAL = True
#BKLIGHT_ON = True
#USE_NAND = True
#USE_UBI = True

BOARD := rzx50

VARIANTS := mmc
JZ_VERSION = 4755

CFLAGS_all := -mips32 -DTRY_ORIGINAL_FIRMWARE
CFLAGS_mmc := -DHAVE_MBR
