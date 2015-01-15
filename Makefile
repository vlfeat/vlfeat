# file: Makefile
# description: Build everything
# author: Andrea Vedaldi

# Copyright (C) 2014 Andrea Vedaldi.
# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

# VLFEAT BUILDING INSTRUCTIONS
#
# This makefile builds VLFeat on standard Unix installations with the
# GNU toolchain. Mac OS X and GNU-Linux are explicitly
# supported. Usually, compiling VLFeat reduces to typing
#
# > cd PATH_TO_VLFEAT_SOURCE_TREE
# > make
#
# The makefile attempts to automatically determine the host
# architecture. If this fails, or if the architecture is ambiguous,
# the architecture can be set by specifying the ARCH variable. For
# instance:
#
# > make ARCH=maci64
#
# builds VLFeat for Mac OS X Intel 64 bit. Pease
# see http://www.vlfeat.org/compiling.html
# for troubleshooting and details.
#
# Other useful variables are listed below (their default value is in
# square brackets).
#
#   ARCH [not defined] - Active architecture. The supported
#       architectures are maci, maci64, glnx86, or glnxa64 (these are
#       the same architecture identifiers used by MATLAB:
#       http://www.mathworks.com/help/techdoc/ref/computer.html). If
#       undefined, the makefile attempts to automatically detect the
#       architecture.
#
#   DEBUG [not defined] - If defined, turns on debugging symbols and
#       turns off optimizations
#
#   PROFILE [not defined] - If defined, turns on debugging symbols but
#       does NOT turn off optimizations.
#
#   VERB [not defined] - If defined, display in full the command
#       executed and their output.
#
#   MEX [mex]- Path to MATLAB MEX compiler. If undefined, MATLAB support
#       is disabled.
#
#   MKOCTFILE [not defined] - Path to Octave MKOCTFILE compiler. If undefined,
#       Octave support is disabled.
#
# If defined to anything other than "no", the following falgs disable
# specific features in the library. By defaults, all the features are
# enabled.  If the makefile finds that the environment is unable to
# support some of them, it may decide to disable them automatically
# (in this case it will print a message).  This behaviour can be
# overriden by defining the flag to be "no".
#
#   DISABLE_SSE2 - SSE2 vector instructions support.
#   DISABLE_AVX - AVX vector instructions support.
#   DISABLE_THREADS - Supprot for multithreded library client.
#   DISABLE_OPENMP - OpenMP-based multithreaded computations.
#
# To completely remove all build products use
#
# > make distclean
#
# Other useful targets include:
#
#   clean - Removes intermediate build products for the active architecture.
#   archclean - Removes all build products for the active architecture.
#   distclean - Removes all build products.
#   info - Display a list of the variables defined by the Makefile.
#   help - Print this message.
#
# VLFeat is compsed of different parts (DLL, command line utilities,
# MATLAB interface, Octave interface) so the makefile is divided in
# components, located in make/*.mak. Please check out the
# corresponding files in order to adjust parameters.

# Copyright (C) 2014 Andrea Vedaldi.
# Copyright (C) 2007-13 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

SHELL = /bin/bash

.PHONY : all
all:

# Select which features to disable
# DISABLE_SSE2=yes
# DISABLE_AVX=yes
# DISABLE_THREADS=yes
# DISABLE_OPENMP=yes

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

Darwin_PPC_ARCH := mac
Darwin_Power_Macintosh_ARCH := mac
Darwin_i386_ARCH := maci64
Darwin_x86_64_ARCH := maci64
Linux_i386_ARCH := glnx86
Linux_i686_ARCH := glnx86
Linux_unknown_ARCH := glnx86
Linux_x86_64_ARCH := glnxa64

UNAME := $(shell uname -sm)
ARCH ?= $($(shell echo "$(UNAME)" | tr \  _)_ARCH)

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

VLDIR ?= .
LIBTOOL ?= libtool

STD_CLFAGS = $(CFLAGS)
STD_CFLAGS += -std=c99
STD_CFLAGS += -Wall -Wextra
STD_CFLAGS += -Wno-unused-function -Wno-long-long -Wno-variadic-macros
STD_CFLAGS += $(if $(DEBUG), -DDEBUG -O0 -g, -DNDEBUG -O3)
STD_CFLAGS += $(if $(PROFILE), -g)

STD_LDFLAGS = $(LDFLAGS)

# Architecture specific ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Detect compiler
COMPILER_VER_STRING:=$(shell $(CC) --version) $(shell $(CC) -v 2>&1)
COMPILER:=other

ifneq ($(shell echo "$(COMPILER_VER_STRING)" | grep "gcc"),)
COMPILER:=gcc
COMPILER_VER:=$(shell \
$(CC) -dumpversion | \
sed -e 's/\.\([0-9][0-9]\)/\1/g' \
    -e 's/\.\([0-9]\)/0\1/g' \
    -e 's/^[0-9]\{3,4\}$$/&00/' )
endif

ifeq "$(findstring clang,$(COMPILER_VER_STRING))" "clang"
COMPILER:=clang
COMPILER_VER:=$(shell \
echo "$(COMPILER_VER_STRING)" | \
sed -n -e 's/.*version *\([0-9.][0-9.]*\).*/\1/p;' | \
sed -e 's/\.\([0-9][0-9]\)/\1/g' \
    -e 's/\.\([0-9]\)/0\1/g' \
    -e 's/^[0-9]\{3,4\}$$/&00/' )
endif

$(info Detected compiler: $(COMPILER) $(COMPILER_VER))
ifeq "$(COMPILER_VER)" "other"
$(warning Unsupported compiler detected, use at your own risk!)
endif

ifeq "$(COMPILER)" "gcc"
ifeq "$(shell expr $(COMPILER_VER) \<= 40600)" "1"
ifneq "$(DISABLE_AVX)" "no"
$(info GCC <= 4.6.0 detected, disabling AVX.)
DISABLE_AVX:=yes
endif
endif
endif

ifeq "$(COMPILER)" "clang"
ifneq "$(DISABLE_OPENMP)" "no"
$(info Clang does not support OpenMP yet, disabling.)
DISABLE_OPENMP:=yes
endif
endif

# Mac OS X Intel
ifeq "$(ARCH)" "$(filter $(ARCH),maci maci64)"
ifeq "$(ARCH)" "maci"
march=32
else
march=64
endif
SDKROOT ?= $(shell xcrun -sdk macosx --show-sdk-path)
MACOSX_DEPLOYMENT_TARGET ?= 10.4
STD_CFLAGS += -m$(march) -isysroot $(SDKROOT) -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
STD_LDFLAGS += -Wl,-syslibroot,$(SDKROOT) -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
endif

# Linux
ifeq "$(ARCH)" "$(filter $(ARCH),glnx86 glnxa64)"
ifeq "$(ARCH)" "glnx86"
march=32
else
march=64
endif
# Target compatibility with GLIBC 2.3.4
# 1) _GNU_SOURCE avoids using isoc99_fscanf, limiting binary portability to recent GLIBC.
# 2) -fno-stack-protector avoids using a feature requiring GLBIC 2.4
STD_CFLAGS += -m$(march) -D_GNU_SOURCE -fno-stack-protector
STD_LDFLAGS += -m$(march) -Wl,--rpath,\$$ORIGIN/ -Wl,--as-needed
endif

# Convert back DISALBE_*="no" flags to be empty
ifeq "$(DISABLE_SSE2)" "no"
override DISABLE_SSE2:=
endif
ifeq "$(DISABLE_AVX)" "no"
override DISABLE_AVX:=
endif
ifeq "$(DISABLE_THREADS)" "no"
override DISABLE_THREADS:=
endif
ifeq "$(DISABLE_OPENMP)" "no"
override DISABLE_OPENMP:=
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
@echo "$(1) ="
@echo "$($(1))" | sed 's/\([^ ][^ ]* [^ ][^ ]*\) */\1#/g' | \
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
@printf "%15s = %s\n" '$(1)' '$($(1))'
endef

# $(call echo-title,TITLE) pretty-prints TITLE as a title
define echo-title
@printf "** %s\n" '$(1)'
endef

# $(call C, CMD) runs $(CMD) silently
define C
@printf "%15s %s\n" '$(1)' '$(@)'
$(Q)"$($(1))"
endef

# If verbose print everything
ifdef VERB
Q=
else
Q=@
endif

# Greater or equal: returns the empty string if $1 >= $2, otherwise returns 'false',
# where the arguments are integer numbers
gt = $(shell if [ "$(1)" -lt "$(2)" ] ; then echo false ; fi)

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
# clean, archclean, distclean, and info targets. In addition, it
# appends to the deps and bins variables the list of .d files (to be
# inclued by make as auto-dependencies) and the list of files to be
# added to the binary distribution.

.PHONY: clean, archclean, distclean, info, help
no_dep_targets := clean archclean distclean info help

include make/dll.mak
include make/bin.mak
include make/matlab.mak
include make/octave.mak
include make/doc.mak
include make/dist.mak

clean:
	rm -f  `find . -name '*~'`
	rm -f  `find . -name '.DS_Store'`
	rm -f  `find . -name '.gdb_history'`
	rm -f  `find . -name '._*'`
	rm -rf ./results

archclean: clean

distclean:

info:
	$(call echo-title,General settings)
	$(call dump-var,deps)
	$(call echo-var,PROFILE)
	$(call echo-var,DEBUG)
	$(call echo-var,VER)
	$(call echo-var,ARCH)
	$(call echo-var,CC)
	$(call echo-var,COMPILER)
	$(call echo-var,COMPILER_VER)
	$(call echo-var,STD_CFLAGS)
	$(call echo-var,STD_LDFLAGS)
	$(call echo-var,DISABLE_SSE2)
	$(call echo-var,DISABLE_AVX)
	$(call echo-var,DISABLE_THREADS)
	$(call echo-var,DISABLE_OPENMP)
	@printf "\nThere are %s lines of code.\n" \
	`cat $(m_src) $(mex_src) $(dll_src) $(dll_hdr) $(bin_src) | wc -l`

# Holw help works: cat this file,
# skip the first block until an empty line is found (twice)
# print the first block until an empty line,
# remove the `# ' prefix from each remaining line

help:
	@cat Makefile | \
	sed -n '1,/^$$/!p' | \
	sed -n '1,/^$$/!p' | \
	sed -n '1,/^$$/p' | \
	sed 's/^# \{0,1\}\(.*\)$$/\1/'

# --------------------------------------------------------------------
#                                                 Include dependencies
# --------------------------------------------------------------------

.PRECIOUS: $(deps)

ifeq ($(filter $(no_dep_targets), $(MAKECMDGOALS)),)
-include $(deps)
endif
