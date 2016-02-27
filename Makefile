OUTDIR	= bin
BLDDIR	= build
UNTY_F  = unity.c

.DEFAULT_GOAL = all

ifndef TARGET
    TARGET = ts7200
endif

ifndef UNITY
	UNITY = 1
endif

include make/preprocessor.make
include make/$(TARGET).make

CSOURCE := $(shell find calibration common kernel user -type f -name *.c)
CSOURCE += $(shell find hardware/$(HRDWR) -type f -name *.c)
HSOURCE := $(shell find common kernel user -type f -name *.h)
HSOURCE += $(shell find hardware -maxdepth 1 -type f -name *.h)
HSOURCE += $(shell find hardware/$(HRDWR) -type f -name *.h)
SSOURCE := $(shell find common kernel user -type f -name *.s)
SSOURCE += $(shell find hardware/$(HRDWR) -type f -name *.s)

ifeq ($(UNITY), 1)
$(BLDDIR)/$(UNTY_F): $(CSOURCE)
	cat $^ > $@

CSOURCE = $(BLDDIR)/$(UNTY_F)
endif

_OBJS = $(addprefix $(BLDDIR)/, $(CSOURCE:.c=.o))
_OBJS += $(addprefix $(BLDDIR)/, $(SSOURCE:.s=.o))
_OBJDIRS = $(dir $(_OBJS))

_OUT = $(OUTDIR)/$(OUT)
_OUT += $(OUTDIR)/$(basename $(OUT)).lst

all: directories $(_OUT)

$(BLDDIR)/%.s: %.c $(HSOURCE) make/preprocessor.make
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

install: directories $(_OUT) $(TARGET)_install

directories:
	mkdir -p $(BLDDIR)
	mkdir -p $(OUTDIR)
	mkdir -p $(_OBJDIRS)

clean:
	rm -rf $(BLDDIR)
	rm -rf $(OUTDIR)

doc:
	$(MAKE) -C doc/

.SUFFIXES:
.SECONDARY:
.PHONY: calibration clean directories doc install
