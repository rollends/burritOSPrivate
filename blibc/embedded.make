XCC     = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/gcc
AS		= /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/as
AR      = /u/wbcowan/gnuarm-4.0.2/arm-elf/bin/ar

OUT     = libblibc_embdedded.a

CFLAGS  = -c -fPIC -Wall -I./include -mcpu=arm920t -msoft-float -DEMBEDDED_TARGET
ASFLAGS	= -mcpu=arm920t -mapcs-32
ARFLAGS = rcs