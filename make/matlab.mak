# file: matlab.mak
# description: Build MATALB toolbox
# author: Andrea Vedaldi

# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

# MATLAB support is enabled if $(MEX) is executable and if MATLAB root
# can be deduced from the output of `$(MEX) -v`. Therefore setting MEX to
# the empty string disables MATLAB support.

MEX ?= mex
MATLAB_PATH ?= $(strip $(shell test "$$(command -v '$(MEX)')" && \
  $(MEX) -v 2>&1 | sed -n 's/.*MATLAB *= *\(.*\)/\1/gp'))
MATLAB_EXE ?= "$(MATLAB_PATH)/bin/matlab"

# if expand to empty string, set to empty string for use with ifdef
ifeq ($(MATLAB_PATH),)
MATLAB_PATH=
endif

ifdef MATLAB_PATH
all: mex-all matlab-all
clean: mex-clean matlab-clean
archclean: mex-archclean matlab-archclean
distclean: mex-distclean matlab-distclean
endif

info: mex-info matlab-info

MEX_ARCH = $(ARCH)

MEX_CFLAGS = -I$(VLDIR) -I$(VLDIR)/toolbox
MEX_LDFLAGS = -L$(BINDIR) -lvl

MEX_FLAGS = $(MEXFLAGS)
MEX_FLAGS += -$(MEX_ARCH) -largeArrayDims
MEX_FLAGS += $(if $(DEBUG), -g, -O)
MEX_FLAGS += $(if $(PROFILE), -O -g,)
MEX_FLAGS += CFLAGS='$$CFLAGS $(STD_CFLAGS)'

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
MEX_SUFFIX := mexmaci
MEX_FLAGS += LDFLAGS='$$LDFLAGS $(STD_LDFLAGS)'
endif

# Mac OS X on Intel 64 bit processor
ifeq ($(ARCH),maci64)
MEX_SUFFIX := mexmaci64
MEX_FLAGS += LDFLAGS='$$LDFLAGS $(STD_LDFLAGS)'
endif

# Linux on 32 bit processor
ifeq ($(ARCH),glnx86)
MEX_FLAGS += LDFLAGS='$$LDFLAGS $(STD_LDFLAGS) -Wl,--rpath,\\\$$ORIGIN/'
MEX_SUFFIX := mexglx
endif

# Linux on 64 bit processorm
ifeq ($(ARCH),glnxa64)
MEX_FLAGS += LDFLAGS='$$LDFLAGS $(STD_LDFLAGS) -Wl,--rpath,\\\$$ORIGIN/'
MEX_SUFFIX := mexa64
endif

MEX_BINDIR := toolbox/mex/$(MEX_SUFFIX)

# --------------------------------------------------------------------
#                                                         Sanity check
# --------------------------------------------------------------------

err_no_mex_suffix  = $(shell echo "** Could not set MEX_SUFFIX for ARCH = $(ARCH)"  1>&2)
err_no_mex_suffix +=no_mex_suffix

ifeq ($(MEX_SUFFIX),)
die:=$(error $(err_no_mex_suffix))
endif

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

.PHONY: mex-all, mex-dir, mex-info, mex-test
.PHONY: mex-clean, mex-distclean, mex-archclean
no_dep_targets += mex-dir mex-info mex-test
no_dep_targets += mex-clean mex-distclean mex-archclean

mex_src := $(shell find $(VLDIR)/toolbox -name "*.c")
mex_tgt := $(addprefix $(MEX_BINDIR)/,\
	   $(notdir $(mex_src:.c=.$(MEX_SUFFIX)) ) )
mex_dep := $(mex_tgt:.$(MEX_SUFFIX)=.d)
mex_dll := $(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)

ifdef MATLAB_PATH
arch_bins += $(mex_tgt) $(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)
comm_bins +=
deps += $(mex_dep)
endif

vpath vl_%.c $(shell find $(VLDIR)/toolbox -type d)

mex-all: $(mex_tgt)

# generate mex-dir target
$(eval $(call gendir, mex, $(MEX_BINDIR)))

$(mex_dll) : $(dll_tgt) $(mex-dir)
	cp -v "$(<)" "$(@)"

$(MEX_BINDIR)/%.d : %.c $(mex-dir)
	$(call C,CC) \
	       $(MEX_CFLAGS) \
	       -I"$(MATLAB_PATH)/extern/include" -M -MT \
	       '$(MEX_BINDIR)/$*.$(MEX_SUFFIX) $(MEX_BINDIR)/$*.d' \
	       "$(<)" -MF "$(@)"

$(MEX_BINDIR)/%.$(MEX_SUFFIX) : %.c $(mex-dir) $(mex_dll)
	$(call C,MEX) \
	       $(MEX_FLAGS) \
               $(MEX_CFLAGS) \
               "$(<)" \
	       $(MEX_LDFLAGS) \
	       -outdir "$(dir $(@))"

mex-info:
	$(call echo-title,MATLAB support)
	$(if $(MATLAB_PATH),\
	  @echo "MATLAB support enabled (MEX found)",\
	  @echo "MATLAB support disabled (MEX not found)")
	$(call dump-var,mex_src)
	$(call dump-var,mex_tgt)
	$(call dump-var,mex_dep)

mex-clean:
	rm -f $(mex_dep)

mex-archclean: mex-clean
	rm -rf $(MEX_BINDIR)

mex-distclean:
	rm -rf "toolbox/mex" ;

# --------------------------------------------------------------------
#                                                  Prefix-less M files
# --------------------------------------------------------------------
# Populate the directory toolbox/noprefix with links to the MEX / M
# files without the vl_ prefix.

m_src := $(shell find $(VLDIR)/toolbox -name "vl_*.m")
m_lnk := $(addprefix toolbox/noprefix/,                              \
          $(filter-out setup.m,                                      \
          $(filter-out help.m,                                       \
          $(filter-out version.m,                                    \
          $(filter-out root.m,                                       \
          $(filter-out demo.m,                                       \
          $(filter-out compile.m,                                    \
          $(filter-out test_%,                                       \
          $(filter-out demo_%,                                       \
          $(filter-out tune_%,                                       \
          $(subst vl_,,$(notdir $(m_src)))))))))))))
m_lnk += $(addprefix toolbox/noprefix/,                              \
	  $(subst, $(MEX_SUFFIX),.m,$(subst vl_,,$(notdir $(mex_tgt)))))

vpath vl_%.m $(shell find $(VLDIR)/toolbox -type d)

.PHONY: matlab-all, matlab-noprefix, matlab-info
.PHONY: matlab-clean, matlab-archclean, matlab-distclean
no_dep_targets += matlab-all matlab-noprefix matlab-info
no_dep_targets += matlab-clean matlab-archclean matlab-distclean

ifdef MATLAB_PATH
arch_bins +=
comm_bins += $(m_lnk)
deps +=
endif

matlab-all: matlab-noprefix

# generate matlab-noprefix-dir target
$(eval $(call gendir, matlab-noprefix, toolbox/noprefix))

matlab-noprefix: $(matlab-noprefix-dir) $(m_lnk)

toolbox/noprefix/%.m : vl_%.m
	@upperName=`echo "$*" | tr [a-z]  [A-Z]` ;                   \
	echo "function varargout = $*(varargin)" > "$@" ;            \
	cat "$<" | sed -n -e '/^function/b' -e '/^%.*$$/p'           \
             -e '/^%.*$$/b' -e q >> "$@" ;                           \
	echo "[varargout{1:nargout}] = vl_$*(varargin{:});" >> "$@" ;

matlab-test:
	@echo "Testing Matlab toolbox" ; \
	cd toolbox ; \
	RESULT=$$(\
	$(MATLAB_EXE) -$(ARCH) -nodesktop -r \
	"vl_setup('xtest','verbose') ; vl_test ; exit") ; \
	echo "$$RESULT" ; \
	if test -n "$$(echo \"$$RESULT\" | grep \"failed\")" ; \
	then \
	  echo "Matlab toolbox test encountered an error!" ; \
	  exit 1 ; \
	else \
	  echo "Matlab toolbox test completed successfully!" ; \
	fi

matlab-info:
	$(call dump-var,m_src)
	$(call dump-var,m_lnk)
	$(call echo-var,mex_dll)
	$(call echo-var,MATLAB_PATH)
	$(call echo-var,MATLAB_EXE)
	$(call echo-var,MEX)
	$(call echo-var,MEX_FLAGS)
	$(call echo-var,MEX_CFLAGS)
	$(call echo-var,MEX_LDFLAGS)
	@echo

matlab-clean:

matlab-archclean: matlab-clean

matlab-distclean:
	rm -rf toolbox/noprefix

# Emacs:
# Local variables:
# mode: Makefile
# End:
