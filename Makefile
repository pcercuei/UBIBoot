CONFIG=a320

CROSS_COMPILE ?= mipsel-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
NM = $(CROSS_COMPILE)nm

CFLAGS	:= -Wall -mips32 -Os -fno-pic -mno-abicalls
LDFLAGS	:= -nostdlib -EL -T target.ld

OUTDIR := output/$(CONFIG)

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

.PHONY: all clean map

all: $(OUTDIR)/ubiboot-ili9325.bin $(OUTDIR)/ubiboot-ili9331.bin $(OUTDIR)/ubiboot-ili9338.bin

$(OUTDIR)/ubiboot-ili9325.elf: $(addprefix $(OUTDIR)/,$(OBJS)) $(OUTDIR)/main_ili9325.o
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $^ -o $@

$(OUTDIR)/ubiboot-ili9331.elf: $(addprefix $(OUTDIR)/,$(OBJS)) $(OUTDIR)/main_ili9331.o
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $^ -o $@

$(OUTDIR)/ubiboot-ili9338.elf: $(addprefix $(OUTDIR)/,$(OBJS)) $(OUTDIR)/main_ili9338.o
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $^ -o $@

$(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	@mkdir -p $(@D)
	$(OBJCOPY) -O binary $< $@

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.o: src/%.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/main_ili9325.o: src/main.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -DJZ_SLCD_PANEL="\"ili9325\"" -c $< -o $@

$(OUTDIR)/main_ili9331.o: src/main.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -DJZ_SLCD_PANEL="\"ili9331\"" -c $< -o $@

$(OUTDIR)/main_ili9338.o: src/main.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -DJZ_SLCD_PANEL="\"ili9338\"" -c $< -o $@

map: $(OUTDIR)/System-ili9325.map $(OUTDIR)/System-ili9331.map $(OUTDIR)/System-ili9338.map

$(OUTDIR)/System-%.map: $(OUTDIR)/ubiboot-%.elf
	$(OBJDUMP) -D $< > $(basename $@).disasm
	$(OBJDUMP) -h $< > $(basename $@).headers
	$(NM) -n $< > $@

clean:
	rm -rf $(OUTDIR)
