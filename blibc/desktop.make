XCC     = gcc
AS		= as
AR      = ar

OUT     = libblibc_desktop.a

CFLAGS  = -c -fPIC -Wall -I./include -DDESKTOP_TARGET
ASFLAGS	=
ARFLAGS = rcs