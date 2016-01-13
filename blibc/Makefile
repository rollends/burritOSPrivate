#
# Makefile for burritOS C standard lib.
#
# Can be built for the board with TARGET=embedded or 
# for a Linux machine with TARGET=desktop. By default,
# the embedded target is chosen. 
#
ifndef TARGET
    TARGET=embedded
endif

include ../preprocessor.make
include $(TARGET).make

OUTDIR = build
LIBDIR = lib
SOURCE = byteBuffer.c
HEADER = byteBuffer.h

_HEADER = $(addprefix include/, $(HEADER))

all: directories $(OUT)

%.s: src/%.c $(_HEADER)
	$(XCC) -S $(CFLAGS) $(PRE_FLAGS) -o $(addprefix build/, $@) $< 

%.o: %.s
	$(AS) $(ASFLAGS) -o $(addprefix build/, $@) $(addprefix build/, $^)

$(OUT): $(SOURCE:.c=.o)
	$(AR) $(ARFLAGS) lib/$@ $(addprefix build/, $^)

.PHONY: clean directories
.SUFFIXES:
.SECONDARY:

clean:
	-rm -rf $(OUTDIR)

directories:
	mkdir -p $(OUTDIR)
	mkdir -p $(LIBDIR)