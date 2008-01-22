# file:        Makefile
# author:      Andrea Vedaldi
# description: Build everything

NAME               := vlfeat
VER                := 0.9
DIST                = $(NAME)-$(VER)

# --------------------------------------------------------------------
#                                                       Error messages
# --------------------------------------------------------------------

err_no_arch  =
err_no_arch +=$(shell echo "** Unknown host architecture '$(UNAME)'. This identifier"   1>&2)
err_no_arch +=$(shell echo "** was obtained by running 'uname -sm'. Edit the Makefile " 1>&2)
err_no_arch +=$(shell echo "** to add the appropriate configuration."                   1>&2)
err_no_arch +=Configuration failed

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

# executables
MEX              ?= mex
CC               ?= cc

# generic flags
CFLAGS           += -I. -pedantic -Wall -std=c89 -g -O0
#CFLAGS           += -Wno-overlength-strings
#CFLAGS           += -Wno-variadic-macros
CFLAGS           += -Wno-unused-function 
CFLAGS           += -Wno-long-long
LDFLAGS          +=
MEX_CFLAGS        = CFLAGS='$$CFLAGS $(CFLAGS)' -L$(BINDIR) -lvl -Itoolbox

# Determine on the flight the system we are running on
Darwin_PPC_ARCH    := mac
Darwin_i386_ARCH   := mci
Linux_i386_ARCH    := glx
Linux_i686_ARCH    := glx
Linux_x86_64_ARCH  := g64
Linux_unknown_ARCH := glx

UNAME             := $(shell uname -sm)
ARCH              := $($(shell echo "$(UNAME)" | tr \  _)_ARCH)

mac_BINDIR       := bin/mac
mac_CFLAGS       := -Wno-variadic-macros -D__BIG_ENDIAN__
mac_LDFLAGS      := 
mac_MEX_CFLAGS   := 
mac_MEX_SUFFIX   := mexmac

mci_BINDIR       := bin/maci
mci_CFLAGS       := -Wno-variadic-macros -D__LITTLE_ENDIAN__ -gstabs+
mci_LDFLAGS      :=
mci_MEX_CFLAGS   :=
mci_MEX_SUFFIX   := mexmaci

glx_BINDIR       := bin/glx
glx_CFLAGS       := -D__LITTLE_ENDIAN__ -std=c99
glx_LDFLAGS      := -lm
glx_MEX_CFLAGS   :=
glx_MEX_SUFFIX   := mexglx

g64_BINDIR       := bin/g64
g64_CFLAGS       := -D__LITTLE_ENDIAN__ -std=c99 -fPIC
g64_LDFLAGS      := -lm
g64_MEX_CFLAGS   :=
g64_MEX_SUFFIX   := mexa64

CFLAGS           += $($(ARCH)_CFLAGS)
LDFLAGS          += $($(ARCH)_LDFLAGS)
MEX_SUFFIX       := $($(ARCH)_MEX_SUFFIX)
MEX_CFLAGS       += $($(ARCH)_MEX_CFLAGS)
BINDIR           := $($(ARCH)_BINDIR)
BINDIST          := $(DIST)-bin

ifeq ($(ARCH),)
die:=$(error $(err_no_arch))
endif

.PHONY : all
all : all-dir all-lib all-bin all-mex

# create auxiliary directories
.PHONY: all-dir
all-dir: results/.dirstamp doc/figures/demo/.dirstamp

# trick to make directories
.PRECIOUS: %/.dirstamp
%/.dirstamp :	
	mkdir -p $(dir $@)
	echo "I'm here" > $@

# --------------------------------------------------------------------
#                                                        Build libvl.a
# --------------------------------------------------------------------
# We place the object and dependency files in $(BINDIR)/objs/ and
# the library in $(BINDIR)/libvl.a.

lib_src := $(wildcard vl/*.c)
lib_obj := $(notdir $(lib_src))
lib_obj := $(addprefix $(BINDIR)/objs/, $(lib_obj:.c=.o))
lib_dep := $(lib_obj:.o=.d)

# create library libvl.a
.PHONY: all-lib
all-lib: $(BINDIR)/libvl.a

.PRECIOUS: $(BINDIR)/objs/%.d

$(BINDIR)/objs/%.o : vl/%.c $(BINDIR)/objs/.dirstamp
	@echo "   CC '$<' ==> '$@'"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/objs/%.d : vl/%.c $(BINDIR)/objs/.dirstamp
	@echo "   D  '$<' ==> '$@'"
	@$(CC) -M -MT '$(BINDIR)/objs/$*.o $(BINDIR)/objs/$*.d' $< -MF $@

$(BINDIR)/libvl.a : $(lib_obj)
	@echo "   A  '$@'"
	@ar rcs $@ $^

ifeq ($(filter doc dox clean distclean info, $(MAKECMDGOALS)),)
include $(lib_dep) 
endif

# --------------------------------------------------------------------
#                                                       Build binaries
# --------------------------------------------------------------------
# We place the exacutables in $(BINDIR).

bin_src := $(wildcard src/*.c)
bin_tgt := $(notdir $(bin_src))
bin_tgt := $(addprefix $(BINDIR)/, $(bin_tgt:.c=))

.PHONY: all-bin
all-bin : $(bin_tgt)

$(BINDIR)/% : src/%.c $(BINDIR)/libvl.a src/generic-driver.h
	@echo "   CC '$<' ==> '$@'"
	@$(CC) $(CFLAGS) $(LDFLAGS) $< $(BINDIR)/libvl.a -o $@

# --------------------------------------------------------------------
#                                                      Build MEX files
# --------------------------------------------------------------------
# We place the MEX files in toolbox/.

mex_src := $(shell find toolbox -name "*.c")
mex_tgt := $(mex_src:.c=.$(MEX_SUFFIX))

.PHONY: all-mex
all-mex : $(mex_tgt)

toolbox/%.$(MEX_SUFFIX) : toolbox/%.c toolbox/mexutils.h $(BINDIR)/libvl.a
	@echo "   MX '$<' ==> '$@'"
	@$(MEX) $(MEX_CFLAGS) $< -outdir $(dir $(@))

# --------------------------------------------------------------------
#                                                  Build documentation
# --------------------------------------------------------------------

m_src := $(shell find toolbox -name "*.m")

.PHONY: doc dox docdeep mdoc

doc:
	make -C doc all

docdeep: all
	cd toolbox ; \
	matlab -nojvm -nodesktop -r 'vlfeat_setup;demo_all;exit'

dox: VERSION
	make -C doc/figures all
	(test -e dox || mkdir dox)
	doxygen doc/doxygen.conf

.PHONY: modc
mdoc:	doc/toolbox.html

doc/toolbox.html : $(m_src)
	perl mdoc.pl -o doc/toolbox.html toolbox

# --------------------------------------------------------------------
#                                                       Clean and dist
# --------------------------------------------------------------------

TIMESTAMP:
	echo "Version $(VER)"            > TIMESTAMP
	echo "Archive created on `date`" >>TIMESTAMP

VERSION:
	echo "$(VER)" > VERSION

.PHONY: clean
clean:
	make -C doc clean
	rm -rf `find ./bin -name 'objs' -type d`
	rm -f  `find . -name '*~'`
	rm -f  `find . -name '.DS_Store'`
	rm -f  `find . -name '.gdb_history'`
	rm -f  `find . -name '._*'`
	rm -rf  ./results
	rm -rf $(NAME)

.PHONY: distclean
distclean: clean
	make -C doc distclean
	rm -rf bin dox
	rm -f  doc/toolbox.html
	for i in mexmac mexmaci mexglx mexw32 dll pdb ;             \
	do                                                          \
		rm -f `find toolbox -name "*.$${i}"` ;                    \
	done
	rm -f  $(NAME)-*.tar.gz

.PHONY: $(NAME), dist, bindist

$(NAME): TIMESTAMP VERSION
	rm -rf $(NAME)
	git archive --prefix=$(NAME)/ HEAD | tar xvf -
	cp TIMESTAMP $(NAME)
	cp VERSION $(NAME)
		
dist: $(NAME)
	COPYFILE_DISABLE=1						                                \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1                            \
	tar czvf $(DIST).tar.gz $(NAME)
	
bindist: $(NAME) all doc
	cp -rp bin $(NAME)
	cp -rp doc $(NAME)
	for i in mexmaci mexmac mexw32 mexglx mexg64 dll ;            \
	do                                                            \
		find toolbox -name "*.$${i}" -exec cp -p "{}" "$(NAME)/{}" \; ;\
	done
	COPYFILE_DISABLE=1						                                \
	COPY_EXTENDED_ATTRIBUTES_DISABLE=1                            \
	tar czvf $(BINDIST).tar.gz                                    \
	    --exclude "objs"                                          \
			$(NAME)

.PHONY: autorights
autorights: distclean
	autorights \
	  tooblox vl \
	  --recursive    \
	  --verbose \
	  --template doc/copylet.txt \
	  --years 2007   \
	  --authors "Andrea Vedaldi and Brian Fulkerson" \
	  --holders "Andrea Vedaldi and Brian Fulkerson" \
	  --program "VLFeat"

# --------------------------------------------------------------------
#                                                       Debug Makefile
# --------------------------------------------------------------------

.PHONY: info
info :
	@echo "lib_src ="
	@echo $(lib_src) | rs -j
	@echo "lib_obj ="
	@echo $(lib_obj) | rs -j
	@echo "lib_dep ="
	@echo $(lib_dep) | rs -j
	@echo "mex_src ="
	@echo $(mex_src) | rs -j
	@echo "mex_tgt ="
	@echo $(mex_tgt) | rs -j
	@echo "bin_src ="
	@echo $(bin_src) | rs -j
	@echo "bin_tgt ="
	@echo $(bin_tgt) | rs -j
	@echo "DIST         = $(DIST)"
	@echo "BINDIST      = $(BINDIST)"
	@echo "CFLAGS       = $(CFLAGS)"
	@echo "LDFLAGS      = $(LDFLAGS)"
	@echo "MEX_CFLAGS   = $(MEX_CFLAGS)"
	@echo "MEX_LDFLAGS  = $(MEX_LDFLAGS)"
	
# --------------------------------------------------------------------
#                                                       X-Code Support
# --------------------------------------------------------------------

.PHONY: dox-
dox- : dox

.PHONY: dox-clean
dox-clean:

