
CROSS_COMPILE ?= mipsel-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
NM = $(CROSS_COMPILE)nm

CFLAGS	:= -Wall -mips32 -Os -fno-pic -mno-abicalls
LDFLAGS	:= -nostdlib -EL -T target.ld

OBJS	= head.o board.o nand.o ubi.o utils.o mmc.o fat.o

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

# BKLIGHT_ON = True
ifdef BKLIGHT_ON
	CFLAGS += -DBKLIGHT_ON
endif

TARGET = ubiboot-$(JZ_SLCD_PANEL)

all: ubiboot-ili9325.bin ubiboot-ili9331.bin ubiboot-ili9338.bin

ubiboot-ili9325.elf: $(OBJS) main_ili9325.o
	$(LD) $(LDFLAGS) $^ -o $@

ubiboot-ili9331.elf: $(OBJS) main_ili9331.o
	$(LD) $(LDFLAGS) $^ -o $@

ubiboot-ili9338.elf: $(OBJS) main_ili9338.o
	$(LD) $(LDFLAGS) $^ -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: src/%.S
	$(CC) $(CFLAGS) -c $< -o $@

main_ili9325.o: src/main.c
	$(CC) $(CFLAGS) -DJZ_SLCD_PANEL="\"ili9325\"" -c $< -o $@

main_ili9331.o: src/main.c
	$(CC) $(CFLAGS) -DJZ_SLCD_PANEL="\"ili9331\"" -c $< -o $@

main_ili9338.o: src/main.c
	$(CC) $(CFLAGS) -DJZ_SLCD_PANEL="\"ili9338\"" -c $< -o $@

map: $(TARGET).elf
	$(OBJDUMP) -D $< > $(basename $@).dump
	$(OBJDUMP) -h $< > $(basename $@).map
	$(NM) -n $< > System.map

clean:
	rm -f $(OBJS) ubiboot-ili*.elf ubiboot-ili*.bin main_ili*.o map.dump map.map System.map

