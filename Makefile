# file:        Makfile
# author:      Andrea Vedaldi
# description: Build everything

NAME               := vlfeat
VER                := 0.9.1
HOST               := ganesh.cs.ucla.edu:/var/www/vlfeat/
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
# LIBTOOL       libtool (used only under Mac)
#
# The following programs are needed only to generate the documentation
# and the source code and binary distributions:
#
# GIT:          Used to create distribution (e.g. git)
# PYTHON:       Python interpreter (e.g. python)
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

CFLAGS          += -I$(CURDIR) -pedantic -Wall -std=c89 -O3
CFLAGS          += -Wno-unused-function 
CFLAGS          += -Wno-long-long
CFLAGS          += $(if $(DEBUG), -O0 -g)

LDFLAGS         += $(BINDIR)/libvl.a

DLL_CFLAGS       = $(CFLAGS) -fvisibility=hidden -fPIC -DVL_BUILD_DLL
MEX_FLAGS        = -L$(BINDIR) -lvl
MEX_CFLAGS       = $(CFLAGS) -Itoolbox
MEX_LDFLAGS      =

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
MATLABPATH          ?= $(dir $(shell readlink `which mex`))/..
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
MATLABPATH          ?= $(dir $(shell readlink `which mex`))/..
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
LDFLAGS         += -lm
MATLABPATH          ?= $(dir $(shell readlink -f `which mex`))/..
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
MATLABPATH          ?= $(dir $(shell readlink -f `which mex`))/..
MEX_FLAGS       += -lm
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

.PHONY : all
all : all-dir all-lib all-bin all-mex

# create auxiliary directories
.PHONY: all-dir
all-dir: results/.dirstamp doc/demo/.dirstamp

# trick to make directories
.PRECIOUS: %/.dirstamp
%/.dirstamp :
	@echo "   MK '$(dir $@)'"
	@mkdir -p $(dir $@)
	@echo "Directory generated by make." > $@

# --------------------------------------------------------------------
#                                   Build static and dynamic libraries
# --------------------------------------------------------------------
#
# Objects are placed in the $(BINDIR)/objs/ directory. The makefile
# creates a static and a dynamic version of the library. Depending on
# the architecture, one or more of the following files are produced:
#
# $(OBJDIR)/libvl.a       Static library
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

# We place the object and dependency files in $(BINDIR)/objs/ and
# the library in $(BINDIR)/libvl.a.

lib_src := $(wildcard vl/*.c)
lib_obj := $(addprefix $(BINDIR)/objs/, $(notdir $(lib_src:.c=.o)))
lib_dep := $(lib_obj:.o=.d)

.PHONY: all-lib dll lib
all-lib: dll lib
lib: $(BINDIR)/libvl.a 
dll: $(BINDIR)/libvl.$(DLL_SUFFIX)

.PRECIOUS: $(BINDIR)/objs/%.d

$(BINDIR)/objs/%.o : vl/%.c $(BINDIR)/objs/.dirstamp
	@echo "   CC '$<' ==> '$@'"
	@$(CC) $(DLL_CFLAGS) -c $< -o $@

$(BINDIR)/objs/%.d : vl/%.c $(BINDIR)/objs/.dirstamp
	@echo "   D  '$<' ==> '$@'"
	@$(CC) $(DLL_CFLAGS)                                         \
	       -M -MT '$(BINDIR)/objs/$*.o $(BINDIR)/objs/$*.d'      \
	       $< -MF $@

$(BINDIR)/libvl.a : $(lib_obj)
	@echo "   A  '$@'"
	@$(AR) rcs $@ $^

$(BINDIR)/libvl.dylib : $(lib_obj)
	@echo "DYLIB '$@'"
	@$(LIBTOOL) -dynamic                                         \
                    -flat_namespace                                  \
                    -install_name @loader_path/libvl.dylib           \
	            -compatibility_version $(VER)                    \
                    -current_version $(VER)                          \
	            -o $@ -undefined suppress $^

$(BINDIR)/libvl.so : $(lib_obj)
	@echo "   SO '$@'"
	@$(CC) $(DLL_CFLAGS) -shared $(^) -o $(@)

# --------------------------------------------------------------------
#                                                          Build execs
# --------------------------------------------------------------------
# We place the exacutables in $(BINDIR).

bin_src := $(wildcard src/*.c)
bin_tgt := $(notdir $(bin_src))
bin_tgt := $(addprefix $(BINDIR)/, $(bin_tgt:.c=))

.PHONY: all-bin
all-bin : $(bin_tgt)

$(BINDIR)/% : src/%.c $(BINDIR)/libvl.a src/generic-driver.h
	@echo "   CC '$<' ==> '$@'"
	@$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

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
all-mex : $(mex_tgt) dll

$(MEX_BINDIR)/.dirstamp:
	@echo "   MK '$(dir $@)'"
	@mkdir -p $(MEX_BINDIR)
	@echo "Directory generated by make." > $@
	@test -h $(MEX_BINDIR)/libvl.$(DLL_SUFFIX)                   \
	 || ln -sf ../../$(BINDIR)/libvl.$(DLL_SUFFIX)               \
	           $(MEX_BINDIR)/libvl.$(DLL_SUFFIX)

$(MEX_BINDIR)/%.d : %.c $(MEX_BINDIR)/.dirstamp
	@echo "   D  '$<' ==> '$@'"
	@$(CC) $(MEX_CFLAGS)                                         \
               -I$(MATLABPATH)/extern/include -M -MT                 \
	       '$(MEX_BINDIR)/$*.$(MEX_SUFFIX) $(MEX_BINDIR)/$*.d'   \
	       $< -MF $@

$(MEX_BINDIR)/%.$(MEX_SUFFIX) : %.c $(MEX_BINDIR)/.dirstamp
	@echo "   MX '$<' ==> '$@'"
	@$(MEX) CFLAGS='$$CFLAGS  $(MEX_CFLAGS)'                     \
		LDFLAGS='$$LDFLAGS $(MEX_LDFLAGS)'                   \
	        $(MEX_FLAGS)                                         \
	        $< -outdir $(dir $(@))

# --------------------------------------------------------------------
#                                                  Build documentation
# --------------------------------------------------------------------

.PHONY: doc, doc-figures, doc-api, doc-toolbox, doc-web, doc-demo
.PHONY: doc-distclean

m_src := $(shell find toolbox -name "*.m")
fig_src := $(wildcard doc/figures/*.fig)
demo_src := $(wildcard doc/demo/*.eps)

pdf_tgt  := $(fig_src:.fig=.pdf) 
eps_tgt  := $(fig_src:.fig=.eps)
png_tgt  := $(fig_src:.fig=.png) $(demo_src:.eps=.png)

doc/figures/%.png : doc/figures/%.dvi
	@echo DVIPNG $< \=\=\> $@
	@cd doc/figures ; $(DVIPNG) -D 75 -T tight -o $*.png *.dvi

doc/figures/%.eps : doc/figures/%.dvi
	@echo DVIPS $< \=\=\> $@	
	@cd doc/figures ; $(DVIPS) -E -o $*.eps $*.dvi

doc/figures/%-raw.tex : doc/figures/%.fig
	@echo FIG2DEV $< \=\=\> $@	
	@$(FIG2DEV) -L pstex_t -p $*-raw.ps $< $@

doc/figures/%-raw.ps : doc/figures/%.fig
	@echo FIG2DEV $< \=\=\> $@
	@$(FIG2DEV) -L pstex $< $@

doc/figures/%.dvi doc/figures/%.aux doc/figures/%.log :  \
  doc/figures/%.tex doc/figures/%-raw.tex doc/figures/%-raw.ps
	@echo LATEX $< \=\=\> $@
	@cd doc/figures ; latex $*.tex ; \
	rm $*.log $*.aux

doc/figures/%.tex :
	@echo GEN $@
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

doc: doc-demo doc-fig doc-api doc-toolbox doc-web

doc-demo: all
	cd toolbox ; \
	$(MATLAB) -nojvm -nodesktop -r 'vlfeat_setup;demo_all;exit'

doc-fig: $(png_tgt) $(pdf_tgt) $(eps_tgt)

doc-api: doc-fig VERSION
	$(DOXYGEN) doc/doxygen.conf

doc-toolbox:
	$(PYTHON) doc/mdoc.py toolbox doc/toolbox --site=doc/web/src/site.xml

doc-web: doc-fig
	$(PYTHON) doc/webdoc.py --srcdir=doc/web/src/ --outdir=doc/web \
	          doc/web/src/site.xml
	ln -sf ../toolbox doc/web/toolbox
	ln -sf ../figures doc/web/figures
	ln -sf ../demo doc/web/demo
	ln -sf ../api doc/web/api
	
doc-distclean:
	rm -rf doc/api
	rm -rf doc/toolbox
	rm -f doc/web/*.html doc/web/*.css
	rm -f $(png_tgt) $(pdf_tgt) $(eps_tgt)

doc-wiki: $(NAME) 
	$(PYTHON) doc/mdoc.py --wiki toolbox doc/wiki

# --------------------------------------------------------------------
#                                                       Clean and dist
# --------------------------------------------------------------------

.PHONY: $(NAME) 
.PHONY: dist, bindist, clean, distclean
.PHONY: post, post-doc, autorights

TIMESTAMP:
	echo "Version $(VER)"            > TIMESTAMP
	echo "Archive created on `date`" >>TIMESTAMP

VERSION:
	echo "$(VER)" > VERSION

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
	for i in mexmac mexmaci mexglx mexw32 mexa64 dll pdb ;       \
	do                                                           \
	   rm -rf "toolbox/$${i}" ;                                  \
	done
	rm -f  $(NAME)-*.tar.gz

$(NAME): TIMESTAMP VERSION
	rm -rf $(NAME)
	$(GIT) archive --prefix=$(NAME)/ HEAD | tar xvf -
	cp TIMESTAMP $(NAME)
	cp VERSION $(NAME)

dist: $(NAME)
	COPYFILE_DISABLE=1                                           \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1                           \
	tar czvf $(DIST).tar.gz $(NAME)

bindist: $(NAME) all doc
	cp -rp bin $(NAME)
	cp -rp doc $(NAME)
	for i in mexmaci mexmac mexw32 mexglx mexa64 dll ;           \
	do                                                           \
	  fin2d toolbox                                              \
               -name "*.$${i}"                                       \
               -exec cp -p "{}" "$(NAME)/{}" \; ;                    \
	done
	COPYFILE_DISABLE=1                                           \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1                           \
	tar czvf $(BINDIST).tar.gz                                   \
	    --exclude "objs"                                         \
	    $(NAME)

post:
	scp $(DIST).tar.gz $(BINDIST).tar.gz                         \
	    $(HOST)/download

post-doc: doc
	rsync -rv doc/api -e "ssh" $(HOST)

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
include $(lib_dep) $(mex_dep)
endif

# --------------------------------------------------------------------
#                                                       Debug Makefile
# --------------------------------------------------------------------

.PHONY: info
info :
	@echo "lib_src ="
	@echo $(lib_src) 
	@echo "lib_obj ="
	@echo $(lib_obj) 
	@echo "lib_dep ="
	@echo $(lib_dep) 
	@echo "mex_src ="
	@echo $(mex_src) 
	@echo "fig_src ="
	@echo $(fig_src)
	@echo "demo_src ="
	@echo $(demo_src) 
	@echo "mex_tgt ="
	@echo $(mex_tgt) 
	@echo "bin_src ="
	@echo $(bin_src) 
	@echo "bin_tgt ="
	@echo $(bin_tgt)
	@echo "pdf_tgt ="
	@echo $(pdf_tgt)
	@echo "eps_tgt ="
	@echo $(eps_tgt)
	@echo "png_tgt ="
	@echo $(png_tgt)
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
	cat $(m_src) $(mex_src) $(lib_src) $(bin_src) | wc

# --------------------------------------------------------------------
#                                                        Xcode Support
# --------------------------------------------------------------------

.PHONY: dox-
dox- : dox

.PHONY: dox-clean
dox-clean:
