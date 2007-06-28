# file:        Makefile
# author:      Andrea Vedaldi
# description: Build everything

DIST=vlfeat
VER=alpha-1
BINDIST=$(DIST)-$(VER)-$(ARCH)

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------

# Determine on the flight the system we are running on
Darwin_PPC_ARCH  := mac
Darwin_i386_ARCH := mci
Linux_i386_ARCH  := glx
ARCH        := $($(shell echo `uname -sp` | tr \  _)_ARCH)

mac_CFLAGS       := -O -I.  -pedantic -Wall -std=c99
mac_MEX_CFLAGS   := CFLAGS='$$CFLAGS $(mac_CFLAGS)'
mac_MEX_SUFFIX   := mexmac

mci_CFLAGS       := -O -I.  -pedantic -Wall -std=c99
mci_MEX_CFLAGS   := CFLAGS='$$CFLAGS $(mac_CFLAGS)'
mci_MEX_SUFFIX   := mexmaci

glx_CFLAGS       := -O -I. -pedantic -Wall -std=c99
glx_MEX_CFLAGS   := CFLAGS='$$CFLAGS $(glx_CFLAGS)'
glx_MEX_SUFFIX   := mexglx

MEX_SUFFIX   := $($(ARCH)_MEX_SUFFIX)
MEX_CFLAGS   := $($(ARCH)_MEX_CFLAGS)

.PHONY : all
all : vl/libvl.a src/sift src/mser

# --------------------------------------------------------------------
#                                                        Build libvl.a
# --------------------------------------------------------------------

lib_src = $(wildcard vl/*.c)
lib_obj = $(lib_src:.c=.o)
lib_dep = $(lib_src:.c=.d)

vl/%.d : vl/%.c
	@echo "   D '$<' ==> '$@'"
	@cc -M -MT 'lib/$*.o lib/$*.d' $< -MF $@

vl/libvl.a : $(lib_obj)
	ar rcs $@ $^

ifeq ($(filter doc dox clean distclean info, $(MAKECMDGOALS)),)
include $(lib_dep) 
endif

# --------------------------------------------------------------------
#                                                       Build binaries
# --------------------------------------------------------------------

CFLAGS += -I. -Lvl

src/sift             : src/sift-driver.c      vl/libvl.a
src/mser             : src/mser-driver.c      vl/libvl.a
src/test_stringop    : src/test_sringop.c     vl/libvl.a
src/test_getopt_long : src/test_getopt_long.c vl/libvl.a

# --------------------------------------------------------------------
#                                                      Build MEX files
# --------------------------------------------------------------------

mex_src := mser
mex_tgt := $(addprefix toolbox/, $(addsuffix .$(MEX_SUFFIX), $(mex_src)))

.PHONY: all-mex
all-mex : $(mex_tgt)

toolbox/%.$(MEX_SUFFIX) : toolbox/%.mex.c
	mex $(MEX_FLAGS) $(MEX_CFLAGS) $(MEX_CLIBS) $< -outdir 'toolbox' ;
	@mv toolbox/$*.mex.$(MEX_SUFFIX) toolbox/$*.$(MEX_SUFFIX)

# --------------------------------------------------------------------
#                                                                  Doc
# --------------------------------------------------------------------

.PHONY: dox
dox: doxygen.conf
	(test -e dox || mkdir dox)
	doxygen doxygen.conf
	touch dox

# --------------------------------------------------------------------
#                                                       Clean and dist
# --------------------------------------------------------------------

.PHONY: clean
clean:
	rm -f $(lib_obj)
	rm -f $(lib_dep)
	find . -name '*~'           -exec rm -f \{\} \;
	find . -name '.DS_Store'    -exec rm -f \{\} \;
	find . -name '.gdb_history' -exec rm -f \{\} \;
	rm -f src/*.o

.PHONY: distclean
distclean: clean
	rm -f toolbox/*.mexmac
	rm -f toolbox/*.mexmaci
	rm -f toolbox/*.mexglx
	rm -f src/sift
	rm -f src/mser
	rm -f src/test_getopt_long
	rm -f src/test_stringop
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

