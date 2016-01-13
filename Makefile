#
# Top level Makefile for building all subdirectories
#

DIRS = blibc test

.PHONY: all $(DIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@

clean:
	$(MAKE) -C blibc clean
	$(MAKE) -C test clean