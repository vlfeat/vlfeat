# file: doc.mak
# description: Makefile submodule to build the documentation
# author: Andrea Vedaldi

# Copyright (C) 2013-14 Andrea Vedaldi.
# Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
# All rights reserved.
#
# This file is part of the VLFeat library and is made available under
# the terms of the BSD license (see the COPYING file).

info: doc-info
clean: doc-clean
distclean: doc-distclean
archclean: doc-archclean

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

DOXYGEN    ?= doxygen

PDFLATEX   ?= pdflatex
FIG2DEV    ?= fig2dev
CONVERT    ?= convert

PYTHON     ?= python
GROFF      ?= groff
TIDY       ?= tidy

MV         ?= mv
SED        ?= sed

# 95 DPI makes a letter page 808 pixels large
screen_dpi := 95

doc: doc-api doc-man doc-matlab

# generate $(doc-dir) target
$(eval $(call gendir, doc, \
	 doc \
	 doc/build doc/build/api doc/build/man doc/build/figures \
         doc/demo doc/figures doc/matlab doc/api))

# generate results-dir target
$(eval $(call gendir, results, results))

# --------------------------------------------------------------------
#                                                               MATLAB
# --------------------------------------------------------------------

.PHONY: doc-deep

doc-matlab : toolbox/doc/matlab/helptoc.xml
doc-matlab : toolbox/doc/matlab/helpsearch-v3/segments.gen

ifdef MATLAB_PATH
ifeq ($(call gt,$(MATLAB_VER),80500),)
doc-matlab : toolbox/doc/matlab/helpsearch-v3/segments.gen
else
doc-matlab : toolbox/doc/matlab/helpsearch-v2/segments.gen
endif
endif

# use MATLAB to create the figures for the tutorials
doc-deep: $(doc-dir) $(results-dir) all
	cd toolbox ; \
	VL_DEMO_PRINT=y $(MATLAB_EXE) \
	    -$(ARCH) -nodesktop -nosplash \
	    -r "clear mex;vl_setup demo;vl_demo;exit"
	@echo "Trimming images ..."
	find doc/demo -name "*.jpg" -exec $(CONVERT) -trim "{}" "{}" ";"
	@echo "Done trimming"
	$(MAKE) doc

# make documentation searchable in MATLAB
m_doc_lnk := toolbox/doc

toolbox/doc/matlab/helptoc.xml : doc/build/matlab/helptoc.xml doc/index.html
	ln -sf ../doc toolbox/doc
	cp -v "$<" "$@"

toolbox/doc/matlab/helpsearch-v3/segments.gen : toolbox/doc/matlab/helptoc.xml
	$(MATLAB_EXE) -$(ARCH) -nodisplay -nosplash -r \
	"addpath('toolbox');builddocsearchdb('toolbox/doc/matlab/');exit"

toolbox/doc/matlab/helpsearch-v2/segments.gen : toolbox/doc/matlab/helptoc.xml
	$(MATLAB_EXE) -$(ARCH) -nodisplay -nosplash -r \
	"addpath('toolbox');builddocsearchdb('toolbox/doc/matlab/');exit"

# --------------------------------------------------------------------
#                                                                 MDoc
# --------------------------------------------------------------------

doc-matlab: doc/build/matlab/mdoc.html

#
# Use mdoc.py to create the toolbox documentation that will be
# embedded in the website.
#

doc/build/matlab/mdoc.html doc/build/matlab/helptoc.xml: \
	$(m_src) \
	docsrc/mdoc.py #make/doc.mak
	$(PYTHON) docsrc/mdoc.py toolbox doc/build/matlab \
	          --format=web \
	          --exclude='noprefix/.*' \
                  --exclude='xtune/.*' \
                  --exclude='xtest/.*' \
	          --exclude='.*/vl_test_.*' \
	          --exclude='.*/vl_demo_.*' \
	          --exclude='.*/vl_tune_.*' \
	          --helptoc \
	          --helptoc-toolbox-name VLFeat \
	          --verbose

# --------------------------------------------------------------------
#                                            Auto-include demo m-files
# --------------------------------------------------------------------

m_demo_src=$(wildcard toolbox/demo/vl_demo_*.m)

# Convert the various man pages
doc/build/matlab/demo.xml : $(m_demo_src) $(doc-dir) make/doc.mak
	@echo "Indexing demo files -> $@"
	@( \
	  echo '<!DOCTYPE group PUBLIC ' ; \
	  echo '  "-//W3C//DTD XHTML 1.0 Transitional//EN"' ; \
	  echo '  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">' ; \
	  echo "<group>" ; \
	  for fullName in $(m_demo_src) ; \
	  do  \
	  fileName=$${fullName#toolbox/demo/} ; \
	  stem=$${fileName%.*} ; \
	  ( \
	    echo "<page id='$${stem}' name='$${stem}' title='$${stem}' hide='yes'>" ; \
	    printf "<h1>Source code for <code>%s.m</code></h1>" "$${stem}"; \
	    echo "<p>This file is located in the <code>toolbox/demo</code> folder in VLFeat package.</p>" ; \
	    echo "<precode type='matlab'>" ; \
	    echo "<include src='toolbox/demo/$${fileName}' type='text'/>" ; \
	    echo "</precode>" ; \
	    echo "</page>" \
	  ) ; \
	  done ; \
	  echo "</group>" ; \
	)  > "$@"

# --------------------------------------------------------------------
#                                                                  Man
# --------------------------------------------------------------------

doc-man: doc/build/man/xman.xml doc/build/man/xman.html
man_src := $(wildcard src/*.1) $(wildcard src/*.7)
man_tgt := $(subst src/,doc/build/man/,$(addsuffix .html,$(man_src)))

doc/build/man/index.html : $(doc-dir)

# Integrate in Webdoc
doc/build/man/xman.xml : $(man_tgt) $(doc-dir)
	@echo "Generating MAN XML webdooc document $@"
	@echo "<group>" > "$@"
	@for fullName in $(man_src) ; \
	do  \
	  fileName=$${fullName#src/} ; \
	  stem=$${fileName%.*} ; \
	  ( \
	    echo "<page id='man.$${stem}' name='$${stem}' title='$${stem}'>" ; \
	    echo "<include src='doc/build/man/$${fileName}.html'/>" ; \
	    echo "</page>" \
	  ) >> "$@" ; \
	done
	echo "</group>" >> "$@"

# Index page
doc/build/man/xman.html : $(man_tgt) $(doc-dir)
	@echo "Generating MAN HTML index page $@"
	@echo "<ul>" > "$@"
	@for fullName in $(man_src) ; \
	do  \
	  fileName=$${fullName#src/} ; \
	  stem=$${fileName%.*} ; \
	  echo "<li><a href='%pathto:man.$${stem};'>$${stem}</a></li>" >> "$@" ; \
	done
	echo "</ul>" >> "$@"

# Convert the various man pages
doc/build/man/%.html : src/% $(doc-dir)
	@echo MAN2HTML "$(@)"
	@( \
	  echo '<!DOCTYPE group PUBLIC ' ; \
	  echo '  "-//W3C//DTD XHTML 1.0 Transitional//EN"' ; \
	  echo '  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">' ; \
	  echo "<group>" ; \
	  $(GROFF) -mandoc -Thtml < "$<" | \
	    $(TIDY) -asxhtml 2>/dev/null | \
	    sed -e '1,/<body>/ d' -e '/<\/body>/,$$ d' ; \
	  echo "</group>" \
	)  > "$@"

# --------------------------------------------------------------------
#                                       FIG and SVG figures and images
# --------------------------------------------------------------------

doc_fig_src := $(wildcard docsrc/figures/*.fig)
doc_svg_src := $(wildcard docsrc/figures/*.svg)
doc_fig_tgt += \
$(subst docsrc/,doc/,$(doc_fig_src:.fig=.png)) \
$(subst docsrc/,doc/,$(doc_svg_src:.svg=.png))

.PRECIOUS: doc/build/figures/%.pdf
.PRECIOUS: doc/build/figures/%.tex

doc/figures/%.png : doc/build/figures/%.pdf
	$(call C,CONVERT) -units PixelsPerInch -density $(screen_dpi) -resample $(screen_dpi) -trim "$<" "$@"

doc/figures/%.png : docsrc/figures/%.svg
	$(call C,CONVERT) -units PixelsPerInch -density $(screen_dpi) -resample $(screen_dpi) -trim "$<" "$@"

# Fig
doc/build/figures/%-raw.tex : docsrc/figures/%.fig $(doc-dir)
	$(call C,FIG2DEV) -L pdftex_t -p doc/build/figures/$*-raw.pdf "$<" "$@"

doc/build/figures/%-raw.pdf : docsrc/figures/%.fig $(doc-dir)
	$(call C,FIG2DEV) -L pdftex "$<" "$@"

doc/build/figures/%.pdf doc/build/figures/%.aux doc/build/figures/%.log : \
  doc/build/figures/%.tex doc/build/figures/%-raw.tex doc/build/figures/%-raw.pdf $(doc-dir)
	$(call C,PDFLATEX) -shell-escape -interaction=batchmode -output-directory="$(dir $@)" "$<" 2>/dev/null

doc/build/figures/%.tex : $(doc-dir)
	@echo GEN "$(@)"
	@/bin/echo '\documentclass[landscape]{article}'                 >$@
	@/bin/echo '\usepackage[paper=a2paper,margin=0pt]{geometry}'	>>$@
	@/bin/echo '\usepackage{graphicx,color}'			>>$@
	@/bin/echo '\begin{document}'					>>$@
	@/bin/echo '\pagestyle{empty}'					>>$@
	@/bin/echo '\input{doc/build/figures/$(*)-raw.tex}'		>>$@
	@/bin/echo '\end{document}'					>>$@

# --------------------------------------------------------------------
#                                                              Doxygen
# --------------------------------------------------------------------

doc-api: doc/api/index.html

doc/api/index.html: docsrc/doxygen.conf docsrc/vlfeat.bib VERSION \
  $(dll_src) $(dll_hdr) $(doc_fig_tgt) toolbox/mexutils.h \
  doc/build/doxygen_header.html doc/build/doxygen_footer.html
	ln -sf docsrc/vlfeat.bib vlfeat.bib
	$(DOXYGEN) $< 2>&1 | sed -e 's/Warning:/warning: /g'
	rm vlfeat.bib
	cp -fv docsrc/doxygen.css doc/api/doxygen.css

# --------------------------------------------------------------------
#                                                               Webdoc
# --------------------------------------------------------------------

webdoc_src = $(wildcard docsrc/*.xml) $(wildcard docsrc/*.html) $(wildcard docsrc/tutorials/*.html)

doc: doc/index.html doc/vlfeat.css doc/pygmentize.css $(doc_fig_tgt)

# prebuild to generate doxygen header and footer
doc/build/doxygen_header.html doc/build/doxygen_footer.html: $(webdoc_src) $(doc-dir)
	VERSION=$(VER) $(PYTHON) docsrc/webdoc.py \
	     --outdir=doc/build/ \
             --verbose \
	     docsrc/vlfeat-website-preproc.xml
	cat doc/build/api/index.html | \
	    sed -n '/<!-- Doc Here -->/q;p'  > doc/build/doxygen_header.html
	echo '<div id="top">' >> doc/build/doxygen_header.html
	cat doc/build/api/index.html | \
	    sed -n '/<!-- Doc Here -->/,$$p' > doc/build/doxygen_footer.html

doc/vlfeat.css : docsrc/vlfeat.css
	cp -fv "$<" "$@"

doc/pygmentize.css : docsrc/pygmentize.css
	cp -fv "$<" "$@"

# build: this is the last step integrating all documentation
doc/index.html: $(webdoc_src) $(doc-dir) \
 doc/api/index.html \
 doc/build/matlab/mdoc.html \
 doc/build/man/xman.xml \
 doc/build/man/xman.html \
 doc/build/matlab/demo.xml \
 docsrc/webdoc.py \
 $(html_src)
	cp doc/api/index.html doc/api/index.html.bak
	VERSION=$(VER) $(PYTHON) docsrc/webdoc.py \
             --outdir=doc \
	     --verbose \
	     --indexfile=doc/index.txt \
	     --doxytag=doc/doxygen.tag \
	     --doxydir=api \
	     docsrc/vlfeat-website.xml
	mv doc/api/index.html.bak doc/api/index.html
	rsync -r docsrc/images doc/

# --------------------------------------------------------------------
#                                               Maintenance and others
# --------------------------------------------------------------------

.PHONY: doc-clean, doc-archclean, doc-distclean
no_dep_targets += doc-clean doc-archclean doc-distclean

VERSION: vl/generic.h
	echo "$(VER)" > VERSION

doc-clean:
	rm -rf doc/build

doc-archclean:
doc-distclean:
	rm -f docsrc/*.pyc
	rm -rf doc
	rm -rf toolbox/doc

# --------------------------------------------------------------------
#                                                       Debug Makefile
# --------------------------------------------------------------------

.PHONY: doc-info
doc-info :
	$(call echo-title,Documentation)
	$(call dump-var,man_src)
	$(call dump-var,fig_src)
	$(call dump-var,demo_src)
	$(call dump-var,pdf_tgt)
	$(call dump-var,eps_tgt)
	$(call dump-var,png_tgt)
	$(call dump-var,jpg_tgt)
	$(call dump-var,man_tgt)
	$(call dump-var,DOXYGEN)
	@echo

# --------------------------------------------------------------------
#                                                        Xcode Support
# --------------------------------------------------------------------

.PHONY: dox-
dox- : dox

.PHONY: dox-clean
dox-clean:

# Local variables:
# mode: Makefile
# End:
