# file: bin.mak
# description: Build VLFeat command line utilities
# author: Andrea Vedaldi

# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

all: bin-all
clean: bin-clean
archclean: bin-archclean
distclean: bin-distclean
info: bin-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

BIN_CFLAGS = $(STD_CFLAGS) -I$(VLDIR)
BIN_CFLAGS += $(if $(DISABLE_THREADS),,-pthread)
BIN_CFLAGS += $(if $(DISABLE_OPENMP),,-fopenmp)

BIN_LDFLAGS = $(STD_LDFLAGS) -L$(BINDIR) -lvl -lm
BIN_LDFLAGS += $(if $(DISABLE_THREADS),,-lpthread)
BIN_LDFLAGS += $(if $(DISABLE_OPENMP),,-fopenmp)

# Mac OS X Intel 32
ifeq ($(ARCH),maci)
endif

# Mac OS X Intel 64
ifeq ($(ARCH),maci64)
endif

# Linux-32
ifeq ($(ARCH),glnx86)
BIN_LDFLAGS += -Wl,--rpath,\$$ORIGIN/
endif

# Linux-64
ifeq ($(ARCH),glnxa64)
BIN_LDFLAGS += -Wl,--rpath,\$$ORIGIN/
endif

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

# On Mac OS X the library install_name is prefixed with @loader_path/.
# At run time this causes the loader to search for a local copy of the
# library for any binary which is linked against it. The install_name
# can be modified later by install_name_tool.

bin_src := $(wildcard $(VLDIR)/src/*.c)
bin_tgt := $(addprefix $(BINDIR)/, $(patsubst %.c,%,$(notdir $(bin_src))))
bin_dep := $(addsuffix .d, $(bin_tgt))

deps += $(bin_dep)
arch_bins += $(bin_tgt)
comm_bins +=

.PHONY: bin-all, bin-info
.PHONY: bin-clean, bin-archclean, bin-distclean
no_dep_targets += bin-dir bin-clean bin-archclean bin-distclean
no_dep_targets += bin-info

bin-all: $(dll-dir) $(bin_tgt)

# BIN_LDFLAGS includes the libraries to link to and must be
# specified after the object "$<" that uses them. If not, stricter
# linkers (e.g. --as-needed option with the GNU toolchain)
# will break as they will not include the dependencies. See
# also http://wiki.debian.org/ToolChain/DSOLinking

$(BINDIR)/% : $(VLDIR)/src/%.c $(dll_tgt)
	$(call C,CC) $(BIN_CFLAGS) "$<" $(BIN_LDFLAGS) -o "$@"

$(BINDIR)/%.d : $(VLDIR)/src/%.c $(dll-dir)
	$(call C,CC) $(BIN_CFLAGS) -M -MT  \
	       '$(BINDIR)/$* $(BINDIR)/$*.d' \
	       "$<" -MF "$@"

bin-clean:
	rm -f $(bin_dep)

bin-archclean: bin-clean

bin-distclean:

bin-info:
	$(call echo-title,Command line utilities)
	$(call dump-var,bin_src)
	$(call dump-var,bin_tgt)
	$(call dump-var,bin_dep)
	$(call echo-var,BIN_CFLAGS)
	$(call echo-var,BIN_LDFLAGS)
	@echo

# Local variables:
# mode: Makefile
# End:
