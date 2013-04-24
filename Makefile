ifeq ($(CONFIG),)
CONFIGS:=$(foreach CFG,$(wildcard config-*.mk),$(CFG:config-%.mk=%))
$(error Please specify CONFIG, possible values: $(CONFIGS))
endif

include config-$(CONFIG).mk

ifdef V
	CMD:=
	SUM:=@\#
else
	CMD:=@
	SUM:=@echo
endif

CROSS_COMPILE ?= mipsel-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
NM = $(CROSS_COMPILE)nm

CFLAGS	:= -Wall -mips32 -Os -fno-pic -mno-abicalls
LDFLAGS	:= -nostdlib -EL -T target-$(BOARD).ld

OUTDIR	:= output/$(CONFIG)

OBJS	:= utils.o mmc.o fat.o
OBJS	+= head-$(BOARD).o

ifdef GC_FUNCTIONS
	CFLAGS += -ffunction-sections
	LDFLAGS += --gc-sections
endif

ifdef USE_SERIAL
	CFLAGS += -DUSE_SERIAL
	OBJS += serial.o
endif

ifdef BKLIGHT_ON
	CFLAGS += -DBKLIGHT_ON
endif

ifdef USE_NAND
	CFLAGS += -DUSE_NAND
	OBJS += nand.o
endif

ifdef USE_UBI
	CFLAGS += -DUSE_UBI
	OBJS += ubi.o
endif

.PHONY: all clean map

BINFILES := $(foreach VARIANT,$(VARIANTS),$(OUTDIR)/ubiboot-$(VARIANT).bin)
ELFFILES := $(foreach VARIANT,$(VARIANTS),$(OUTDIR)/ubiboot-$(VARIANT).elf)
OBJFILES := $(addprefix $(OUTDIR)/,$(OBJS))

all: $(BINFILES)

$(ELFFILES): $(OUTDIR)/ubiboot-%.elf: \
		$(OUTDIR)/main_%.o $(OUTDIR)/board-$(BOARD)_%.o $(OBJFILES)
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

CFLAGS_FOR_VARIANT=$(CFLAGS) -DVARIANT="\"$(1)\"" $(CFLAGS_$(1))

$(OUTDIR)/board-$(BOARD)_%.o: src/board-$(BOARD).c
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) \
		$(call CFLAGS_FOR_VARIANT,$(@:$(OUTDIR)/board-$(BOARD)_%.o=%)) \
		-c $< -o $@

$(OUTDIR)/main_%.o: src/main.c
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) \
		$(call CFLAGS_FOR_VARIANT,$(@:$(OUTDIR)/main_%.o=%)) \
		-c $< -o $@

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
