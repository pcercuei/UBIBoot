
CROSS_COMPILE ?= mipsel-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
NM = $(CROSS_COMPILE)nm

CFLAGS	:= -Wall -mips32 -O2 -fno-pic -mno-abicalls
LDFLAGS	:= -nostdlib -EL -T target.ld

OBJS	= head.o board.o main.o nand.o ubi.o
TARGET = ubiboot

GC_FUNCTIONS = True
ifdef GC_FUNCTIONS
	CFLAGS += -ffunction-sections
	LDFLAGS += --gc-sections
endif

USE_SERIAL = True
ifdef USE_SERIAL
	CFLAGS += -DUSE_SERIAL
	OBJS += serial.o
endif

all: $(TARGET).bin

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

map: $(TARGET).elf
	$(OBJDUMP) -D $< > $(basename $@).dump
	$(OBJDUMP) -h $< > $(basename $@).map
	$(NM) -n $< > System.map

clean:
	rm -fr *.o *.elf *.bin *.dump *.map

