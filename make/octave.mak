# file:        octave.mak
# description: Build MEX files for GNU Octave
# author:      Andrea Vedaldi

# AUTORIGTHS

all: octave-all
info: octave-info
clean: octave-clean
archclean: octave-archclean
distclean: octave-distclean

OCTAVE ?= octave
MKOCTFILE ?= mkoctfile

OCTAVE_MEX_SUFFIX := mex
OCTAVE_MEX_BINDIR := toolbox/mex
OCTAVE_MEX_FLAGS   = -v
OCTAVE_MEX_CFLAGS  = $(CFLAGS) -I$(VLDIR)/toolbox
OCTAVE_MEX_LDFLAGS = $(LDFLAGS) -L$(BINDIR) -lvl

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
endif

# Mac OS X on Intel 64 bit processor
ifeq ($(ARCH),maci64)
endif

# Linux on 32 bit processor
ifeq ($(ARCH),glx)
OCTAVE_MEX_LDFLAGS += -Wl,--rpath,\\\$$ORIGIN/
endif

# Linux on 64 bit processor
ifeq ($(ARCH),a64)
OCTAVE_MEX_LDFLAGS += -Wl,--rpath,\\\$$ORIGIN/
endif

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

.PHONY: octave-all, octave-mex-all, octave-mex-dir, octave-info
.PHONY: octave-clean octave-archclean octave-distclean
no_dep_targets += octave-info
no_dep_targets += octave-clean octave-archclean octave-distclean

octave_mex_src := $(shell find $(VLDIR)/toolbox -name "*.c")
octave_mex_tgt := $(addprefix $(OCTAVE_MEX_BINDIR)/,\
	   $(notdir $(mex_src:.c=.$(OCTAVE_MEX_SUFFIX)) ) )
octave_mex_dep := $(octave_mex_tgt:.$(OCTAVE_MEX_SUFFIX)=.d)

deps += $(octave_mex_dep)

vpath %.c $(shell find $(VLDIR)/toolbox -type d)
vpath vl_%.m $(shell find $(VLDIR)/toolbox -type d)

octave-all: octave-mex-all
octave-mex-all: $(octave_mex_tgt) matlab-noprefix

# generate octav-emex-dir target
$(eval $(call gendir, octave-mex, $(OCTAVE_MEX_BINDIR)))

$(OCTAVE_MEX_BINDIR)/%.d : %.c $(octave-mex-dir)
	$(call C,MKOCTFILE) $(OCTAVE_MEX_FLAGS) \
	       -M "$(<)"
	@mv "$(<:.c=.d)" $(OCTAVE_MEX_BINDIR)

$(OCTAVE_MEX_BINDIR)/%.$(OCTAVE_MEX_SUFFIX) : %.c $(octave-mex-dir)
	@make -s $(dll_tgt)
	@ln -sf "../../$(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)" \
	        "$(OCTAVE_MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)"
	CFLAGS="$(OCTAVE_MEX_CFLAGS)" \
	LDFLAGS="$(OCTAVE_MEX_LDFLAGS)" \
	 $(MKOCTFILE) \
	       --mex $(OCTAVE_MEX_FLAGS) \
	       "$(<)" --output "$(@)"

octave-info:
	@echo "********************************************** Octave "
	$(call dump-var,octave_mex_src)
	$(call dump-var,octave_mex_tgt)
	$(call dump-var,octave_mex_dep)
	@echo

octave-clean:

octave-archclean:
	rm -f $(octave_mex_tgt)

octave-distclean: octave-archclean

# Emacs:
# Local variables:
# mode: Makefile
# End:
