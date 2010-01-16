# file:        Makefile
# author:      Andrea Vedaldi
# description: Build everything

# AUTORIGHTS

# This makefile builds VLFeat on modern UNIX boxes with the GNU
# compiler and make programs. Mac and Linux are explicitly supported,
# and many other boxes should be easy to add.
#
# This makefile builds three components: VLFeat shared library (DLL),
# the command line programs, and the MATLAB toolbox. It also builds
# the documentation (for the API, the MATLAB toolbox, the command line
# utility, and also VLFeat homepage).
#
# Configuring the build system entails setting the appropriate
# variables, which are summarized next. The makefile attempts to
# auto-detect the right configuration parameters depending on the
# architecture (see later).
#
# == MISCELLANEOUS VARIABLES ==
#
# VER:          Package version (e.g. 1.0)
# DIST:         Package name for the source distr. ('vlfeat-1.0')
# BINDIST:      Package name for the binary distr. ('vlfeat-1.0-bin')
# HOST:         Where to pulbish the package.
# NDEBUG:       Define this flag to remove debugging support (default undefined)
# VERB:         Define this flag to provide extra information (default NO)
#
# == PROGRAMS REQUIRED FOR BUILDING ==
#
# The following programs are required to compile the C library and the
# command line utilities:
#
# CC:           C compiler (e.g. gcc).
# LIBTOOL:      libtool (used only under Mac)
#
# The following programs are required to compile the C code in the
# MATLAB Toolbox. Both are bundled with MATLAB, but may not be
# available directly from the command line path.
#
# MATLAB:       Matlab executable (typically `matlab')
# MEX:          MEX compiler executable (typically `mex')
#
# The following programs are required to make the distribution
# packages:
#
# GIT:          Version system (you also need the full GIT repository).
#
# == BUILDING ===
#
# You can use CFLAGS and LDFLAGS to pass additional flags to the C
# compiler and linker. These are in turn passed to the various flags
# defined below.
#
# == BUILDING THE SHARED LIBRARY ==
#
# DLL_CLFAGS:   Aflags passed to $(CC) to compile a DLL C source
# DLL_SUFFIX:   suffix of a DLL (.so, .dylib)
#
# == BUILDING THE COMMAND LINE UTILITIES ==
#
# BINDIR:       where to put the exec (and libraries)
# C_CLFAGS:     flags passed to $(CC) to compile a C source
# C_LDFLAGS:    flags passed to $(CC) to link C objects into an exec
#
# == BUILDING THE MEX FILES ==
#
# MATLABPATH:   MATALB root path
# MEX_BINDIR:   where to put mex files
# MEX_SUFFIX:   suffix of a MEX file (.mexglx, .mexmac, ...)
# MEX_FLAGS:    flags passed to $(MEX)
# MEX_CFLAGS:   flags added to the CLFAGS variable of $(MEX)
# MEX_LDFLAGS:  flags added to the LDFLAGS variable of $(MEX)
#
# == BUILDING THE DOCUMENTATION ==
#
# There are no configuration parameters.
#
# == ABOUT DEBUGGING ==
#
# We use the older STABS+ debug format as MATLAB MEX command has buggy
# support for DWARD-2. In particular, compiling a MEX file produces a
# DLL, but removes the intermediate object files, where DWARF-2 stores
# the actual debugging information. Since MEX uses the default
# debugging format (-g option) and this corresponds often to DWARF-2,
# this should be considered a bug.
#
# As a workaround, we pass the -gstabs+ flag to MEX
#
# An alternative workaround is the following hack.
# Fist we compile the MEX file without linking and then we linking in a
# second pass, effectively preserving an intermediate object file
# (but not all of them).  E.g., on Mac OS X
#
# > mex -c -g mymex.c ; mex -g mymex.o ;
#
# does preserve debugging information. In fact is also possible to run
#
# > dsymutil mymex.mexmaci
#
# does produce a mymex.mexmaci.dSYM bundle with (almost complete)
# debugging information.

NAME   ?= vlfeat
VER    := $(shell cat vl/generic.h | sed -n \
	's/.*VL_VERSION_STRING.*\(\([0-9][0-9]*\.\{0,1\}\)\{3\}\).*/\1/p')
HOST   := ganesh.cs.ucla.edu:/var/www/vlfeat.org

# programs required to build VLFeat
CC         ?= cc
MATLABEXE  ?= matlab
MEX        ?= mex
LIBTOOL    ?= libtool

# programs required to build VLFeat distribution
GIT        ?= git

# VLFeat root directory. Useful to use absolute paths in Xcode.
VLDIR      ?= .

.PHONY : all
all : dll all-bin

# --------------------------------------------------------------------
#                                                       Error Messages
# --------------------------------------------------------------------

err_no_arch  =
err_no_arch +=$(shell echo "** Unknown host architecture '$(UNAME)'. This identifier"   1>&2)
err_no_arch +=$(shell echo "** was obtained by running 'uname -sm'. Edit the Makefile " 1>&2)
err_no_arch +=$(shell echo "** to add the appropriate configuration."                   1>&2)
err_no_arch +=config

err_internal  =$(shell echo Internal error)
err_internal +=internal

err_spaces  = $(shell echo "** VLFeat root dir VLDIR='$(VLDIR)' contains spaces."  1>&2)
err_spaces += $(shell echo "** This is not supported due to GNU Make limitations." 1>&2)
err_spaces +=spaces

# --------------------------------------------------------------------
#                                     Auto-detect architecture, MATLAB
# --------------------------------------------------------------------

Darwin_PPC_ARCH             := mac
Darwin_Power_Macintosh_ARCH := mac
Darwin_i386_ARCH            := maci
Linux_i386_ARCH             := glx
Linux_i686_ARCH             := glx
Linux_unknown_ARCH          := glx
Linux_x86_64_ARCH           := a64

UNAME := $(shell uname -sm)
ARCH  ?= $($(shell echo "$(UNAME)" | tr \  _)_ARCH)

# sanity check
ifeq ($(ARCH),)
die:=$(error $(err_no_arch))
endif

ifneq ($(VLDIR),$(shell echo "$(VLDIR)" | sed 's/ //g'))
die:=$(error $(err_spaces))
endif

MATLABPATH := $(strip $(shell $(MEX) -v 2>&1 |                       \
                sed -n 's/.*MATLAB *= *\(.*\)/\1/gp'))

# --------------------------------------------------------------------
#                                                            Functions
# --------------------------------------------------------------------

# $(call if-like,FILTER,WHY,WHAT)
define if-like
$(if $(filter $(1),$(2)),$(3))
endef

# $(call dump-var,VAR) pretty-prints the content of a variable VAR
define dump-var
@echo $(1) =
@echo $($(1)) | sed 's/\([^ ][^ ]* [^ ][^ ]*\) */\1#/g' | \
tr '#' '\n' | column -t | sed 's/\(.*\)/  \1/g'
endef

# $(call print-command, CMD, TGT) prints a message
define print-command
@printf "%10s %s\n" "$(strip $(1))" "$(strip $(2))"
endef

# $(call C, CMD) runs $(CMD) silently
define C
@$(call print-command, $(1), "$(@)")
@quiet ()                                                            \
{                                                                    \
    local cmd out err ;					             \
    cmd="$($(1))";                                                   \
    out=$$($${cmd} "$${@}" 2>&1);                                    \
    err=$${?};                                                       \
    if test $${err} -gt 0 ; then                                     \
        echo "******* Offending Command:";                           \
        echo "$${cmd}" "$${@}";                                      \
        echo "******* Error Code: $${err}";                          \
	echo "******* Command Output:";                              \
        echo "$${out}";                                              \
    fi;                                                              \
    echo "$${out}" | grep arning ;                                   \
    return $${err};                                                  \
} ; quiet
endef

# If verbose print everything
ifdef VERB
C = $($(1))
endif

# --------------------------------------------------------------------
#                                            Common UNIX Configuration
# --------------------------------------------------------------------

ifndef NDEBUG
DEBUG := yes
endif

C_CFLAGS     = $(CFLAGS)
C_CFLAGS    += -I$(VLDIR)
C_CFLAGS    += -pedantic -std=c99 -O3
C_CFLAGS    += -Wall -Wno-unused-function -Wno-long-long -Wno-variadic-macros
C_CFLAGS    += $(if $(DEBUG), -O0 -g)

C_LDFLAGS    = $(LDFLAGS)
C_LDFLAGS   += -L$(BINDIR) -l$(DLL_NAME)

DLL_NAME     = vl
DLL_CFLAGS   = $(C_CFLAGS) -fvisibility=hidden -fPIC -DVL_BUILD_DLL

MEX_FLAGS    = $(if $(DEBUG), -g)
MEX_CFLAGS   = $(CFLAGS) -I$(VLDIR) -I$(VLDIR)/toolbox
MEX_CFLAGS  += -Wall -Wno-unused-function -Wno-long-long -Wno-variadic-macros
MEX_LDFLAGS  = -L$(BINDIR) -l$(DLL_NAME)

ifdef MATLABPATH
all: all-mex
endif

# --------------------------------------------------------------------
#                                  Architecture-specific Configuration
# --------------------------------------------------------------------

# Mac OS X on PPC processor
ifeq ($(ARCH),mac)
BINDIR          := $(VLDIR)/bin/mac
DLL_SUFFIX      := dylib
MEX_SUFFIX      := mexmac
C_CFLAGS        += $(if $(DEBUG), -gstabs+)
C_LDFLAGS       += -lm
DLL_CFLAGS      += -fvisibility=hidden
MEX_FLAGS       += -lm CC='gcc' CXX='g++' LD='gcc'
MEX_CFLAGS      +=
MEX_LDFLAGS     +=
endif

# Mac OS X on Intel processor
ifeq ($(ARCH),maci)
BINDIR          := $(VLDIR)/bin/maci
DLL_SUFFIX      := dylib
MEX_SUFFIX      := mexmaci
SDKROOT         := /Developer/SDKs/MacOSX10.5.sdk
C_CFLAGS        += -DVL_SUPPORT_SSE2
C_CFLAGS        += -m32
C_CFLAGS        += -isysroot $(SDKROOT)
C_CFLAGS        += $(call if-like,%_sse2,$*,-msse2)
CFLAGS          += $(if $(DEBUG), -gstabs+)
LDFLAGS         += -lm -mmacosx-version-min=10.5
MEX_FLAGS       += -lm -maci
MEX_CFLAGS      +=
MEX_LDFLAGS     +=
endif

# Mac OS X on Intel 64 processor
ifeq ($(ARCH),maci64)
BINDIR          := $(VLDIR)/bin/maci64
DLL_SUFFIX      := dylib
MEX_SUFFIX      := mexmaci64
SDKROOT         := /Developer/SDKs/MacOSX10.5.sdk
C_CFLAGS        += -DVL_SUPPORT_SSE2
C_CFLAGS        += -m64
C_CFLAGS        += -isysroot $(SDKROOT)
C_CFLAGS        += $(call if-like,%_sse2,$*,-msse2)
CFLAGS          += $(if $(DEBUG), -gstabs+)
LDFLAGS         += -lm -mmacosx-version-min=10.5
MEX_FLAGS       += -lm -maci64
MEX_CFLAGS      +=
MEX_LDFLAGS     +=
endif

# Linux-32
ifeq ($(ARCH),glx)
BINDIR          := $(VLDIR)/bin/glx
MEX_SUFFIX      := mexglx
DLL_SUFFIX      := so
C_CFLAGS        += -D__LITTLE_ENDIAN__ -std=c99
C_CFLAGS        += -DVL_SUPPORT_SSE2
C_CFLAGS        += -march=i686
C_CFLAGS        += $(call if-like,%_sse2,$*,-msse2)
LDFLAGS         += -lm -Wl,--rpath,\$$ORIGIN/
MEX_FLAGS       += -lm
MEX_CFLAGS      +=
MEX_LDFLAGS     += -Wl,--rpath,\\\$$ORIGIN/
endif

# Linux-64
ifeq ($(ARCH),a64)
BINDIR          := $(VLDIR)/bin/a64
MEX_SUFFIX      := mexa64
DLL_SUFFIX      := so
C_CFLAGS        += -D__LITTLE_ENDIAN__ -std=c99
C_CFLAGS        += -DVL_SUPPORT_SSE2
C_CFLAGS        += $(call if-like,%_sse2,$*,-msse2)
LDFLAGS         += -lm -Wl,--rpath,\$$ORIGIN/
MEX_FLAGS       += -lm -largeArrayDims
MEX_CFLAGS      +=
MEX_LDFLAGS     += -Wl,--rpath,\\\$$ORIGIN/
endif

