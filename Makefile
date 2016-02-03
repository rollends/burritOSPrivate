OUTDIR	= bin
BLDDIR	= build

.DEFAULT_GOAL = all

ifndef TARGET
    TARGET=ts7200
endif

include make/preprocessor.make
include make/$(TARGET).make

CSOURCE := $(shell find common kernel user -type f -name *.c)
CSOURCE += $(shell find hardware/$(HRDWR) -type f -name *.c)
HSOURCE := $(shell find common kernel user -type f -name *.h)
HSOURCE += $(shell find hardware -maxdepth 1 -type f -name *.h)
HSOURCE += $(shell find hardware/$(HRDWR) -type f -name *.h)
SSOURCE := $(shell find common kernel user -type f -name *.s)

_OBJS = $(addprefix $(BLDDIR)/, $(CSOURCE:.c=.o))
_OBJS += $(addprefix $(BLDDIR)/, $(SSOURCE:.s=.o))
_OBJDIRS = $(dir $(_OBJS))

_OUT = $(OUTDIR)/$(OUT)
_OUT += $(OUTDIR)/$(basename $(OUT)).lst

all: directories $(_OUT)  

$(BLDDIR)/%.s: %.c $(HSOURCE)
	$(XCC) -S -I./ $(CFLAGS) $(PREFLAGS) $< -o $@

$(BLDDIR)/%.o: $(BLDDIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $^

$(BLDDIR)/%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

$(OUTDIR)/%.elf: $(_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	$(CP) $< -O binary $@

$(OUTDIR)/%.lst: $(OUTDIR)/%.elf
	$(DM) -D $^ > $@

install: directories $(TARGET)_install

directories:
	mkdir -p $(BLDDIR)
	mkdir -p $(OUTDIR)
	mkdir -p $(_OBJDIRS)

clean:
	rm -rf $(BLDDIR)
	rm -rf $(OUTDIR)

clean_doc:
	rm -rf doc

clean_all: clean_doc clean

doc:
	doxygen doxygen.cfg
	$(MAKE) -C doc/latex
	cp doc/latex/refman.pdf doc/doc.pdf

.SUFFIXES:
.SECONDARY:
.PHONY: clean clean_all clean_doc directories doc install
