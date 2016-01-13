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

include $(TARGET).make

OUTDIR = build
LIBDIR = lib
SOURCE = byteBuffer.c
HEADER = byteBuffer.h

_HEADER = $(addprefix include/, $(HEADER))

all: directories $(OUT)

%.s: src/%.c $(_HEADER)
	$(XCC) -S $(CFLAGS) -DNULL_CHECK -DBOUNDS_CHECK -DZERO_MEMORY -o build/$@ $< 

%.o: %.s
	$(AS) $(ASFLAGS) -o build/$@ build/$^

$(OUT): $(SOURCE:.c=.o)
	$(AR) $(ARFLAGS) lib/$@ $(addprefix build/, $^)

.PHONY: clean directories
.SUFFIXES:
.SECONDARY:

clean:
	-rm -rf $(OUTDIR) $(LIBDIR)

directories:
		mkdir -p $(OUTDIR)
		mkdir -p $(LIBDIR)