DIST            := $(NAME)-$(VER)
BINDIST         := $(DIST)-bin
MEX_BINDIR      := $(VLDIR)/toolbox/$(MEX_SUFFIX)

# Sanity check
ifeq ($(DLL_SUFFIX),)
die:=$(error $(err_internal))
endif

ifdef VERB
$(info * *********************************************************** )
$(info * Version:    $(VER)                                          )
$(info * Auto arch:  $(ARCH)                                         )
$(info * Debug mode: $(if $(DEBUG),yes,no)                           )
$(info * CFLAGS:     $(CFLAGS)                                       )
$(info * C_CFLAGS:   $(C_CFLAGS)                                     )
ifdef MATLABPATH
$(info * MATLABPATH: $(MATLABPATH)                                   )
$(info * MEX_SUFFIX: $(MEX_SUFFIX)                                   )
$(info * MEX_CFLAGS: $(MEX_CFLAGS)                                   )
else
$(info * MATLAB not found. Will not compile MEX files.               )
endif
$(info * *********************************************************** )
endif

# --------------------------------------------------------------------
#                                                     Make directories
# --------------------------------------------------------------------

.PRECIOUS: %/.dirstamp
%/.dirstamp :
	@printf "%10s %s\n" MK "$(dir $@)"
	@mkdir -p $(dir $@)
	@echo "Directory generated by make." > $@

define gendir
$(1)-dir=$(foreach x,$(2),$(x)/.dirstamp)
endef

$(eval $(call gendir, doc,     doc doc/demo doc/figures doc/man-src ))
$(eval $(call gendir, results, results                              ))
$(eval $(call gendir, bin,     $(BINDIR) $(BINDIR)/objs             ))
$(eval $(call gendir, mex,     $(MEX_BINDIR)                        ))
$(eval $(call gendir, noprefix, toolbox/noprefix                    ))

# --------------------------------------------------------------------
#                                             Build the shared library
# --------------------------------------------------------------------
#
# Objects and dependency files are placed in the $(BINDIR)/objs/
# directory.
#
# MAC/MACI: The library install_name is prefixed with @loader_path/.
#   At run time this causes the loader to search for a local copy of
#   the library for any binary which is linked against it. The
#   install_name can be modified later by install_name_tool.
#
# LINUX:

