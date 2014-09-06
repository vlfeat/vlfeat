# file: matlab.mak
# description: Build MATALB toolbox
# author: Andrea Vedaldi

# Copyright (C) 2013-14 Andrea Vedaldi.
# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

# MATLAB support is enabled if $(MEX) is executable and if MATLAB root
# can be deduced from the output of `$(MEX) -v`. Therefore setting MEX to
# the empty string disables MATLAB support.

MEX ?= mex
MATLAB_PATH ?= $(subst /bin/mex,,$(realpath $(shell which '$(MEX)')))
MATLAB_EXE ?= "$(MATLAB_PATH)/bin/matlab"

# transform in immediate for efficiency
MATLAB_PATH := $(MATLAB_PATH)

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

# With 2014a the new mex uses a revamped configuration system. It also
# breaks in subtle way how variables need to be escaped when passed to the
# mex command.

ifeq ($(strip $(shell $(MEX) -v 2>&1 | grep MATLAB)),)
# new style
$(info MATLAB 2014a or greater detected)
escape =$(1)
else
# old style
$(info MATLAB 2013b or earlier detected)
escape =$(subst $$,\\$$,$(1))
endif

# --------------------------------------------------------------------
#                                                  Prepare MEX options
# --------------------------------------------------------------------
#
# MATLAB provides the MEX command to compile and link MEX files. MEX is
# in fact a wrapper of the host compiler. By itself, it understand a
# set of standard compiler flags, but not, in general, flags which are
# specific to the underlying complier.
#
# The MEX command is called as follows:
#
#   $(MEX) $(MEX_FLAGS) $(MEX_CFLAGS) $(MEX_LDFLAGS)
#
# where
#
# * MEX_CLFAGS are standard compiler flags such as -I (include path).
# * MEX_LDFLAGS are standard linker flags such as -L (library path)
#   and -l (link library)
# * MEX_FLAGS are other MEX flags such as -v (verbose). This variable
#   is also used to contain overrides for the variables used
#   internally by MEX.
#
# The variables STD_CLFAGS and STD_LDFLAGS contain settings which are
# specific to one complier (e.g. GCC or clang). To pass these to MEX,
# the following is appended to MEX_FLAGS, and ultimately to the MEX
# command line:
#
#   CFLAGS='$$CFLAGS $(STD_CFLAGS)'
#   LDFLAGS='$$LDFLAGS $(STD_LDFLAGS)'
#
# This causes MEX to append $(STD_CLFAGS) and $(STD_LDFLAGS) to its
# default settings.
#
# While this usually achieves the desired effects, some versions of
# MATLAB may not be compatible with certain compilers (e.g. MATLAB
# 2013a and Xcode 5.0 and Mac OS X 10.9). Fixing this requires
# changing CLFAGS and LDFLAGS completely (i.e.  not just appending to
# their default values).

MEX_ARCH = $(ARCH)
MEX_CFLAGS = $(LINK_DLL_CFLAGS) -Itoolbox
MEX_LDFLAGS = $(subst bin/$(ARCH),toolbox/mex/$(MEX_SUFFIX),$(LINK_DLL_LDFLAGS)) -lm
MEX_FLAGS = \
$(MEXFLAGS) \
-$(MEX_ARCH) \
$(if $(VERB),-v,) \
$(if $(DEBUG),-g,-O) \
$(if $(PROFILE),-g -O,)

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
MEX_SUFFIX := mexmaci
MEX_FLAGS += CC='$(CC)'
MEX_FLAGS += LD='$(CC)'
# a hack to support recent Xcode/clang/GCC versions on old MATLABs
MEX_FLAGS += CFLAGS='\
-arch i386 \
-fno-common \
-fexceptions \
$(call escape,$(STD_CFLAGS))'
MEX_FLAGS += LDFLAGS='\
-arch i386 \
-Wl,-syslibroot,$(SDKROOT) \
-mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET) \
-bundle -Wl,-exported_symbols_list,$(MATLAB_PATH)/extern/lib/maci/mexFunction.map \
$(if $(DISABLE_OPENMP),,-L$(MATLAB_PATH)/sys/os/$(ARCH)/) \
$(call escape,$(STD_LDFLAGS))'
endif

# Mac OS X on Intel 64 bit processor
ifeq ($(ARCH),maci64)
MEX_SUFFIX := mexmaci64
MEX_FLAGS += -largeArrayDims
MEX_FLAGS += CC='$(CC)'
MEX_FLAGS += LD='$(CC)'
MEX_FLAGS += CFLAGS='\
-arch x86_64 \
-fno-common \
-fexceptions \
$(call escape,$(STD_CFLAGS))'
MEX_FLAGS += LDFLAGS='\
-arch x86_64 \
-Wl,-syslibroot,$(SDKROOT) \
-mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET) \
-bundle -Wl,-exported_symbols_list,$(MATLAB_PATH)/extern/lib/maci64/mexFunction.map \
$(if $(DISABLE_OPENMP),,-L$(MATLAB_PATH)/sys/os/$(ARCH)/) \
$(call escape,$(STD_LDFLAGS))'
endif

# Linux on 32 bit processor
ifeq ($(ARCH),glnx86)
MEX_SUFFIX := mexglx
MEX_FLAGS += CFLAGS='$$CFLAGS $(call escape,$(STD_CFLAGS))'
MEX_FLAGS += LDFLAGS='$$LDFLAGS $(call escape,$(STD_LDFLAGS))'
endif

# Linux on 64 bit processorm
ifeq ($(ARCH),glnxa64)
MEX_SUFFIX := mexa64
MEX_FLAGS += -largeArrayDims
MEX_FLAGS += CFLAGS='$$CFLAGS $(call escape,$(STD_CFLAGS))'
MEX_FLAGS += LDFLAGS='$$LDFLAGS $(call escape,$(STD_LDFLAGS))'
endif

MEX_BINDIR := toolbox/mex/$(MEX_SUFFIX)

# For efficiency reasons, immediately expand this variable once
MEX_FLAGS := $(MEX_FLAGS)

# --------------------------------------------------------------------
#                                                         Sanity check
# --------------------------------------------------------------------

err_no_mex_suffix = $(shell echo "** Could not set MEX_SUFFIX for ARCH = $(ARCH)"  1>&2)
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

mex_sub := $(shell find $(VLDIR)/toolbox -type d)
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

vpath vl_%.c $(mex_sub)

mex-all: $(mex_dll) $(mex_tgt)

# generate the mex-dir target
$(eval $(call gendir, mex, $(MEX_BINDIR)))

# Create a copy of the VLFeat DLL that links to MATLAB OpenMP library
# (Intel OMP 5) rather than the system one. The Intel library is
# binary compatible with GCC. This avoids running two OpenMP
# subsystems when VLFeat runs within MATLAB.

$(MEX_BINDIR)/lib$(DLL_NAME).dylib : $(mex-dir) $(dll_obj)
	$(call C,CC)                                                            \
	    -dynamiclib								\
	    -undefined suppress							\
	    -flat_namespace							\
	    -install_name @loader_path/lib$(DLL_NAME).dylib			\
	    -compatibility_version $(VER)					\
	    -current_version $(VER)						\
	    -isysroot $(SDKROOT)						\
	    $(dll_obj)								\
	    $(filter-out -fopenmp, $(DLL_LDFLAGS))                              \
	    $(if $(DISABLE_OPENMP),,-L$(MATLAB_PATH)/bin/$(ARCH)/)              \
	    $(if $(DISABLE_OPENMP),,-L$(MATLAB_PATH)/sys/os/$(ARCH)/ -liomp5)	\
	   -o $@

$(MEX_BINDIR)/lib$(DLL_NAME).so : $(mex-dir) $(dll_obj)
	$(call C,CC) -shared							\
	    $(dll_obj)							        \
	    $(filter-out -fopenmp, $(DLL_LDFLAGS))                              \
	    $(if $(DISABLE_OPENMP),,-L$(MATLAB_PATH)/bin/$(ARCH)/)              \
	    $(if $(DISABLE_OPENMP),,-L$(MATLAB_PATH)/sys/os/$(ARCH)/ -liomp5)   \
	   -o $(@)

$(MEX_BINDIR)/%.d : %.c $(mex-dir)
	$(call C,CC)								\
	    -MM									\
	    -MF "$(@)"								\
	    -MT '$(MEX_BINDIR)/$*.$(MEX_SUFFIX) $(MEX_BINDIR)/$*.d'		\
	    -I"$(MATLAB_PATH)/extern/include"					\
	    $(MEX_CFLAGS) "$(<)"

$(MEX_BINDIR)/%.$(MEX_SUFFIX) : %.c $(mex-dir) $(mex_dll)
	$(call C,MEX)								\
	    $(MEX_FLAGS)							\
	    $(MEX_CFLAGS)							\
	    "$(<)"								\
	    $(MEX_LDFLAGS)							\
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

mex-test: mex-all
	cd toolbox ; \
	$(MATLAB_EXE) \
	    -$(ARCH) -nodesktop -nosplash \
	    -r "clear mex;vl_setup test;r=vl_test();if(any(~[r.succeded])),disp('MEX testing failed');end;exit"

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

matlab-noprefix: $(m_lnk)

toolbox/noprefix/%.m : vl_%.m $(matlab-noprefix-dir)
	@upperName=`echo "$*" | tr [a-z]  [A-Z]` ;              \
	echo "function varargout = $*(varargin)" > "$@" ;       \
	cat "$<" | sed -n -e '/^function/b' -e '/^%.*$$/p'      \
             -e '/^%.*$$/b' -e q >> "$@" ;                      \
	echo "[varargout{1:nargout}] = vl_$*(varargin{:});" >> "$@" ;

matlab-test:
	@echo "Testing Matlab toolbox" ;			\
	cd toolbox ;						\
	RESULT=$$(						\
	$(MATLAB_EXE) -$(ARCH) -nodesktop -r			\
	"vl_setup('xtest','verbose') ; vl_test ; exit") ;	\
	echo "$$RESULT" ;					\
	if test -n "$$(echo \"$$RESULT\" | grep \"failed\")" ;	\
	then							\
	  echo "Matlab toolbox test encountered an error!" ;	\
	  exit 1 ;						\
	else							\
	  echo "Matlab toolbox test completed successfully!" ;	\
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
