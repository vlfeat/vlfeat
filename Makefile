# file:        Makfile
# author:      Andrea Vedaldi
# description: Build everything

NAME               := vlfeat
VER                := 0.9.1
HOST               := ganesh.cs.ucla.edu:/var/www/vlfeat
NDEBUG             :=

# --------------------------------------------------------------------
#                                                       Error messages
# --------------------------------------------------------------------

err_no_arch  =
err_no_arch +=$(shell echo "** Unknown host architecture '$(UNAME)'. This identifier"   1>&2)
err_no_arch +=$(shell echo "** was obtained by running 'uname -sm'. Edit the Makefile " 1>&2)
err_no_arch +=$(shell echo "** to add the appropriate configuration."                   1>&2)
err_no_arch +=config

err_internal  =$(shell echo Internal error)
err_internal +=internal

# --------------------------------------------------------------------
#                                  Architecture Identification Strings
# --------------------------------------------------------------------

Darwin_PPC_ARCH             := mac
Darwin_Power_Macintosh_ARCH := mac
Darwin_i386_ARCH            := mci
Linux_i386_ARCH             := glx
Linux_i686_ARCH             := glx
Linux_unknown_ARCH          := glx
Linux_x86_64_ARCH           := g64

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

# == DISTRIBUTION AND VERSION ==
#
# VER:
# DIST:
# BINDIST:
# NDEBUG:
#
# == PROGRAMS ==
#
# CC:           C compiler (e.g. gcc).
# MATLAB:       Matlab (e.g. matlab)
# MEX:          MEX compiler (e.g mex).
# LIBTOOL:      libtool (used only under Mac)
#
# The following programs are needed only to generate the documentation
# and the source code and binary distributions:
#
# GIT:          Used to create distribution (e.g. git)
# PYTHON:       Python interpreter (e.g. python)
# DOXYGEN:      Doxygen
# DVIPNG:       TeX DVI to PNG
# DVIPS:        TeX DVI to PS
# CONVERT:      ImageMagik convert utility
# FIG2DEV:      X-Fig FIG format conversion program
# EPS2PDF:      EPS to PDF
# 
# == EXECUTABLES ==
#
# BINDIR:       where to put the exec (and libraries)
# CLFAGS:       flags passed to $(CC) to compile a C source
# LDFLAGS:      flags passed to $(CC) to link C objects into an exec
#
# == LIBRARY ==
#
# DLL_CLFAGS:   flags passed to $(CC) to compile a DLL C source
# DLL_SUFFIX:   suffix of a DLL (.so, .dylib)
#
# == MEX FILES ==
#
# MATLABPATH:   MATALB root path
# MEX_BINDIR:   where to put mex files
# MEX_SUFFIX:   suffix of a MEX file (.mexglx, .mexmac, ...)
# MEX_FLAGS:    flags passed to $(MEX)
# MEX_CFLAGS:   flags added to the CLFAGS variable of $(MEX)
# MEX_LDFLAGS:  flags added to the LDFLAGS variable of $(MEX)
#
# == DOCUMENTATION ==
#

ifndef NDEBUG
DEBUG=yes
endif

MATLAB          ?= matlab
MEX             ?= mex
CC              ?= cc
LIBTOOL         ?= libtool
PYTHON          ?= python
AR              ?= ar
DOXYGEN         ?= doxygen
CONVERT         ?= convert
DVIPNG          ?= dvipng
DVIPS           ?= dvips
FIG2DEV         ?= fig2dev
EPSTOPDF        ?= epstopdf
GIT             ?= git

# $(call make-silent, CMD) makes the execution of $(CMD) silent
define make-silent
$(eval define $(strip $(1))
@function run () {                         \
printf '%10s %s\n' $(strip $(1)) "$$(@)" ; \
output=$$$$($($(strip $(1))) "$$$$@") ;    \
status=$$$$? ;                             \
} ; run
endef)
endef

$(eval $(call make-silent, CC      ))
$(eval $(call make-silent, MEX     ))
$(eval $(call make-silent, FIG2DEV ))
$(eval $(call make-silent, DVIPS   ))
$(eval $(call make-silent, DVIPNG  ))
$(eval $(call make-silent, DOXYGEN ))
$(eval $(call make-silent, CONVERT ))

