#
# Makefile for unit tests.
#
# Can only be built for Linux, not for the embedded
# hardware.
#
include ../preprocessor.make

OUTDIR = build
TESTS = byteBuffer

%.o: %.c
	gcc -c -Wall $(PRE_FLAGS) -I../blibc/include -o $(addprefix build/, $@) $^

%.out: %.o
	gcc -L../blibc/lib -o build/$@ $(addprefix build/, $^) -lblibc_desktop

all: directories $(addsuffix Test.out, $(TESTS))

.PHONY: clean directories
.SUFFIXES:
.SECONDARY:

clean:
	-rm -rf $(OUTDIR)

directories:
	mkdir -p $(OUTDIR)
