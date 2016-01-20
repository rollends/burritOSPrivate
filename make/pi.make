XCC		= arm-none-eabi-gcc
AS		= arm-none-eabi-as
LD		= arm-none-eabi-ld
CP		= arm-none-eabi-objcopy
DM		= arm-none-eabi-objdump

OUT		= main.bin

CFLAGS	= -Wall -O3 -nostdlib -nostartfiles -ffreestanding
LDFLAGS = -init main -Map bin/main.map -N -T make/loadmap.ld
ASFLAGS = 

FILES	= build/main_pi.o build/start.o build/contextSwitch.o