CFLAGS          += -I$(CURDIR) -pedantic 
CFLAGS          += -Wall -std=c89 -O3
CFLAGS          += -Wno-unused-function 
CFLAGS          += -Wno-long-long
CFLAGS          += $(if $(DEBUG), -O0 -g)
LDFLAGS         += -L$(BINDIR) -l$(DLL_NAME)

DLL_NAME         = vl
DLL_CFLAGS       = $(CFLAGS) -fvisibility=hidden -fPIC -DVL_BUILD_DLL

MEX_CFLAGS       = $(CFLAGS) -Itoolbox
MEX_LDFLAGS      = -L$(BINDIR) -l$(DLL_NAME)

UNAME           := $(shell uname -sm)
ARCH            := $($(shell echo "$(UNAME)" | tr \  _)_ARCH)

# Mac OS X on PPC processor
ifeq ($(ARCH),mac)
BINDIR          := bin/mac
DLL_SUFFIX      := dylib
MEX_SUFFIX      := mexmac
CFLAGS          += -D__BIG_ENDIAN__ -Wno-variadic-macros 
CLFAGS          += $(if $(DEBUG), -gstabs+)
LDFLAGS         += -lm
DLL_CFLAGS      += -fvisibility=hidden
MATLABPATH      ?= $(dir $(shell readlink `which mex`))/..
MEX_FLAGS       += -lm CC='gcc' CXX='g++' LD='gcc'
MEX_CFLAGS      += 
MEX_LDFLAGS     +=
endif

# Mac OS X on Intel processor
ifeq ($(ARCH),mci)
BINDIR          := bin/maci
DLL_SUFFIX      := dylib
MEX_SUFFIX      := mexmaci
CFLAGS          += -D__LITTLE_ENDIAN__ -Wno-variadic-macros
CFLAGS          += $(if $(DEBUG), -gstabs+)
LDFLAGS         += -lm
MATLABPATH      ?= $(dir $(shell readlink `which mex`))/..
MEX_FLAGS       += -lm
MEX_CFLAGS      += 
MEX_LDFLAGS     += 
endif

# Linux-32
ifeq ($(ARCH),glx)
BINDIR          := bin/glx
MEX_SUFFIX      := mexglx
DLL_SUFFIX      := so
CFLAGS          += -D__LITTLE_ENDIAN__ -std=c99
LDFLAGS         += -lm -Wl,--rpath,\\\$$ORIGIN/
MATLABPATH      ?= $(dir $(shell readlink -f `which mex`))/..
MEX_FLAGS       += -lm
MEX_CFLAGS      += 
MEX_LDFLAGS     += -Wl,--rpath,\\\$$ORIGIN/
endif

# Linux-64
ifeq ($(ARCH),g64)
BINDIR          := bin/g64
MEX_SUFFIX      := mexa64
DLL_SUFFIX      := so
CFLAGS          += -D__LITTLE_ENDIAN__ -std=c99
LDFLAGS         += -lm
MATLABPATH      ?= $(dir $(shell readlink -f `which mex`))/..
MEX_FLAGS       += -lm -Wl,--rpath,\\\$$ORIGIN/
MEX_CFLAGS      += 
MEX_LDFLAGS     += -Wl,--rpath,\\\$$ORIGIN/
endif

DIST            := $(NAME)-$(VER)
BINDIST         := $(DIST)-bin
MEX_BINDIR      := toolbox/$(MEX_SUFFIX)

# Print an error message if the architecture was not recognized.
ifeq ($(ARCH),)
die:=$(error $(err_no_arch))
endif

# Sanity check
ifeq ($(DLL_SUFFIX),)
die:=$(error $(err_internal))
endif


# --------------------------------------------------------------------
#                                  Generation of auxiliary directories
# --------------------------------------------------------------------

define gendir
$(1)-dir=$(foreach x,$(2),$(x)/.dirstamp)
endef

.PRECIOUS: %/.dirstamp
%/.dirstamp :
	@printf "%10s %s" MK "$(dir $@)"
	@mkdir -p $(dir $@)
	@echo "Directory generated by make." > $@

