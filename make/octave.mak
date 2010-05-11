# file:        octave.mak
# description: Build MEX files for GNU Octave
# author:      Andrea Vedaldi

# AUTORIGTHS

# Octave support is experimental. Currently, the MEX files compile
# successfully but a number of bugs and subtle MATLAB
# imcompatibilities make VLFeat not fully functional.
#
# Octave support is enabled if $(MKOCTFILE) is executable. Therefore
# setting MKOCTFILE to the empty string disables Octave support.

OCTAVE ?= octave
MKOCTFILE ?=
OCTAVE_ENABLE ?= $(strip $(shell type -P $(MKOCTFILE) 2>&1 >/dev/null && \
                   $(MKOCTFILE) -p OCTAVE_LIBS 2>&1 | \
                   grep octave))

# if expand to empty string, set to empty string for use with ifdef
ifeq ($(OCTAVE_ENABLE),)
OCTAVE_ENABLE=
endif

ifdef OCTAVE_ENABLE
all: octave-all
clean: octave-clean
archclean: octave-archclean
distclean: octave-distclean
endif

info: octave-info

OCTAVE_MEX_SUFFIX := mex
OCTAVE_MEX_BINDIR := toolbox/mex/octave
OCTAVE_MEX_FLAGS =
OCTAVE_MEX_CFLAGS = $(CFLAGS) -I$(VLDIR)/toolbox
OCTAVE_MEX_LDFLAGS = $(LDFLAGS) -L$(BINDIR) -lvl

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
OCTAVE_MEX_LDFLAGS += -m32
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

ifdef OCTAVE_ENABLE
deps += $(octave_mex_dep)
endif

vpath %.c $(shell find $(VLDIR)/toolbox -type d)
vpath vl_%.m $(shell find $(VLDIR)/toolbox -type d)

octave-all: octave-mex-all
octave-mex-all: $(octave_mex_tgt) matlab-noprefix

# generate octave-mex-dir target
$(eval $(call gendir, octave-mex, $(OCTAVE_MEX_BINDIR)))

$(OCTAVE_MEX_BINDIR)/%.d : %.c $(octave-mex-dir)
	$(call C,MKOCTFILE) $(OCTAVE_MEX_FLAGS) -M "$(<)"
	@mv "$(<:.c=.d)" $(OCTAVE_MEX_BINDIR)

$(OCTAVE_MEX_BINDIR)/%.$(OCTAVE_MEX_SUFFIX) : %.c $(octave-mex-dir)
	@make -s $(dll_tgt)
	@ln -sf "../../../$(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)" \
	        "$(OCTAVE_MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)"
	CFLAGS="$(OCTAVE_MEX_CFLAGS)" \
	CXXFLAGS="$(OCTAVE_MEX_CXXFLAGS)" \
	LDFLAGS="$(OCTAVE_MEX_LDFLAGS)" \
	 $(MKOCTFILE) \
	       --mex $(OCTAVE_MEX_FLAGS) \
	       "$(<)" --output "$(@)"
	@rm -f $(<:.c=.o)

octave-info:
	$(call echo-title,Octave support)
	$(if $(OCTAVE_ENABLE),\
	  @echo "OCTAVE support enabled (MKOCTFILE found)",\
	  @echo "OCTAVE support disabled (MKOCTFILE not found)")
	$(call dump-var,octave_mex_src)
	$(call dump-var,octave_mex_tgt)
	$(call dump-var,octave_mex_dep)
	$(call echo-var,OCTAVE)
	$(call echo-var,MKOCTFILE)
	$(call echo-var,OCTAVE_MEX_BINDIR)
	$(call echo-var,OCTAVE_MEX_FLAGS)
	$(call echo-var,OCTAVE_MEX_CFLAGS)
	$(call echo-var,OCTAVE_MEX_LDLAGS)
	@echo

octave-clean:
	rm -f $(octave_mex_dep)

octave-archclean: octave-clean
	rm -f $(octave_mex_tgt)

octave-distclean: octave-archclean
	rm -rf toolbox/mex/octave

# Emacs:
# Local variables:
# mode: Makefile
# End:
