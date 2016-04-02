XCC     = /u/rs2dsouza/arm-none-eabi-gcc/bin/arm-none-eabi-gcc
AS      = /u/rs2dsouza/arm-none-eabi-gcc/bin/arm-none-eabi-as
LD      = /u/rs2dsouza/arm-none-eabi-gcc/bin/arm-none-eabi-ld
CP	    = 
DM 	    = /u/rs2dsouza/arm-none-eabi-gcc/bin/arm-none-eabi-objdump

OUT		= p1.elf
HRDWR	= ts7200

CFLAGS  = -fPIC -O3 -Wall -Werror -nostdlib -nostartfiles -ffreestanding  -I. -mcpu=arm920t -msoft-float -DTARGET_TS7200
LDFLAGS = -Map bin/main.map -N -T make/loadmap.ld -L/u/rs2dsouza/arm-none-eabi-gcc/lib/gcc/arm-none-eabi/4.9.0 --entry 0x50000
LDLIBS  = -lgcc
ASFLAGS = -mcpu=arm920t -mapcs-32

ts7200_install: $(OUTDIR)/$(OUT) 
	cp $< /u/cs452/tftp/ARM/$(LOGNAME)