$(eval $(call gendir, doc,     doc doc/demo doc/figures             ))
$(eval $(call gendir, results, results                              ))
$(eval $(call gendir, bin,     $(BINDIR) $(BINDIR)/objs             ))
$(eval $(call gendir, mex,     $(MEX_BINDIR)                        ))

.PHONY : all
all : dll all-bin all-mex $(results-dir) $(doc-dir)

# --------------------------------------------------------------------
#                                   Build static and dynamic libraries
# --------------------------------------------------------------------
#
# Objects and dependecies are placed in the $(BINDIR)/objs/
# directory. The makefile creates a static and a dynamic version of
# the library. Depending on the architecture, one or more of the
# following files are produced:
#
# $(OBJDIR)/libvl.so      ELF dynamic library (Linux)
# $(OBJDIR)/libvl.dylib   Mach-O dynamic library (Mac OS X)
#
# == Note on Mac OS X ==
#
# On Mac we set the install name of the library to look in
# @loader_path/.  This means that any binary linked (either an
# executable or another DLL) will search in his own directory for a
# copy of libvl (this behaviour can then be changed by
# install_name_tool).

dll_tgt := $(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)
dll_src := $(wildcard vl/*.c)
dll_obj := $(addprefix $(BINDIR)/objs/, $(notdir $(dll_src:.c=.o)))
dll_dep := $(dll_obj:.o=.d)

.PHONY: dll
dll: $(dll_tgt)

.PRECIOUS: $(BINDIR)/objs/%.d

$(BINDIR)/objs/%.o : vl/%.c $(bin-dir)
	$(CC) $(DLL_CFLAGS) -c $< -o $@

$(BINDIR)/objs/%.d : vl/%.c $(bin-dir)
	$(CC) $(DLL_CFLAGS)                                          \
	       -M -MT '$(BINDIR)/objs/$*.o $(BINDIR)/objs/$*.d'      \
	       $< -MF $@

$(BINDIR)/lib$(DLL_NAME).dylib : $(dll_obj)
	@echo "DYLIB '$@'"
	@$(LIBTOOL) -dynamic                                         \
                    -flat_namespace                                  \
                    -install_name @loader_path/libvl.dylib           \
	            -compatibility_version $(VER)                    \
                    -current_version $(VER)                          \
	            -o $@ -undefined suppress $^

$(BINDIR)/lib$(DLL_NAME).so : $(dll_obj)
	$(CC) $(DLL_CFLAGS) -shared $(^) -o $(@)

# --------------------------------------------------------------------
#                                                          Build execs
# --------------------------------------------------------------------
# We place the exacutables in $(BINDIR).

bin_src := $(wildcard src/*.c)
bin_tgt := $(notdir $(bin_src))
bin_tgt := $(addprefix $(BINDIR)/, $(bin_tgt:.c=))

.PHONY: all-bin
all-bin: $(bin_tgt)

$(BINDIR)/% : src/%.c src/generic-driver.h $(dll_tgt)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

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

mex_src := $(shell find toolbox -name "*.c")
mex_tgt := $(addprefix $(MEX_BINDIR)/,                               \
	   $(notdir $(mex_src:.c=.$(MEX_SUFFIX)) ) )
mex_dep := $(mex_tgt:.$(MEX_SUFFIX)=.d)

vpath %.c $(shell find toolbox -type d)

.PHONY: all-mex
all-mex : $(mex_tgt)

$(MEX_BINDIR)/%.d : %.c $(mex-dir) $(dll_tgt)
	$(CC) $(MEX_CFLAGS)                                          \
               -I$(MATLABPATH)/extern/include -M -MT                 \
	       '$(MEX_BINDIR)/$*.$(MEX_SUFFIX) $(MEX_BINDIR)/$*.d'   \
	       $< -MF $@

$(MEX_BINDIR)/%.$(MEX_SUFFIX) : %.c $(mex-dir) $(dll_tgt)
	$(MEX) CFLAGS='$$CFLAGS  $(MEX_CFLAGS)'                      \
	       LDFLAGS='$$LDFLAGS $(MEX_LDFLAGS)'                    \
	       $(MEX_FLAGS)                                          \
	       $< -outdir $(dir $(@))
	@test -e $(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX) ||       \
	 ln -sf ../../$(BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)         \
	        $(MEX_BINDIR)/lib$(DLL_NAME).$(DLL_SUFFIX)

# --------------------------------------------------------------------
#                                                  Build documentation
# --------------------------------------------------------------------

# doc-figures: create figures from X-Fig sources
# 

.PRECIOUS: *

.PHONY: doc, doc-figures, doc-api, doc-toolbox, doc-web, doc-demo
.PHONY: doc-bindist, doc-distclean

m_src    := $(shell find toolbox -name "*.m")
fig_src  := $(wildcard docsrc/figures/*.fig)
demo_src := $(wildcard doc/demo/*.eps)

pdf_tgt := #$(fig_src:.fig=.pdf) 
eps_tgt := #$(fig_src:.fig=.eps)
png_tgt := $(subst docsrc/,doc/,$(fig_src:.fig=.png)) $(demo_src:.eps=.png)

doc/figures/%.png : doc/figures/%.dvi
	$(DVIPNG) -D 75 -T tight -o $@ $<

doc/figures/%.eps : doc/figures/%.dvi
	$(DVIPS) -E -o $@ $<

doc/figures/%-raw.tex : docsrc/figures/%.fig
	$(FIG2DEV) -L pstex_t -p $*-raw.ps $< $@ 

doc/figures/%-raw.ps : docsrc/figures/%.fig
	$(FIG2DEV) -L pstex $< $@

doc/figures/%.dvi doc/figures/%.aux doc/figures/%.log :  \
  doc/figures/%.tex doc/figures/%-raw.tex doc/figures/%-raw.ps $(doc-dir)
	@echo LATEX $@
	@cd doc/figures ; latex $*.tex ; \
	rm -f $*.log $*.aux

doc/figures/%.tex : $(doc-dir)
	@printf "%20s %s" GEN $@
	@/bin/echo '\documentclass[landscape]{article}' >$@
	@/bin/echo '\usepackage[margin=0pt]{geometry}' >>$@
	@/bin/echo '\usepackage{graphicx,color}'       >>$@
	@/bin/echo '\begin{document}'                  >>$@
	@/bin/echo '\pagestyle{empty}'                 >>$@
	@/bin/echo '\input{$(*)-raw.tex}'              >>$@
	@/bin/echo '\end{document}'	               >>$@

doc/demo/%.png : doc/demo/%.eps
	@echo CONVERT $< \=\=\> $@
	@$(CONVERT) -resample 75 $< png:$@

doc/%.pdf : doc/%.eps
	@echo EPSTOPDF $< \=\=\> $@
	@$(EPSTOPDF) --outfile=$@ $<

doc: doc-fig doc-api doc-toolbox doc-web

doc-deep: all $(doc-dir) $(results-dir)
	cd toolbox ; \
	$(MATLAB) -nojvm -nodesktop -r 'vlfeat_setup;demo_all;exit'

doc-fig: $(png_tgt) $(pdf_tgt) $(eps_tgt)

doc-api: doc-fig VERSION
	$(DOXYGEN) docsrc/doxygen.conf

doc-toolbox:
	$(PYTHON) docsrc/mdoc.py toolbox doc/toolbox --site=docsrc/web.xml

doc-web: doc-fig
	$(PYTHON) docsrc/webdoc.py --srcdir=docsrc/ --outdir=doc \
	          docsrc/web.xml
	rsync -arv docsrc/images doc

doc-bindist: $(NAME) doc
	rsync -arv doc $(NAME) --exclude=doc/demo/*.eps

doc-distclean:
	rm -f  docsrc/*.pyc
	rm -rf doc

doc-wiki: $(NAME) 
	$(PYTHON) doc/mdoc.py --wiki toolbox doc/wiki

# --------------------------------------------------------------------
#                                                       Clean and dist
# --------------------------------------------------------------------

.PHONY: $(NAME) 
.PHONY: dist, bindist, clean, distclean
.PHONY: post, post-doc, autorights

VERSION:
	echo "Version $(VER)"            > VERSION
	echo "Archive created on `date`" >>VERSION

clean:
	rm -rf `find ./bin -name 'objs' -type d`
	rm -f  `find . -name '*~'`
	rm -f  `find . -name '.DS_Store'`
	rm -f  `find . -name '.gdb_history'`
	rm -f  `find . -name '._*'`
	rm -rf  ./results
	rm -rf $(NAME)

distclean: clean doc-distclean
	rm -rf bin
	for i in mexmac mexmaci mexglx mexw32 mexa64 ;               \
	do                                                           \
	   rm -rf "toolbox/$${i}" ;                                  \
	done
	rm -f $(NAME)-*.tar.gz

$(NAME): VERSION
	rm -rf $(NAME)
	$(GIT) archive --prefix=$(NAME)/ HEAD | tar xvf -
	cp VERSION $(NAME)

dist: $(NAME)
	COPYFILE_DISABLE=1                                           \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1                           \
	tar czvf $(DIST).tar.gz $(NAME)

bindist: $(NAME) all doc
	rsync -arv --exclude=objs --exclude=*.pdb bin $(NAME)
	rsync -arv --exclude=*.eps doc $(NAME)
	rsync -arv --include=*mexmaci                                \
	           --include=*mexmac                                 \
	           --include=*.dylib                                 \
	           --include=*.so                                    \
	           --include=*mexw32                                 \
	           --include=*mexglx                                 \
	           --include=*mexa64                                 \
	           --include=*dll                                    \
		   --exclude=*                                       \
	           toolbox/ $(NAME)/toolbox 
	tar czvf $(BINDIST).tar.gz $(NAME)


post:
	scp $(DIST).tar.gz $(BINDIST).tar.gz                         \
	    $(HOST)/download

post-doc: doc
	rsync -aP doc/ $(HOST)

autorights: distclean
	autorights                                                   \
	  toolbox vl                                                 \
	  --recursive                                                \
	  --verbose                                                  \
	  --template doc/copylet.txt                                 \
	  --years 2007                                               \
	  --authors "Andrea Vedaldi and Brian Fulkerson"             \
	  --holders "Andrea Vedaldi and Brian Fulkerson"             \
	  --program "VLFeat"

# --------------------------------------------------------------------
#                                               Automatic Dependencies
# --------------------------------------------------------------------

ifeq ($(filter doc dox clean distclean info, $(MAKECMDGOALS)),)
include $(dll_dep) $(mex_dep)
endif

# --------------------------------------------------------------------
#                                                       Debug Makefile
# --------------------------------------------------------------------

# $(call tocol,VAR) prints the content of a variable VAR
define tocol
@echo $(1) =
@echo $($(1)) | sed -E 's/([^ ]+ [^ ]+ [^ ]+) */\1#/g' | tr '#' '\n' | column -t
endef

.PHONY: info
info :
	$(call tocol,dll_src)
	$(call tocol,dll_obj)
	$(call tocol,dll_dep)
	$(call tocol,mex_src)
	$(call tocol,fig_src)
	$(call tocol,demo_src)
	$(call tocol,mex_tgt)
	$(call tocol,bin_src)
	$(call tocol,bin_tgt)
	$(call tocol,pdf_tgt)
	$(call tocol,eps_tgt)
	$(call tocol,png_tgt)
	@echo "ARCH         = $(ARCH)"
	@echo "DIST         = $(DIST)"
	@echo "BINDIST      = $(BINDIST)"
	@echo "MEX_BINDIR   = $(MEX_BINDIR)"
	@echo "DLL_SUFFIX   = $(DLL_SUFFIX)"
	@echo "MEX_SUFFIX   = $(MEX_SUFFIX)"
	@echo "CFLAGS       = $(CFLAGS)"
	@echo "LDFLAGS      = $(LDFLAGS)"
	@echo "MATLAB       = $(MATLAB)"
	@echo "MEX_FLAGS    = $(MEX_FLAGS)"
	@echo "MEX_CFLAGS   = $(MEX_CFLAGS)"
	@echo "MEX_LDFLAGS  = $(MEX_LDFLAGS)"

.PHONY: wc
wc:
	cat $(m_src) $(mex_src) $(dll_src) $(bin_src) | wc

# --------------------------------------------------------------------
#                                                        Xcode Support
# --------------------------------------------------------------------

.PHONY: dox-
dox- : dox

.PHONY: dox-clean
dox-clean:
