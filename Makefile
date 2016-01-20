OUTDIR	= bin
BLDDIR	= build
ASMDIR 	= asm
SRCDIR 	= source
INCDIR 	= include

CSOURCE = kernel.c print.c queue.c task.c uart.c userTasks.c
SSOURCE	= contextSwitch.s start.s
HSOURCE = memory.h print.h queue.h sysCall.h task.h ts7200.h types.h uart.h userTasks.h

.DEFAULT_GOAL = all

ifndef TARGET
    TARGET=ts7200
endif

include make/preprocessor.make
include make/$(TARGET).make

_HSOURCE = $(addprefix $(INCDIR)/, $(HSOURCE))

all: directories $(OUTDIR)/$(OUT) $(OUTDIR)/$(basename $(OUT)).lst

$(BLDDIR)/%.s: $(SRCDIR)/%.c $(_HSOURCE)
	$(XCC) -S -I$(INCDIR) $(CFLAGS) $(PRE_FLAGS) $< -o $@

$(BLDDIR)/%.o: $(BLDDIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $^

$(BLDDIR)/%.o: $(ASMDIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $^

$(OUTDIR)/%.elf: $(addprefix build/, $(CSOURCE:.c=.o)) $(addprefix build/, $(SSOURCE:.s=.o))
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

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
