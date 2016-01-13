#
# Top level Makefile for building all subdirectories
#

ifndef TARGET
    TARGET=embedded
endif

DIRS = blibc test

.PHONY: all $(DIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@ TARGET=$(TARGET)

clean:
	$(MAKE) -C blibc TARGET=$(TARGET) clean
	$(MAKE) -C test TARGET=$(TARGET) clean