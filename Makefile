XCC     = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/gcc
AS      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/as
LD      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/ld

CFLAGS  = -c -fPIC -Wall -I. -mcpu=arm920t -msoft-float -DEMBEDDED_BUILD
ASFLAGS = -mcpu=arm920t -mapcs-32
LDFLAGS = -init main -Map bin/main.map -N -T loadmap.ld -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 -L../lib
LDLIBS  = -lgcc

all: directories bin/main.elf

build/%.s: %.c
	$(XCC) -S $(CFLAGS) $< -o $@

build/%.o: build/%.s
	$(AS) $(ASFLAGS) -o $@ $^

build/%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

bin/main.elf: build/uart.o build/main.o build/start.o build/contextSwitch.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

install: main.elf
	cp $< /u/cs452/tftp/ARM/tpetrick

directories:
	mkdir -p build
	mkdir -p bin
clean:
	rm -rf build
	rm -rf bin

.SUFFIXES:
.SECONDARY:
.PHONY: clean directories
