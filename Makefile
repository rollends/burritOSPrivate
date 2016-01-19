ifeq ($(TARGET), pi)
XCC 	= arm-none-eabi-gcc
AS 	= arm-none-eabi-as
LD	= arm-none-eabi-ld
CP	= arm-none-eabi-objcopy
DM 	= arm-none-eabi-objdump
CLFAGS	= -Wall -fCPIC -nostdlib -nostartfiles -ffreestanding
ASFLAGS = 
FILES 	= build/main_pi.o build/start.o build/contextSwitch.o
else
XCC     = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/gcc
AS      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/as
LD      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/ld
CFLAGS  = -fPIC -Wall -I. -mcpu=arm920t -msoft-float
ASFLAGS = -mcpu=arm920t -mapcs-32
FILES	= build/uart.o build/main.o build/start.o build/contextSwitch.o
endif

LDFLAGS = -init main -Map bin/main.map -N -T loadmap.ld

all: directories bin/main.bin bin/main.lst

build/%.s: %.c
	$(XCC) -S $(CFLAGS) $< -o $@

build/%.o: build/%.s
	$(AS) $(ASFLAGS) -o $@ $^

build/%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

bin/main.elf: $(FILES)
	$(LD) $(LDFLAGS) -o $@ $^

bin/main.bin: bin/main.elf
	$(CP) $< -O binary $@

bin/main.lst: bin/main.elf
	$(DM) -D $^ > $@

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
