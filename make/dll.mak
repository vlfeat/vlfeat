# file:        Makefile.dll
# description: Build VLFeat DLL
# author:      Andrea Vedaldi

# AUTORIGHTS

all: dll-all
clean: dll-clean
archclean: dll-archclean
distclean: dll-distclean
info: dll-info

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

DLL_NAME = vl

DLL_CFLAGS  = $(CFLAGS)
DLL_CFLAGS += -fvisibility=hidden -fPIC -DVL_BUILD_DLL -pthread
DLL_CFLAGS += $(call if-like,%_sse2,$*,-msse2)

BINDIR = bin/$(ARCH)

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
DLL_SUFFIX := dylib
LIBTOOL := libtool
endif

# Mac OS X on Intel 64 bit processor
ifeq ($(ARCH),maci64)
DLL_SUFFIX := dylib
LIBTOOL := libtool
endif

# Linux-32
ifeq ($(ARCH),glx)
DLL_SUFFIX := so
endif

# Linux-64
ifeq ($(ARCH),a64)
DLL_SUFFIX := so
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
	$(call C,CC) $(DLL_CFLAGS) -c "$(<)" -o "$(@)"

$(BINDIR)/objs/%.d : $(VLDIR)/vl/%.c $(dll-dir)
	$(call C,CC) $(DLL_CFLAGS)                                   \
	       -M -MT '$(BINDIR)/objs/$*.o $(BINDIR)/objs/$*.d'      \
	       "$(<)" -MF "$(@)"

$(BINDIR)/lib$(DLL_NAME).dylib : $(dll_obj)
	$(call C,LIBTOOL) -dynamic                                   \
                    -flat_namespace                                  \
                    -install_name @loader_path/lib$(DLL_NAME).dylib  \
	            -compatibility_version $(VER)                    \
                    -current_version $(VER)                          \
                    -syslibroot $(SDKROOT)                           \
		    -macosx_version_min 10.5                         \
	            -o $@ -undefined suppress $^

$(BINDIR)/lib$(DLL_NAME).so : $(dll_obj)
	$(call C,CC) $(DLL_CFLAGS) -shared $(^) -o $(@)

dll-clean:
	rm -f $(dll_dep) $(dll_obj)

dll-archclean: dll-clean
	rm -rf $(BINDIR)

dll-distclean:
	rm -rf bin

dll-info:
	$(call echo-title,VLFeat core library)
	$(call dump-var,dll_src)
	$(call dump-var,dll_obj)
	$(call dump-var,dll_dep)
	$(call echo-var,BINDIR)
	$(call echo-var,DLL_NAME)
	$(call echo-var,DLL_CFLAGS)
	$(call echo-var,DLL_SUFFIX)
	$(call echo-var,LIBTOOL)
	@echo

# Local variables:
# mode: Makefile
# End:
