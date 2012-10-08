ifdef V
	CMD:=
	SUM:=@\#
else
	CMD:=@
	SUM:=@echo
endif

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

VARIANTS := ili9325 ili9331 ili9338

CFLAGS_ili9325 := -DJZ_SLCD_PANEL="\"ili9325\""
CFLAGS_ili9331 := -DJZ_SLCD_PANEL="\"ili9331\""
CFLAGS_ili9338 := -DJZ_SLCD_PANEL="\"ili9338\""

.PHONY: all clean map

BINFILES := $(foreach VARIANT,$(VARIANTS),$(OUTDIR)/ubiboot-$(VARIANT).bin)
ELFFILES := $(foreach VARIANT,$(VARIANTS),$(OUTDIR)/ubiboot-$(VARIANT).elf)
OBJFILES := $(addprefix $(OUTDIR)/,$(OBJS))

all: $(BINFILES)

$(ELFFILES): $(OUTDIR)/ubiboot-%.elf: $(OUTDIR)/main_%.o $(OBJFILES)
	@mkdir -p $(@D)
	$(SUM) "  LD      $@"
	$(CMD)$(LD) $(LDFLAGS) $^ -o $@

$(BINFILES): $(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	@mkdir -p $(@D)
	$(SUM) "  BIN     $@"
	$(CMD)$(OBJCOPY) -O binary $< $@

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.o: src/%.S
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/main_%.o: src/main.c
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) $(CFLAGS) $(CFLAGS_$(@:$(OUTDIR)/main_%.o=%)) -c $< -o $@

MAPFILES := $(foreach VARIANT,$(VARIANTS),$(OUTDIR)/System-$(VARIANT).map)
DISFILES := $(foreach VARIANT,$(VARIANTS),$(OUTDIR)/System-$(VARIANT).disasm)
SECFILES := $(foreach VARIANT,$(VARIANTS),$(OUTDIR)/System-$(VARIANT).sections)

map: $(MAPFILES) $(DISFILES) $(SECFILES)

$(MAPFILES): $(OUTDIR)/System-%.map: $(OUTDIR)/ubiboot-%.elf
	@mkdir -p $(@D)
	$(NM) -n $< > $@

$(DISFILES): $(OUTDIR)/System-%.disasm: $(OUTDIR)/ubiboot-%.elf
	@mkdir -p $(@D)
	$(OBJDUMP) -D $< > $@

$(SECFILES): $(OUTDIR)/System-%.sections: $(OUTDIR)/ubiboot-%.elf
	$(OBJDUMP) -h $< > $@

clean:
	$(SUM) "  RM      $(OUTDIR)"
	$(CMD)rm -rf $(OUTDIR)
