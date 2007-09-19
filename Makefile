# file:        Makefile
# author:      Andrea Vedaldi
# description: Build everything

NAME               := vlfeat
VER                := beta-3
DIST                = $(NAME)-$(VER)
BINDIST             = $(DIST)-$(ARCH)

# --------------------------------------------------------------------
#                                                                Flags
# --------------------------------------------------------------------

# generic flags
CFLAGS           += -I. -pedantic -Wall -std=c99 -g -O0
CFLAGS           += -Wno-variadic-macros -Wno-unused-function
LDFLAGS          +=
MEX_CFLAGS        = CFLAGS='$$CFLAGS $(CFLAGS)' -L$(BINDIR) -lvl

# Determine on the flight the system we are running on
Darwin_PPC_ARCH    := mac
Darwin_i386_ARCH   := mci
Linux_i386_ARCH    := glx
Linux_unknown_ARCH := glx

ARCH             := $($(shell echo `uname -sp` | tr \  _)_ARCH)

mac_BINDIR       := bin/mac
mac_CFLAGS       :=
mac_LDFLAGS      := -D__BIG_ENDIAN__
mac_MEX_CFLAGS   := 
mac_MEX_SUFFIX   := mexmac

mci_BINDIR       := bin/maci
mci_CFLAGS       := -D__LITTLE_ENDIAN__
mci_LDFLAGS      :=
mci_MEX_CFLAGS   :=
mci_MEX_SUFFIX   := mexmaci

glx_BINDIR       := bin/glx
glx_CFLAGS       := -D__LITTLE_ENDIAN__
glx_LDFLAGS      := -lm
glx_MEX_CFLAGS   :=
glx_MEX_SUFFIX   := mexglx

CFLAGS           += $($(ARCH)_CFLAGS)
LDFLAGS          += $($(ARCH)_LDFLAGS)
MEX_SUFFIX       := $($(ARCH)_MEX_SUFFIX)
MEX_CFLAGS       += $($(ARCH)_MEX_CFLAGS)
BINDIR           := $($(ARCH)_BINDIR)
BINDIST          := $(DIST)-bin

.PHONY : all
all : all-lib all-bin all-mex

# this creates the directory hiearchy 
$(BINDIR) $(BINDIR)/objs $(BINDIR)/objs/.stamp:
	mkdir -p $(BINDIR)/objs
	touch $@

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

$(BINDIR)/objs/%.o : vl/%.c $(BINDIR)/objs/.stamp
	@echo "   CC '$<' ==> '$@'"
	@cc $(CFLAGS) -c $< -o $@

$(BINDIR)/objs/%.d : vl/%.c $(BINDIR)/objs/.stamp
	@echo "   D  '$<' ==> '$@'"
	@cc -M -MT '$(BINDIR)/objs/$*.o $(BINDIR)/objs/$*.d' $< -MF $@

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
	@cc $(CFLAGS) $(LDFLAGS) $^ -o $@

# --------------------------------------------------------------------
#                                                      Build MEX files
# --------------------------------------------------------------------
# We place the MEX files in toolbox/.

mex_src := $(notdir $(basename $(basename $(wildcard toolbox/*.c))))
mex_tgt := $(addprefix toolbox/, $(addsuffix .$(MEX_SUFFIX), $(mex_src)))

.PHONY: all-mex
all-mex : $(mex_tgt)

toolbox/%.$(MEX_SUFFIX) : toolbox/%.c toolbox/mexutils.h $(BINDIR)/libvl.a
	@echo "   MX '$<' ==> '$@'"
	@mex $(MEX_CFLAGS) $< -outdir 'toolbox'

# --------------------------------------------------------------------
#                                                                  Doc
# --------------------------------------------------------------------

.PHONY: doc dox docdeep

doc:
	make -C doc all

docdeep: all
	mkdir -p results
	cd toolbox ; \
	matlab -nojvm -nodesktop -r 'vlfeat_setup;demo_all;exit'

dox:
	make -C doc/figures all
	(test -e dox || mkdir dox)
	doxygen doxygen.conf

# --------------------------------------------------------------------
#                                                       Clean and dist
# --------------------------------------------------------------------

TIMESTAMP:
	echo "Version $(VER)"            > TIMESTAMP
	echo "Archive created on `date`" >>TIMESTAMP

.PHONY: clean
clean:
	make -C doc clean
	rm -rf `find ./bin -name 'objs' -type d`
	rm -f  `find . -name '*~'`
	rm -f  `find . -name '.DS_Store'`
	rm -f  `find . -name '.gdb_history'`
	rm -rf  ./results

.PHONY: distclean
distclean: clean
	make -C doc distclean
	rm -rf bin
	rm -rf dox
	rm -f  toolbox/*.mexmac
	rm -f  toolbox/*.mexmaci
	rm -f  toolbox/*.mexglx
	rm -f  toolbox/*.mexw32
	rm -f  toolbox/*.dll
	rm -f  toolbox/*.pdb
	rm -f  $(NAME)-*.tar.gz

.PHONY: dist
dist: d := $(notdir $(CURDIR)) 
dist: clean TIMESTAMP 
	tar chzvf $(DIST).tar.gz ../$(d)    	                      \
	  --exclude '.git'                                            \
	  --exclude 'bin'                                             \
	  --exclude 'dox'                                             \
	  --exclude 'results'                                         \
	  --exclude 'toolbox/*.mexmac'                                \
	  --exclude 'toolbox/*.mexmaci'                               \
	  --exclude 'toolbox/*.mexglx'                                \
	  --exclude 'toolbox/*.mexw32'                                \
	  --exclude 'toolbox/*.dll'                                   \
	  --exclude 'toolbox/*.pdb'                                   \
	  --exclude 'figures/*.png'                                   \
	  --exclude '$(DIST)-*'                                       \
	  --exclude '$(BINDIST)-*'

.PHONY: bindist
bindist: d := $(notdir $(CURDIR)) 
bindist: all doc clean TIMESTAMP
	tar chzvf $(BINDIST).tar.gz ../$(d)                           \
	  --exclude '.git'                                            \
	  --exclude 'results'                                         \
	  --exclude 'doc/figures/demo/*.eps'                          \
	  --exclude '*.pdb'                                           \
	  --exclude '$(DIST)-*'                                       \
	  --exclude '$(BINDIST)-*'

.PHONY: autorights
autorights: distclean
	autorights \
	  . \
	  --recursive    \
	  --template gpl \
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
