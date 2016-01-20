XCC     = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/gcc
AS      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/as
LD      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/ld
CP	= 
DM 	= /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/objdump

OUT	= main.elf

CFLAGS  = -fPIC -O3 -Wall -I. -mcpu=arm920t -msoft-float
LDFLAGS = -L/u/wbcowan/gnuarm-4.0.2/lib/gcc/arm-elf/4.0.2 -Map bin/main.map -N -T make/loadmap.ld
LDLIBS  = -lgcc
ASFLAGS = -mcpu=arm920t -mapcs-32

ts7200_install: $(OUTDIR)/$(OUT) 
	cp $< /u/cs452/tftp/ARM/tpetrick
