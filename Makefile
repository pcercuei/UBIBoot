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

CFLAGS	:= -Wall -Os -fno-pic -mno-abicalls -mno-check-zero-division -ffreestanding -flto
CFLAGS	+= $(CFLAGS_all)
CPPFLAGS := -DBOARD_$(BOARD) -DJZ_VERSION=$(JZ_VERSION)
LDFLAGS	:= -nostdlib -EL -T target-$(BOARD).ld

OUTDIR	:= output/$(CONFIG)

OBJS	:= utils.o mmc.o fat.o head.o

ifdef GC_FUNCTIONS
	CFLAGS += -ffunction-sections
	LDFLAGS += -Wl,--gc-sections
endif

ifdef USE_SERIAL
	CPPFLAGS += -DUSE_SERIAL
	OBJS += serial.o
endif

ifdef BKLIGHT_ON
	CPPFLAGS += -DBKLIGHT_ON
endif

ifdef USE_NAND
	CPPFLAGS += -DUSE_NAND
	OBJS += nand.o
endif

ifdef USE_UBI
	CPPFLAGS += -DUSE_UBI
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
	$(CMD)$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BINFILES): $(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	@mkdir -p $(@D)
	$(SUM) "  BIN     $@"
	$(CMD)$(OBJCOPY) -O binary $< $@

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OUTDIR)/%.o: src/%.S
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

CFLAGS_FOR_VARIANT=$(CFLAGS) -DVARIANT="\"$(1)\"" $(CFLAGS_$(1))

$(OUTDIR)/board-$(BOARD)_%.o: src/board-$(BOARD).c
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) $(CPPFLAGS) \
		$(call CFLAGS_FOR_VARIANT,$(@:$(OUTDIR)/board-$(BOARD)_%.o=%)) \
		-c $< -o $@

$(OUTDIR)/main_%.o: src/main.c
	@mkdir -p $(@D)
	$(SUM) "  CC      $@"
	$(CMD)$(CC) $(CPPFLAGS) \
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
