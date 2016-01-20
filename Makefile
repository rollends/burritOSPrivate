OUTDIR	= bin
BLDDIR	= build
ASMDIR 	= asm
SRCDIR 	= source
INCDIR 	= include

CSOURCE = kernel.c uart.c
SSOURCE	= contextSwitch.s start.s
HSOURCE = ts7200.h uart.h

.DEFAULT_GOAL = all

ifndef TARGET
    TARGET=ts7200
endif

include make/preprocessor.make
include make/$(TARGET).make

_HSOURCE = $(addprefix $(INCDIR)/, $(HSOURCE))

all: directories $(OUTDIR)/$(OUT)

$(BLDDIR)/%.s: $(SRCDIR)/%.c $(_HSOURCE)
	$(XCC) -S -I$(INCDIR) $(CFLAGS) $(PRE_FLAGS) $< -o $@

$(BLDDIR)/%.o: $(BLDDIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $^

$(BLDDIR)/%.o: $(ASMDIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $^

$(OUTDIR)/%.elf: $(addprefix build/, $(CSOURCE:.c=.o)) $(addprefix build/, $(SSOURCE:.s=.o))
	$(LD) $(LDFLAGS) -o $@ $^

$(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	$(CP) $< -O binary $@

$(OUTDIR)/%.lst: $(OUTDIR)/%.elf
	$(DM) -D $^ > $@

install: directories $(TARGET)_install

directories:
	mkdir -p $(BLDDIR)
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(BLDDIR)
	rm -rf $(OUTDIR)

.SUFFIXES:
.SECONDARY:
.PHONY: clean directories
