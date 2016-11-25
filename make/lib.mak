# file:        Makefile.lib
# description: Build VLFeat static lib
# author:      Viktor Gal

# AUTORIGHTS

all: lib-all
clean: lib-clean
archclean: lib-archclean
distclean: lib-distclean
info: lib-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

LIB_NAME = vl

LIB_CFLAGS  = $(CFLAGS)
LIB_CFLAGS += -fvisibility=hidden -fPIC -DVL_BUILD_DLL -pthread
LIB_CFLAGS += $(call if-like,%_sse2,$*,-msse2)

BINDIR = bin/$(ARCH)

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
LIB_SUFFIX := a
LIBTOOL := libtool
endif

# Mac OS X on Intel 64 bit processor
ifeq ($(ARCH),maci64)
LIB_SUFFIX := a
LIBTOOL := libtool
endif

# Linux-32
ifeq ($(ARCH),glx)
LIB_SUFFIX := a
endif

# Linux-64
ifeq ($(ARCH),glnxa64)
LIB_SUFFIX := a
endif

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

# On Mac OS X the library install_name is prefixed with @loader_path/.
# At run time this causes the loader to search for a local copy of the
# library for any binary which is linked against it. The install_name
# can be modified later by install_name_tool.

lib_tgt := $(BINDIR)/lib$(LIB_NAME).$(LIB_SUFFIX)
lib_src := $(wildcard $(VLDIR)/vl/*.c)
lib_obj := $(addprefix $(BINDIR)/objs/, $(notdir $(lib_src:.c=.o)))
lib_dep := $(lib_obj:.o=.d)

arch_bins += $(lib_tgt)
comm_bins +=
deps += $(lib_dep)

.PHONY: lib
.PHONY: lib-all, lib-clean, lib-archclean, lib-distclean
.PHONY: lib-info
no_dep_targets += lib-dir lib-clean lib-archclean lib-distclean
no_dep_targets += lib-info

lib-all: lib
lib: $(lib_tgt)

# generate the lib-dir target
$(eval $(call gendir, lib, $(BINDIR) $(BINDIR)/objs))

$(BINDIR)/objs/%.o : $(VLDIR)/vl/%.c $(lib-dir)
	$(call C,CC) $(LIB_CFLAGS) -c "$(<)" -o "$(@)"

$(BINDIR)/objs/%.d : $(VLDIR)/vl/%.c $(lib-dir)
	$(call C,CC) $(LIB_CFLAGS)                                   \
	       -M -MT '$(BINDIR)/objs/$*.o $(BINDIR)/objs/$*.d'      \
	       "$(<)" -MF "$(@)"

$(BINDIR)/lib$(LIB_NAME).a : $(lib_obj)
	$(call C,AR) cru $(@) $(^)
	$(call C,RANLIB) $(@)

lib-clean:
	rm -f $(lib_dep) $(lib_obj)

lib-archclean: lib-clean
	rm -rf $(BINDIR)

lib-distclean:
	rm -rf bin

lib-info:
	$(call echo-title,VLFeat core library)
	$(call dump-var,lib_src)
	$(call dump-var,lib_obj)
	$(call dump-var,lib_dep)
	$(call echo-var,BINDIR)
	$(call echo-var,LIB_NAME)
	$(call echo-var,LIB_CFLAGS)
	$(call echo-var,LIB_SUFFIX)
	$(call echo-var,LIBTOOL)
	@echo

# Local variables:
# mode: Makefile
# End:
