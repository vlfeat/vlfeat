# file:        Makefile
# author:      Andrea Vedaldi
# description: Build

CFLAGS += -I. -g -O0 -DNDEBUG -Wall -std=c89 -Wno-long-long -pedantic -Wall
CFLAGS += -DVL_LOWE_STRICT 
CFLAGS += -DVL_USE_FAST_MATH

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------

# Determine on the flight the system we are running on
Darwin_ARCH := mac
Linux_ARCH  := glx
Cygwin_ARCH := cyg

ARCH := $($(shell uname)_ARCH)

mac_SPECS := -DVL_ARCH_MAC
glx_SPECS := -DVL_ARCH_GLX
cyg_SPECS := -DVL_ARCH_CYG

CFLAGS += $($(ARCH)_SPECS)

MODS := getopt_long generic pgm stringop mathop imop mser sift
OBJS := $(addsuffix .o, $(MODS))
SRCS := $(addsuffix .c, $(MODS))

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------

.PHONY: all
all: libvl.a mser sift

.PHONY: info
info :
	@echo "MODS = $(MODS)"
	@echo "OBJS = $(OBJS)"
	@echo "SRCS = $(SRCS)"

libvl.a : $(OBJS)
	ar rcs $@ $^

mser :  mser-driver.o libvl.a
	cc $(CFLAGS) $^ -o $@

sift : sift-driver.o libvl.a
	cc $(CFLAGS) $^ -o $@

test_getopt_long : test_getopt_long.o libvl.a
	cc $(CFLAGS) $^ -o $@

test_stringop : test_stringop.o libvl.a
	cc $(CFLAGS) $^ -o $@

generic.o          : generic.h generic.c
pgm.o              : pgm.h  pgm.c
mser.o             : mser.c mser.c
sift.o             : sift.h sift.c
imop.o             : imop.h imop.c imop-t.c
stringop.o         : stringop.h stringop.c
getopt_long.o      : getopt_long.h getopt_long.c
mser-driver.o      : mser.h generic.h pgm.h imop.h mathop.h getopt_long.h
sift-driver.o      : sift.h generic.h pgm.h imop.h mathop.h getopt_long.h
test_getopt_long.o : test_getopt_long.c getopt_long.h

.PHONY: clean
clean:
	rm -f *.o
	find . -name '*~' -exec rm -f \{\} \;
	find . -name '.DS_Store' -exec rm -f \{\} \;

.PHONY: distclean
distclean: clean
	rm -f mser sift
	rm -f libvl.a
	rm -f test_getopt_long test_stringop
