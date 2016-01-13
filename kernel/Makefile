#
# Makefile for burritOS C standard lib.
#
# Can be built for the board with TARGET=embedded or 
# for a Linux machine with TARGET=desktop. By default,
# the embedded target is chosen. 
#
include ../preprocessor.make
include $(TARGET).make

OUTDIR = build
LIBDIR = lib
SOURCE = contextSwitch.s
HEADER = 

_HEADER = $(addprefix include/, $(HEADER))

all: directories $(LIBDIR)/$(OUT) $(OUTDIR)/testContextSwitch.elf

$(OUTDIR)/%.s: src/%.c $(_HEADER)
	$(XCC) -S $(CFLAGS) $(PRE_FLAGS) -o $@ $< 

$(OUTDIR)/%.s: src/%.s
	cp $< $@

$(OUTDIR)/%.o: build/%.s
	$(AS) $(ASFLAGS) -o $@ $^

$(LIBDIR)/$(OUT): $(addsuffix .o, $(addprefix $(OUTDIR)/, $(basename $(SOURCE))))
	$(AR) $(ARFLAGS) $@ $^

$(OUTDIR)/testContextSwitch.elf: $(LIBDIR)/$(OUT) test/testContextSwitch.c
	$(XCC) $(LDFLAGS) $(PRE_FLAGS) -o $@ $^

.PHONY: clean directories
.SUFFIXES:
.SECONDARY:

clean:
	-rm -rf $(OUTDIR)

directories:
	mkdir -p $(OUTDIR)
	mkdir -p $(LIBDIR)
