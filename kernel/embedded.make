XCC     = arm-none-eabi-gcc
AS	= arm-none-eabi-as
AR      = arm-none-eabi-ar

OUT     = kernel_embdedded.a

CFLAGS  = -c -fPIC -Wall -I./include -mcpu=arm920t -msoft-float -DEMBEDDED_TARGET
ASFLAGS	= -mcpu=arm920t -mapcs-32
ARFLAGS = rcs
LDFLAGS = -lgcc -L lib/

