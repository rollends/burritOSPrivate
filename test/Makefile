#
# Makefile for unit tests.
#
# Can only be built for Linux, not for the embedded
# hardware.
#

OUTDIR = build
TESTS = byteBuffer

%.o: %.c
	gcc -c -Wall -DNULL_CHECK -DBOUNDS_CHECK -DZERO_MEMORY -I../blibc/include -o $@ $^

%.out: %.o
	gcc -L../blibc/lib -o build/$@ $^ -lblibc_desktop

all: directories $(addsuffix Test.out, $(TESTS))

.PHONY: clean directories
.SUFFIXES:
.SECONDARY:

clean:
	-rm -rf $(OUTDIR)

directories:
	mkdir -p $(OUTDIR)
