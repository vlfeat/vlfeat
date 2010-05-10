# file:        Makefile.matlab
# description: Build MATALB toolbox
# author:      Andrea Vedaldi

# AUTORIGHTS

# MATLAB support is enabled if $(MEX) is executable and if MATLAB root
# can be set from the output of `$(MEX) -v`. Therefore setting MEX to
# the empty string disables MATLAB support.

MATLAB_EXE ?= matlab
MEX ?= mex
MATLAB_PATH ?= $(strip $(shell type -P $(MEX) 2>&1 >/dev/null && \
                 $(MEX) -v 2>&1 | sed -n 's/.*MATLAB *= *\(.*\)/\1/gp'))

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

MEX_FLAGS  = -$(MEX_ARCH)
MEX_FLAGS += $(if $(DEBUG), -g, -O)
MEX_FLAGS += $(if $(PROFILE), -O -g,)
MEX_FLAGS += -lm -largeArrayDims

MEX_CFLAGS   = $(CFLAGS)
MEX_CFLAGS  += -I$(VLDIR)/toolbox

MEX_LDFLAGS  = $(LDFLAGS) -L$(BINDIR) -lvl

MEX_SUFFIX  := mex$(ARCH)
MEX_BINDIR  := toolbox/mex/$(MEX_SUFFIX)

# Mac OS X on Intel 32 bit processor
ifeq ($(ARCH),maci)
endif

# Mac OS X on Intel 64 bit processor
ifeq ($(ARCH),maci64)
endif

# Linux on 32 bit processor
ifeq ($(ARCH),glx)
MEX_LDFLAGS += -Wl,--rpath,\\\$$ORIGIN/
endif

# Linux on 64 bit processorm
ifeq ($(ARCH),a64)
MEX_LDFLAGS += -Wl,--rpath,\\\$$ORIGIN/
MEX_ARCH = glxa64
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

ifdef MATLAB_PATH
arch_bins += $(mex_tgt) $(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)
comm_bins +=
deps += $(mex_dep)
endif

vpath vl_%.c $(shell find $(VLDIR)/toolbox -type d)

mex-all: $(mex_tgt)

# generate mex-dir target
$(eval $(call gendir, mex, $(MEX_BINDIR)))

$(MEX_BINDIR)/%.d : %.c $(mex-dir)
	$(call C,CC) $(MEX_CFLAGS) \
	       -I"$(MATLAB_PATH)/extern/include" -M -MT \
	       '$(MEX_BINDIR)/$*.$(MEX_SUFFIX) $(MEX_BINDIR)/$*.d' \
	       "$(<)" -MF "$(@)"

$(MEX_BINDIR)/%.$(MEX_SUFFIX) : %.c $(mex-dir)
	@make -s $(dll_tgt)
	@ln -sf "../../../$(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)" \
	        "$(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)"
	$(call C,MEX) \
               CFLAGS='$$CFLAGS  $(MEX_CFLAGS)'  \
	       LDFLAGS='$$LDFLAGS $(MEX_LDFLAGS)' \
	       $(MEX_FLAGS) \
	       "$(<)" -outdir "$(dir $(@))"

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
          $(subst vl_,,$(notdir $(m_src))))))))))))
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