dll_tgt := $(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)
dll_lnk := $(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)
dll_src := $(wildcard $(VLDIR)/vl/*.c)
dll_hdr := $(wildcard $(VLDIR)/vl/*.h) $(wildcard $(VLDIR)/vl/*.tc)
dll_obj := $(addprefix $(BINDIR)/objs/, $(notdir $(dll_src:.c=.o)))
dll_dep := $(dll_obj:.o=.d)

.PHONY: dll
dll: $(dll_tgt)

.PRECIOUS: $(BINDIR)/objs/%.d

$(BINDIR)/objs/%.o : vl/%.c $(bin-dir)
	$(call C,CC) $(DLL_CFLAGS) -c "$(<)" -o "$(@)"

$(BINDIR)/objs/%.d : vl/%.c $(bin-dir)
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

# --------------------------------------------------------------------
#                                         Build command line utilities
# --------------------------------------------------------------------
# The executables are stored in $(BINDIR). $(dll_tgt) is not a direct
# dependency in order avoiding rebuilding all the executables if only
# a part of the library is changed.

bin_src := $(wildcard $(VLDIR)/src/*.c)
bin_tgt := $(notdir $(bin_src))
bin_tgt := $(addprefix $(BINDIR)/, $(bin_tgt:.c=))
bin_dep := $(addsuffix .d, $(bin_tgt))

.PHONY: all-bin
all-bin: $(bin_tgt)

$(BINDIR)/% : src/%.c $(bin-dir)
	@make -s $(dll_tgt)
	$(call C,CC) $(C_CFLAGS) $< $(C_LDFLAGS) -o $@

$(BINDIR)/%.d : src/%.c $(bin-dir)
	$(call C,CC) $(C_CFLAGS) -M -MT                              \
	       '$(BINDIR)/$* $(MEX_BINDIR)/$*.d'                     \
	       $< -MF $@

# --------------------------------------------------------------------
#                                                      Build MEX files
# --------------------------------------------------------------------
# MEX files are placed in toolbox/$(MEX_SUFFIX). MEX files are linked
# so that they search for the dynamic libvl in the directory where
# they are found. A link is automatically created to the library
# binary file.
#
# On Linux, this is obtained by setting -rpath to $ORIGIN/ for each
# MEX file. On Mac OS X this is implicitly obtained since libvl.dylib
# has install_name relative to @loader_path/.

mex_src := $(shell find $(VLDIR)/toolbox -name "*.c")
mex_tgt := $(addprefix $(MEX_BINDIR)/,                               \
	   $(notdir $(mex_src:.c=.$(MEX_SUFFIX)) ) )
mex_dep := $(mex_tgt:.$(MEX_SUFFIX)=.d)

vpath %.c $(shell find $(VLDIR)/toolbox -type d)
vpath vl_%.m $(shell find $(VLDIR)/toolbox -type d)

.PHONY: all-mex
all-mex : $(mex_tgt) noprefix

$(MEX_BINDIR)/%.d : %.c $(mex-dir)
	$(call C,CC) $(MEX_CFLAGS)                                   \
	       -I"$(MATLABPATH)/extern/include" -M -MT               \
	       '$(MEX_BINDIR)/$*.$(MEX_SUFFIX) $(MEX_BINDIR)/$*.d'   \
	       "$(<)" -MF "$(@)"

$(MEX_BINDIR)/%.$(MEX_SUFFIX) : %.c $(mex-dir) #$(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)
	@make -s $(dll_tgt)
	@ln -sf "../../$(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)"       \
	        "$(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)"
	$(call C,MEX) CFLAGS='$$CFLAGS  $(MEX_CFLAGS)'               \
	       LDFLAGS='$$LDFLAGS $(MEX_LDFLAGS)'                    \
	       $(MEX_FLAGS)                                          \
	       "$(<)" -outdir "$(dir $(@))"

# --------------------------------------------------------------------
#                                          Prefix-less M and MEX files
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

toolbox/noprefix/%.m : vl_%.m
	@upperName=`echo "$*" | tr [a-z]  [A-Z]` ;                   \
	echo "function varargout = $*(varargin)" > "$@" ;            \
	cat "$<" | sed -n -e '/^function/b' -e '/^%.*$$/p'           \
             -e '/^%.*$$/b' -e q >> "$@" ;                           \
	echo "[varargout{1:nargout}] = vl_$*(varargin{:});" >> "$@" ;

.PHONY: noprefix
noprefix: $(noprefix-dir) $(m_lnk)

# --------------------------------------------------------------------
#                                                           Make clean
# --------------------------------------------------------------------

.PHONY: clean archclean distclean

clean:
	rm -f  `find . -name '*~'`
	rm -f  `find . -name '.DS_Store'`
	rm -f  `find . -name '.gdb_history'`
	rm -f  `find . -name '._*'`
	rm -rf `find ./bin -name 'objs' -type d`
	rm -rf  ./results
	rm -rf tmp-$(NAME)-*
	rm -f $(dll_dep) $(bin_dep) $(mex_dep)

archclean: clean
	rm -rf bin/$(ARCH)
	rm -rf toolbox/$(ARCH)
	rm -rf tmp-$(NAME)-*$(ARCH)*

distclean: clean doc-distclean
	rm -rf bin
	for i in mexmac mexmaci mexmaci64 mexglx                     \
                 mexw32 mexa64 mexw64 ;                              \
	do                                                           \
	   rm -rf "toolbox/$${i}" ;                                  \
	done
	rm -f $(NAME)-*.tar.gz
	rm -rf toolbox/noprefix

# --------------------------------------------------------------------
#                                                            Packaging
# --------------------------------------------------------------------

# bin-release: Rebuild binaries with optimizations and no debug symbols
# bin-commit:  Creates a new vXX.XX.XX-ARCH branch with the binaries
#              and pushes it to the remote called bin
# bin-merge:   Creates a new vXX.XX.XX-bin branch by merging
#              the architecture specific binary branches and
#              adding additional files. It then pushes the result
#              to the remote called bin.
# bin-dist:    Packs the commit remotes/bin/vXX.XX.XX-bin.
# src-dist:    Packs the commit vXX.XX.XX.

.PHONY: $(NAME), dist, bindist
.PHONY: post, post-doc
.PHONY: bin-release, bin-commit, bin-dist, src-dist

bin-release:
	set -e ; \
	TMP=tmp-$(NAME)-$(VER)-$(ARCH) ; \
	\
	echo Fetching remote tags ; \
	$(GIT) fetch --tags ; \
	echo Cloning VLFeat ; \
	test -e $$TMP || $(GIT) clone --no-checkout . $$TMP ; \
	$(GIT) --git-dir=$$TMP/.git config remote.bin.url $$($(GIT) config --get remote.bin.url) ; \
	$(GIT) --git-dir=$$TMP/.git config remote.origin.url $$($(GIT) config --get remote.origin.url) ; \
	\
	echo Checking out v$(VER) ; \
	cd $$TMP ; \
	$(GIT) fetch origin ; \
	$(GIT) checkout v$(VER) ; \
	echo Rebuilding binaries for release ; \
	make NDEBUG=yes ARCH=$(ARCH) all

bin-commit: bin-release
	@set -e ; \
	TMP=tmp-$(NAME)-$(VER)-$(ARCH) ; \
	BRANCH=v$(VER)-$(ARCH)  ; \
	\
	cd $$TMP ; \
	echo Setting up $$BRANCH to v$(VER) ; \
	$(GIT) branch -f $$BRANCH v$(VER) ; \
	$(GIT) checkout $$BRANCH ; \
	echo Adding binaries to $$BRANCH ; \
	$(GIT) add -f $(dll_tgt) $(dll_lnk) $(bin_tgt) $(mex_tgt) ; \
	if test -z "$$($(GIT) diff --cached)" ; \
	then \
	  echo No changes to commit ; \
	  exit 1 ; \
	fi ; \
	echo Commiting changes ; \
	$(GIT) commit -m "$(ARCH) binaries for version $(VER)" ; \
	echo Pushing $$BRANCH to the server ; \
	$(GIT) push -v --force bin $$BRANCH:refs/heads/$$BRANCH ;

bin-commit-common: bin-commit
	@set -e ; \
	TMP=tmp-$(NAME)-$(VER)-$(ARCH) ; \
	BRANCH=v$(VER)-common  ; \
	\
	cd $$TMP ; \
	echo Setting up $$BRANCH to v$(VER) ; \
	$(GIT) branch -f $$BRANCH v$(VER) ; \
	$(GIT) checkout $$BRANCH ; \
	echo Adding binaries to $$BRANCH ; \
	$(GIT) add -f $(m_lnk) ; \
	if test -z "$$($(GIT) diff --cached)" ; \
	then \
	  echo No changes to commit ; \
	  exit 1 ; \
	fi ; \
	echo Commiting changes ; \
	$(GIT) commit -m "common products for $(VER)" ; \
	echo Pushing $$BRANCH to the server ; \
	$(GIT) push -v --force bin $$BRANCH:refs/heads/$$BRANCH ;

bin-merge:
	set -e ; \
	BRANCH=v$(VER)-bin ; \
	TMP=tmp-$(NAME)-$(VER)-bin ; \
	\
	echo Fetching remote tags ; \
	$(GIT) fetch --tags ; \
	echo Cloning VLFeat ; \
	test -e $$TMP || $(GIT) clone --no-checkout . $$TMP ; \
	$(GIT) --git-dir=$$TMP/.git config remote.bin.url $$($(GIT) config --get remote.bin.url) ; \
	$(GIT) --git-dir=$$TMP/.git config remote.origin.url $$($(GIT) config --get remote.origin.url) ; \
	\
	cd $$TMP ; \
	echo Creating or resetting and checking out branch $$BRANCH to v$(VER); \
	$(GIT) fetch origin ; \
	$(GIT) checkout v$(VER) ; \
	$(GIT) branch -f $$BRANCH v$(VER) ; \
	$(GIT) checkout $$BRANCH ; \
	MERGE_BRANCHES=; \
	FETCH_BRANCHES=; \
	for ALT_ARCH in common maci64 maci glx a64 w32 w64 ; \
	do \
	  MERGE_BRANCH=v$(VER)-$$ALT_ARCH ; \
	  MERGE_BRANCHES="$$MERGE_BRANCHES bin/$$MERGE_BRANCH" ; \
	  FETCH_BRANCHES="$$FETCH_BRANCHES $$MERGE_BRANCH:remotes/bin/$$MERGE_BRANCH" ; \
	done ; \
	echo Fetching binaries ; \
	echo $(GIT) fetch -f bin $$FETCH_BRANCHES ; \
	$(GIT) fetch -f bin $$FETCH_BRANCHES ; \
	echo merging $$MERGE_BRANCHES ; \
	$(GIT) merge -m "Merged binaries $$MERGE_BRANCHES" $$MERGE_BRANCHES ; \
	echo Pushing to server the merged binaries ; \
	$(GIT) push -v --force bin $$BRANCH:refs/heads/$$BRANCH ;

src-dist:
	COPYFILE_DISABLE=1                                           \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1                           \
	$(GIT) archive --prefix=$(NAME)/ v$(VER) | gzip > $(DIST).tar.gz

bin-dist:
	echo Fetching binaries ; \
	BRANCH=v$(VER)-bin ; \
	git fetch -f bin $$BRANCH:remotes/bin/$$BRANCH ; \
	echo Creating archive ; \
	$(GIT) archive --prefix=$(NAME)/ bin/v$(VER)-bin | gzip > $(BINDIST).tar.gz

post:
	rsync -aP $(DIST).tar.gz $(BINDIST).tar.gz                   \
	    $(HOST)/download

post-doc: doc
	rsync --recursive                                            \
	      --perms                                                \
	      --group=lab                                            \
	      --chmod=Dg+s,g+w,o-w                                   \
	      --exclude=*.eps                                        \
	      --progress                                             \
	      --exclude=download                                     \
	      --exclude=man-src                                      \
	      --exclude=toolbox-src                                  \
	      --exclude=.htaccess                                    \
	      --exclude=favicon.ico                                  \
	      --delete                                               \
	      doc/ $(HOST)

# --------------------------------------------------------------------
#                                                             Includes
# --------------------------------------------------------------------

# Auto-deps
ifeq ($(filter doc clean archclean distclean info \
               bin-release bin-commit bin-merge bin-dist, $(MAKECMDGOALS)),)
-include $(dll_dep) $(bin_dep)
ifdef MATLABPATH
-include $(mex_dep)
endif
endif

# Makefile for documentation
include Makefile.doc

# --------------------------------------------------------------------
#                                                       Debug Makefile
# --------------------------------------------------------------------

.PHONY: info
info :
	$(call dump-var,dll_hdr)
	$(call dump-var,dll_src)
	$(call dump-var,dll_obj)
	$(call dump-var,dll_dep)
	$(call dump-var,mex_src)
	$(call dump-var,mex_dep)
	$(call dump-var,mex_tgt)
	$(call dump-var,m_src)
	$(call dump-var,demo_src)
	$(call dump-var,bin_src)
	$(call dump-var,bin_tgt)
	$(call dump-var,bin_dep)
	@echo "VER          = $(VER)"
	@echo "ARCH         = $(ARCH)"
	@echo "DIST         = $(DIST)"
	@echo "BINDIST      = $(BINDIST)"
	@echo "MEX_BINDIR   = $(MEX_BINDIR)"
	@echo "DLL_SUFFIX   = $(DLL_SUFFIX)"
	@echo "MEX_SUFFIX   = $(MEX_SUFFIX)"
	@echo "CFLAGS       = $(CFLAGS)"
	@echo "LDFLAGS      = $(LDFLAGS)"
	@echo "C_CFLAGS     = $(C_CFLAGS)"
	@echo "C_LDFLAGS    = $(C_LDFLAGS)"
	@echo "DLL_NAME     = $(DLL_NAME)"
	@echo "DLL_CFLAGS   = $(DLL_CFLAGS)"
	@echo "MATLABEXE    = $(MATLABEXE)"
	@echo "MEX          = $(MEX)"
	@echo "MATLABPATH   = $(MATLABPATH)"
	@echo "MEX_FLAGS    = $(MEX_FLAGS)"
	@echo "MEX_CFLAGS   = $(MEX_CFLAGS)"
	@echo "MEX_LDFLAGS  = $(MEX_LDFLAGS)"
	@printf "\nThere are %s lines of code.\n" \
	`cat $(m_src) $(mex_src) $(dll_src) $(dll_hdr) $(bin_src) | wc -l`

# --------------------------------------------------------------------
#                                                       X-Code Support
# --------------------------------------------------------------------

.PHONY: dox-
dox- : dox

.PHONY: dox-clean
dox-clean:
