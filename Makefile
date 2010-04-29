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

NAME := vlfeat
VER := $(shell cat vl/generic.h | sed -n \
    's/.*VL_VERSION_STRING.*\(\([0-9][0-9]*\.\{0,1\}\)\{3\}\).*/\1/p')
CC ?= cc

# VLFeat root directory. This is used to have absolute paths to the
# source files in Xcode.
VLDIR ?= .

.PHONY : all
all:

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
#                                             Auto-detect architecture
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

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

CFLAGS += -std=c99
CFLAGS += -Wall -Wextra
CFLAGS += -Wno-unused-function -Wno-long-long -Wno-variadic-macros
CFLAGS += -DVL_ENABLE_THREADS
CFLAGS += -DVL_ENABLE_SSE2
CFLAGS += -I$(VLDIR)

CFLAGS += $(if $(DEBUG), -DVL_DEBUG -O0 -g, -DNDEBUG -O3)

# Architecture specific ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Mac OS X Intel 32
ifeq ($(ARCH),maci)
SDKROOT := /Developer/SDKs/MacOSX10.5.sdk
CFLAGS += -m32 -isysroot $(SDKROOT)
LDFLAGS += -lm -mmacosx-version-min=10.5
endif

# Mac OS X Intel 64
ifeq ($(ARCH),maci64)
SDKROOT := /Developer/SDKs/MacOSX10.5.sdk
CFLAGS += -m64 -isysroot $(SDKROOT)
LDFLAGS += -lm -mmacosx-version-min=10.5
endif

# Linux-32
ifeq ($(ARCH),glx)
CFLAGS  += -march=i686
LDFLAGS += -lm -Wl,--rpath,\$$ORIGIN/
endif

# Linux-64
ifeq ($(ARCH),a64)
LDFLAGS += -lm -Wl,--rpath,\$$ORIGIN/
endif

# --------------------------------------------------------------------
#                                                            Functions
# --------------------------------------------------------------------

# $(call if-like,FILTER,WHY,WHAT)
define if-like
$(if $(filter $(1),$(2)),$(3))
endef

# $(call dump-var,VAR) pretty-prints the content of a variable VAR on
# multiple columns
ifdef VERB
define dump-var
@echo $(1) =
@echo $($(1)) | sed 's/\([^ ][^ ]* [^ ][^ ]*\) */\1#/g' | \
tr '#' '\n' | column -t | sed 's/\(.*\)/  \1/g'
endef
else
define dump-var
@printf "%15s = %s\n" "$(1)" \
"$$(echo '$($(1))' | sed -e 's/[^ ][^ ]* /\.\.\./3' -e 's/\.\.\..*$$/\.\.\./g')"
endef
endif

# $(call echo-var,VAR) pretty-prints the content of a variable VAR on
# one line
define echo-var
@printf "%15s = %s\n" "$(1)" "$($(1))"
endef

# $(call print-command, CMD, TGT) prints a message
define print-command
@printf "%15s %s\n" "$(strip $(1))" "$(strip $(2))"
endef

# $(call C, CMD) runs $(CMD) silently
define C
@$(call print-command, $(1), "$(@)")
@quiet ()                                                            \
{                                                                    \
    local cmd out err ;					             \
    cmd="$($(1))";                                                   \
    out=$$($${cmd} "$${@}" 2>&1) ;                                   \
    err=$${?} ;                                                      \
    if test $${err} -gt 0 ; then                                     \
        echo "******* Offending Command:";                           \
        printf "%s" "$${cmd}" ;                                      \
	for i in "$${@}" ; do printf " '%s'" "$$i" ; done ;          \
	echo ;                                                       \
        echo "******* Error Code: $${err}";                          \
	echo "******* Command Output:";                              \
        echo "$${out}";                                              \
    fi;                                                              \
    echo "$${out}" | grep [Ww]arning ;                               \
    return $${err};                                                  \
} ; quiet
endef

# If verbose print everything
ifdef VERB
C = $($(1))
endif

# rule to create a directory
.PRECIOUS: %/.dirstamp
%/.dirstamp :
	@printf "%15s %s\n" MK "$(dir $@)"
	@mkdir -p $(dir $@)
	@echo "Directory generated by make." > $@

# $(call gendir, TARGET, DIR1 DIR2 ...) creates a target TARGET-dir that
# triggers the creation of the directories DIR1, DIR2
define gendir
$(1)-dir=$(foreach x,$(2),$(x)/.dirstamp)
endef

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

# Each Makefile submodule appends appropriate dependencies to the all,
# clean, archclean, distclean, and info
# targets. In addition, it appends to the deps and bins variables the
# list of .d files (to be inclued by make as auto-dependencies) and
# the list of files to be added to the binary distribution.

include make/dll.mak
include make/bin.mak
include make/matlab.mak
#include make/octave.mak
include make/doc.mak
include make/dist.mak

.PHONY: clean, archclean, distclean, info, autorights
no_dep_targets += clean archclean distclean info autorights

clean:
	rm -f  `find . -name '*~'`
	rm -f  `find . -name '.DS_Store'`
	rm -f  `find . -name '.gdb_history'`
	rm -f  `find . -name '._*'`
	rm -rf ./results

archclean: clean

distclean:

info :
	@echo "************************************* General settings"
	$(call echo-var,DEBUG)
	$(call echo-var,VER)
	$(call echo-var,ARCH)
	$(call echo-var,CFLAGS)
	$(call echo-var,LDFLAGS)
	$(call echo-var,CC)
	@printf "\nThere are %s lines of code.\n" \
	`cat $(m_src) $(mex_src) $(dll_src) $(dll_hdr) $(bin_src) | wc -l`

autorights: distclean
	autorights                                                   \
	  toolbox vl                                                 \
	  --recursive                                                \
	  --verbose                                                  \
	  --template docsrc/copylet.txt                              \
	  --years 2007-10                                            \
	  --authors "Andrea Vedaldi and Brian Fulkerson"             \
	  --holders "Andrea Vedaldi and Brian Fulkerson"             \
	  --program "VLFeat"

# --------------------------------------------------------------------
#                                                             Includes
# --------------------------------------------------------------------

.PRECIOUS: $(deps)

ifeq ($(filter $(no_dep_targets), $(MAKECMDGOALS)),)
-include $(deps)
endif
