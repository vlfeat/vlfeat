# file:        Makefile
# author:      Andrea Vedaldi
# description: Build everything

DIST=vlfeat
VER=alpha-1

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------

# generic flags
CFLAGS           += -O0 -I. -pedantic -Wall -std=c99 -g
CFLAGS           += -Wno-variadic-macros
LDFLAGS          +=

# Determine on the flight the system we are running on
Darwin_PPC_ARCH    := mac
Darwin_i386_ARCH   := mci
Linux_i386_ARCH    := glx
Linux_unknown_ARCH := glx

ARCH             := $($(shell echo `uname -sp` | tr \  _)_ARCH)

mac_BINDIR       := bin/mac
mac_CFLAGS       := -O0 -I.  -pedantic -Wall -std=c99 -g
mac_LDFLAGS      := -DBIG_ENDIAN
mac_MEX_CFLAGS   := CFLAGS='$$CFLAGS $(mac_CFLAGS)' -L$(mac_BINDIR) -lvl
mac_MEX_SUFFIX   := mexmac

mci_BINDIR       := bin/maci
mci_CFLAGS       := -DLITTLE_ENDIAN
mci_LDFLAGS      :=
mci_MEX_CFLAGS   := CFLAGS='$$CFLAGS $(mci_CFLAGS)' -L$(mci_BINDIR) -lvl
mci_MEX_SUFFIX   := mexmaci

glx_BINDIR       := bin/glx
glx_CFLAGS       := -DLITTLE_ENDIAN
glx_LDFLAGS      := -lm
glx_MEX_CFLAGS   := CFLAGS='$$CFLAGS $(glx_CFLAGS)' -L$(glx_BINDIR) -lvl
glx_MEX_SUFFIX   := mexglx

CFLAGS           += $($(ARCH)_CFLAGS)
LDFLAGS          += $($(ARCH)_LDFLAGS)
MEX_SUFFIX       := $($(ARCH)_MEX_SUFFIX)
MEX_CFLAGS       := $($(ARCH)_MEX_CFLAGS)
BINDIR           := $($(ARCH)_BINDIR)
BINDIST          := $(DIST)-$(VER)-$(ARCH)

.PHONY : all
all : all-lib all-bin all-mex

# this creates the directory hiearchy 
$(BINDIR) $(BINDIR)/.lib_bits $(BINDIR)/.lib_bits/.stamp:
	mkdir -p $(BINDIR)/.lib_bits
	touch $@

# --------------------------------------------------------------------
#                                                        Build libvl.a
# --------------------------------------------------------------------
# We place the object and dependency files in $(BINDIR)/.lib_bits/ and
# the library in $(BINDIR)/libvl.a.

lib_src := $(wildcard vl/*.c)
lib_obj := $(notdir $(lib_src))
lib_obj := $(addprefix $(BINDIR)/.lib_bits/, $(lib_obj:.c=.o))
lib_dep := $(lib_obj:.o=.d)

# create library libvl.a
.PHONY: all-lib
all-lib: $(BINDIR)/libvl.a

$(BINDIR)/.lib_bits/%.o : vl/%.c $(BINDIR)/.lib_bits/.stamp
	@echo "   CC '$<' ==> '$@'"
	@cc $(CFLAGS) -c $< -o $@

$(BINDIR)/.lib_bits/%.d : vl/%.c $(BINDIR)/.lib_bits/.stamp
	@echo "   D  '$<' ==> '$@'"
	@cc -M -MT '$(BINDIR)/.lib_bits/$*.o $(BINDIR)/.lbi_bits/$*.d' $< -MF $@

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

mex_src := $(notdir $(basename $(basename $(wildcard toolbox/*.mex.c))))
mex_tgt := $(addprefix toolbox/, $(addsuffix .$(MEX_SUFFIX), $(mex_src)))

.PHONY: all-mex
all-mex : $(mex_tgt)

toolbox/%.$(MEX_SUFFIX) : toolbox/%.mex.c toolbox/mexutils.h $(BINDIR)/libvl.a
	@echo "   MX '$<' ==> '$@'"
	@mex $(MEX_FLAGS) $(MEX_CFLAGS) $(MEX_CLIBS) $< -outdir 'toolbox' ;
	@mv toolbox/$*.mex.$(MEX_SUFFIX) toolbox/$*.$(MEX_SUFFIX)

# --------------------------------------------------------------------
#                                                                  Doc
# --------------------------------------------------------------------

.PHONY: dox
dox: doxygen.conf
	make -C figures all
	(test -e dox || mkdir dox)
	doxygen doxygen.conf
	touch dox
# --------------------------------------------------------------------
#                                                       Clean and dist
# --------------------------------------------------------------------

.PHONY: clean
clean:
	make -C figures clean
	rm -f $(lib_obj)
	rm -f $(lib_dep)
	find . -name '*~'           -exec rm -f \{\} \;
	find . -name '.DS_Store'    -exec rm -f \{\} \;
	find . -name '.gdb_history' -exec rm -f \{\} \;
	rm -f src/*.o

.PHONY: distclean
distclean: clean
	make -C figures distclean
	rm -f vl/libvl.a
	rm -f toolbox/*.mexmac
	rm -f toolbox/*.mexmaci
	rm -f toolbox/*.mexglx
	rm -rf bin
	rm -rf $(DIST)-*

.PHONY: dist
dist: distclean
	echo Version $(VER) >TIMESTAMP
	echo Archive created on `date` >>TIMESTAMP
	tar chzvf $(DIST)-$(VER).tar.gz ../$(notdir $(CURDIR))

.PHONY: bindist
bindist: all doc
	test -e $(BINDIST) || mkdir $(BINDIST)
	cp README toolbox/*.m vlutil.html mex/*.$(MEX_SUFFIX) $(BINDIST)
	cd $(BINDIST) ; strip -S *.$(MEX_SUFFIX) $(notdir $(LIB))	
	echo Version $(VER) >$(BINDIST)/TIMESTAMP
	echo Package created on `date` >>$(BINDIST)/TIMESTAMP
	tar cvzf $(BINDIST).tar.gz $(BINDIST)

.PHONY: autorights
autorights: distclean
	autorights \
	  . \
	  --recursive \
	  --template gpl \
	  --years 2007 \
	  --authors "Andrea Vedaldi and Brian Fulkerson" \
	  --holders "Andrea Vedaldi and Brian Fulkerson" \
	  --program "VLFeat"

# --------------------------------------------------------------------
#                                                       Debug Makefile
# --------------------------------------------------------------------

.PHONY: info
info :
	@echo "BINDIST  = $(BINDIST)"
	@echo "lib_src  = $(lib_src)"
	@echo "lib_obj  = $(lib_obj)"
	@echo "lib_dep  = $(lib_dep)"
	@echo "mex_src  = $(mex_src)"
	@echo "mex_tgt  = $(mex_tgt)"
	@echo "bin_src  = $(bin_src)"
	@echo "bin_tgt  = $(bin_tgt)"
