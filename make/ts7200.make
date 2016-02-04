XCC     = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/gcc
AS      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/as
LD      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/ld
CP	= 
DM 	= /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/objdump

OUT		= k3.elf
HRDWR	= ts7200

CFLAGS  = -fPIC -O2 -Wall  -nostdlib -nostartfiles -ffreestanding  -I. -mcpu=arm920t -msoft-float -DTARGET_TS7200
LDFLAGS = -Map bin/main.map -N -T make/loadmap.ld
LDLIBS  =
ASFLAGS = -mcpu=arm920t -mapcs-32

ts7200_install: $(OUTDIR)/$(OUT) 
	cp $< /u/cs452/tftp/ARM/$(LOGNAME)
