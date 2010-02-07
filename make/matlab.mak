# file:        Makefile.matlab
# description: Build MATALB toolbox
# author:      Andrea Vedaldi

# AUTORIGHTS

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

MATLABEXE  ?= matlab
MEX ?= mex
MATLABPATH ?= $(strip $(shell $(MEX) -v 2>&1 | \
                sed -n 's/.*MATLAB *= *\(.*\)/\1/gp'))

ifdef MATLABPATH
all: mex-all matlab-all
info: mex-info matlab-info
clean: mex-clean matlab-clean
archclean: mex-archclean matlab-archclean
distclean: mex-distclean matlab-distclean
endif

MEX_FLAGS    = -$(ARCH)
MEX_FLAGS    = $(if $(DEBUG), -g, -O)
MEX_FLAGS   += -lm -largeArrayDims

MEX_CFLAGS   = $(CFLAGS)
MEX_CFLAGS  += -I$(VLDIR)/toolbox

MEX_LDFLAGS  = $(LDFLAGS) -L$(BINDIR) -lvl

MEX_SUFFIX  := mex$(ARCH)
MEX_BINDIR  := toolbox/$(MEX_SUFFIX)

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

# Linux on 64 bit processor
ifeq ($(ARCH),a64)
MEX_LDFLAGS += -Wl,--rpath,\\\$$ORIGIN/
endif

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

.PHONY: mex-all, mex-dir, mex-info, mex-test
.PHONY: mex-clean, mex-distclean, mex-archclean
no_dep_targets += mex-dir mex-info mex-test mex-clean mex-distclean mex-archclean

mex_src := $(shell find $(VLDIR)/toolbox -name "*.c")
mex_tgt := $(addprefix $(MEX_BINDIR)/,\
	   $(notdir $(mex_src:.c=.$(MEX_SUFFIX)) ) )
mex_dep := $(mex_tgt:.$(MEX_SUFFIX)=.d)

ifdef MATLABPATH
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
	       -I"$(MATLABPATH)/extern/include" -M -MT \
	       '$(MEX_BINDIR)/$*.$(MEX_SUFFIX) $(MEX_BINDIR)/$*.d' \
	       "$(<)" -MF "$(@)"

$(MEX_BINDIR)/%.$(MEX_SUFFIX) : %.c $(mex-dir)
	@make -s $(dll_tgt)
	@ln -sf "../../$(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)" \
	        "$(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)"
	$(call C,MEX) \
               CFLAGS='$$CFLAGS  $(MEX_CFLAGS)'  \
	       LDFLAGS='$$LDFLAGS $(MEX_LDFLAGS)' \
	       $(MEX_FLAGS) \
	       "$(<)" -outdir "$(dir $(@))"

mex-info:
	@echo "*************************************** MATLAB support"
	$(if $(MATLABPATH),\
	  @echo "MATLAB support enabled (MATLABPATH defined)",\
	  @echo "MATLAB support disabled (MATLABPATH undefined)")
	$(call dump-var,mex_src)
	$(call dump-var,mex_tgt)
	$(call dump-var,mex_dep)

mex-clean:
	rm -f $(mex_dep)

mex-archclean: mex-clean
	rm -rf $(MEX_BINDIR)

mex-distclean:
	for i in mac maci maci64 glx a64 w32 w64 ; \
	do \
	   rm -rf "toolbox/mex$${i}" ; \
	done

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

ifdef MATLABPATH
arch_bins +=
comm_bins += $(m_lnk)
deps +=
endif

matlab-all: $(matlab-noprefix)

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
	$(MATLABEXE) -$(ARCH) -nodesktop -r \
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
	$(call echo-var,MATLABPATH)
	$(call echo-var,MATLABEXE)
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
