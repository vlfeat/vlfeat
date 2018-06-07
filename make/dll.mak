# file: dll.mak
# description: Build VLFeat DLL
# author: Andrea Vedaldi

# Copyright (C) 2013-14,18 Andrea Vedaldi.
# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

all: dll-all
clean: dll-clean
archclean: dll-archclean
distclean: dll-distclean
info: dll-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

# LINK_DLL_CLFAGS and LINK_DLL_LDFLAGS are the compiler options needed
# to link to the VLFeat DLL. DLL_CLFAGS and DLL_LDFLAGS the options to
# build the DLL.

DLL_NAME = vl

LINK_DLL_CFLAGS = \
$(if $(DISABLE_THREADS),-DVL_DISABLE_THREADS) \
$(if $(DISABLE_OPENMP),-DVL_DISABLE_OPENMP) \
$(if $(DISABLE_SSE2),-DVL_DISABLE_SSE2) \
$(if $(DISABLE_AVX),-DVL_DISABLE_AVX) \
-I$(VLDIR)

LINK_DLL_LDFLAGS =\
-L$(BINDIR) -lvl

DLL_CFLAGS = \
$(STD_CFLAGS) \
-fvisibility=hidden -fPIC -DVL_BUILD_DLL \
$(LINK_DLL_CFLAGS) \
$(call if-like,%_sse2,$*, $(if $(DISABLE_SSE2),,-msse2)) \
$(call if-like,%_avx,$*, $(if $(DISABLE_AVX),,-mavx)) \
$(if $(DISABLE_THREADS),,-pthread) \
$(if $(DISABLE_OPENMP),,-fopenmp)

DLL_LDFLAGS = \
$(STD_LDFLAGS) \
-lm \
$(if $(DISABLE_THREADS),,-lpthread) \
$(if $(DISABLE_OPENMP),,-fopenmp)

BINDIR = bin/$(ARCH)

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
DLL_SUFFIX := dylib
DLL_LDFLAGS += -m32
endif

# Mac OS X on Intel 64 bit processor
ifeq ($(ARCH),maci64)
DLL_SUFFIX := dylib
DLL_LDFLAGS += -m64
endif

# Linux-32
ifeq ($(ARCH),glnx86)
DLL_SUFFIX := so
DLL_LDFLAGS += -m32
endif

# Linux-64
ifeq ($(ARCH),glnxa64)
DLL_SUFFIX := so
DLL_LDFLAGS += -m64
endif

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

# On Mac OS X the library install_name is prefixed with @loader_path/.
# At run time this causes the loader to search for a local copy of the
# library for any binary which is linked against it. The install_name
# can be modified later by install_name_tool.

dll_tgt := $(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)
dll_src := $(wildcard $(VLDIR)/vl/*.c)
dll_hdr := $(wildcard $(VLDIR)/vl/*.h)
dll_obj := $(addprefix $(BINDIR)/objs/, $(notdir $(dll_src:.c=.o)))
dll_dep := $(dll_obj:.o=.d)

arch_bins += $(dll_tgt)
comm_bins +=
deps += $(dll_dep)

.PHONY: dll
.PHONY: dll-all, dll-clean, dll-archclean, dll-distclean
.PHONY: dll-info
no_dep_targets += dll-dir dll-clean dll-archclean dll-distclean
no_dep_targets += dll-info

dll-all: dll
dll: $(dll_tgt)

# generate the dll-dir target
$(eval $(call gendir, dll, $(BINDIR) $(BINDIR)/objs))

$(BINDIR)/objs/%.o : $(VLDIR)/vl/%.c $(dll-dir)
	$(call C,CC)                                            \
	     -c -o "$(@)"                                       \
	     $(DLL_CFLAGS) "$(<)"

$(BINDIR)/objs/%.d : $(VLDIR)/vl/%.c $(dll-dir)
	$(call C,CC)						\
	     -MM						\
	     -MF "$(@)"						\
	     -MT '$(BINDIR)/objs/$*.o $(BINDIR)/objs/$*.d'      \
	     $(DLL_CFLAGS) "$(<)"

$(BINDIR)/lib$(DLL_NAME).dylib : $(dll_obj)
	$(call C,CC)						\
	  -dynamiclib						\
	  -undefined suppress					\
	  -flat_namespace					\
	  -install_name @loader_path/lib$(DLL_NAME).dylib	\
	  -compatibility_version $(VER)				\
	  -current_version $(VER)				\
	  -isysroot $(SDKROOT)					\
	  $(DLL_LDFLAGS)					\
	  $(^)							\
	  -o "$(@)"
ifdef BIN_RELINK_OMP
	make/macos_relink_omp.sh "$(@)"
endif

$(BINDIR)/lib$(DLL_NAME).so : $(dll_obj)
	$(call C,CC) -shared                                    \
	    $(^)                                                \
	    $(DLL_LDFLAGS)	                                \
	    -o "$(@)"

dll-clean:
	rm -f $(dll_dep) $(dll_obj)

dll-archclean: dll-clean
	rm -rf $(BINDIR)

dll-distclean:
	rm -rf bin

dll-info:
	$(call echo-title,VLFeat core library)
	$(call dump-var,dll_hdr)
	$(call dump-var,dll_src)
	$(call dump-var,dll_obj)
	$(call dump-var,dll_dep)
	$(call echo-var,BINDIR)
	$(call echo-var,DLL_NAME)
	$(call echo-var,LINK_DLL_CFLAGS)
	$(call echo-var,LINK_DLL_LDFLAGS)
	$(call echo-var,DLL_CFLAGS)
	$(call echo-var,DLL_LDFLAGS)
	$(call echo-var,DLL_SUFFIX)
	$(call echo-var,LIBTOOL)
	@echo

# Local variables:
# mode: Makefile
# End:
