# file:        Makfile.doc
# author:      Andrea Vedaldi
# description: Makefile submodule to build the documentation

# AUTORIGHTS

info: doc-info
clean: doc-clean
distclean: doc-distclean
archclean: doc-archclean

# --------------------------------------------------------------------
#                                                        Configuration
# --------------------------------------------------------------------

CONVERT    ?= convert
DOXYGEN    ?= doxygen
DVIPNG     ?= dvipng
DVIPS      ?= dvips
EPSTOPDF   ?= epstopdf
FIG2DEV    ?= fig2dev
LATEX      ?= latex
PYTHON     ?= python
GROFF      ?= groff
TIDY       ?= tidy

# 95 DPI makes a letter page 808 pixels large
screen_dpi := 95

# --------------------------------------------------------------------
#                                                                Build
# --------------------------------------------------------------------

.PHONY: doc, doc-api, doc-toolbox, doc-figures, doc-man
.PHONY: doc-web, doc-demo
.PHONY: doc-clean, doc-archclean, doc-distclean
.PHONY: autorights
no_dep_targets := doc-clean doc-archclean doc-distclean

fig_src  := $(wildcard docsrc/figures/*.fig)
demo_src := $(wildcard doc/demo/*.eps)
html_src := $(wildcard docsrc/*.html)
man_src  := $(wildcard src/*.1)
man_src  += $(wildcard src/*.7)

pdf_tgt := #$(fig_src:.fig=.pdf)
eps_tgt := #$(subst docsrc/,doc/,$(fig_src:.fig=.eps))
png_tgt := $(subst docsrc/,doc/,$(fig_src:.fig=.png))
jpg_tgt := $(demo_src:.eps=.jpg)
img_tgt := $(jpg_tgt) $(png_tgt) $(pdf_tgt) $(eps_tgt)
man_tgt := $(subst src/,doc/man-src/,$(addsuffix .html,$(man_src)))

# generate doc-dir: target
$(eval $(call gendir, doc, doc doc/demo doc/figures doc/man-src))

# generate results-dir: target
$(eval $(call gendir, results, results))

VERSION: Makefile
	echo "$(VER)" > VERSION

docsrc/version.html: Makefile vl/generic.h
	echo "<code>$(VER)</code>" > docsrc/version.html

doc: doc-api doc-man doc-web
doc-man: doc/man-src/man.xml doc/man-src/man.html
doc-api: doc/api/index.html
doc-web: doc/index.html
doc-toolbox: doc/toolbox-src/mdoc.html

doc-deep: all $(doc-dir) $(results-dir)
	cd toolbox ; \
	$(MATLAB_EXE) -$(ARCH) -nodesktop -r "vl_setup('noprefix','demo');vl_demo;exit"

#
# Use webdoc.py to generate the website
#

doc/index.html: \
 doc/toolbox-src/mdoc.html \
 doc/man-src/man.xml \
 doc/man-src/man.html \
 docsrc/web.xml \
 docsrc/web.css \
 docsrc/webdoc.py \
 $(img_tgt) $(html_src)
	VERSION=$(VER) $(PYTHON) docsrc/webdoc.py --outdir=doc \
	     docsrc/web.xml --verbose
	rsync -arv docsrc/images doc
	rsync -arv docsrc/web.css doc

#
# Use mdoc.py to create the toolbox documentation that will be
# embedded in the website.
#

doc/toolbox-src/mdoc.html : $(m_src) docsrc/mdoc.py
	$(PYTHON) docsrc/mdoc.py toolbox doc/toolbox-src \
	          --format=web \
	          --exclude='noprefix/.*' \
	          --exclude='.*/vl_test_.*' \
	          --exclude='.*/vl_demo_.*' \
	          --verbose
#
# Generate C API documentation
#

doc/doxygen_header.html doc/doxygen_footer.html: doc/index.html
	cat doc/api/index.html | \
	sed -n '/<!-- Doc Here -->/q;p'  > doc/doxygen_header.html
	cat doc/api/index.html | \
	sed -n '/<!-- Doc Here -->/,$$p' > doc/doxygen_footer.html

doc/api/index.html: docsrc/doxygen.conf VERSION docsrc/version.html  \
  $(dll_src) $(dll_hdr) $(img_tgt) toolbox/mexutils.h                \
  doc/doxygen_header.html doc/doxygen_footer.html
	#$(call C,DOXYGEN) $<
	$(DOXYGEN) $< 2>&1 | sed -e 's/Warning:/warning: /g'

#
# Generate Man documentation
#

doc/man-src/man.xml : $(man_tgt) $(doc-dir)
	echo "<group>" > "$@"
	for fullName in $(man_src) ; \
	do  \
	  fileName=$${fullName#src/} ; \
	  stem=$${fileName%.*} ; \
	  ( \
	    echo "<page id='man.$${stem}' name='$${stem}' title='$${stem}'>" ; \
	    echo "<include src='doc/man-src/$${fileName}.html'/>" ; \
	    echo "</page>" \
	  ) >> "$@" ; \
	done
	echo "</group>" >> "$@"

doc/man-src/man.html : $(man_tgt) $(doc-dir)
	echo "<ul>" > "$@"
	for fullName in $(man_src) ; \
	do  \
	  fileName=$${fullName#src/} ; \
	  stem=$${fileName%.*} ; \
	  echo "<li><a href='%pathto:man.$${stem};'>$${stem}</a></li>" >> "$@" ; \
	done
	echo "</ul>" >> "$@"

doc/man-src/%.html : src/% $(doc-dir)
	( \
	  echo '<!DOCTYPE group PUBLIC ' ; \
	  echo '  "-//W3C//DTD XHTML 1.0 Transitional//EN"' ; \
	  echo '  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">' ; \
	  echo "<group>" ; \
	  $(GROFF) -mandoc -Thtml < "$<" | \
	    $(TIDY) -asxhtml | \
	    sed -e '1,/<body>/ d' -e '/<\/body>/,$$ d' ; \
	  echo "</group>" \
	)  > "$@"

doc/man-src/index.html : $(doc-dir)

#
# Convert figure formats
#

doc-figures: $(img_tgt)

doc/figures/%.png : doc/figures/%.dvi
	$(call C,DVIPNG) -D $(screen_dpi) -T tight -o $@ $<

doc/figures/%.eps : doc/figures/%.dvi
	$(call C,DVIPS) -E -o $@ $<

doc/figures/%-raw.tex : docsrc/figures/%.fig
	$(call C,FIG2DEV) -L pstex_t -p doc/figures/$*-raw.ps $< $@

doc/figures/%-raw.ps : docsrc/figures/%.fig
	$(call C,FIG2DEV) -L pstex $< $@

doc/figures/%.dvi doc/figures/%.aux doc/figures/%.log :              \
  doc/figures/%.tex doc/figures/%-raw.tex doc/figures/%-raw.ps $(doc-dir)
	$(LATEX) -output-directory=$(dir $@) $<

doc/figures/%.tex : $(doc-dir)
	@$(print-command GEN, $@)
	@/bin/echo '\documentclass[landscape]{article}' >$@
	@/bin/echo '\usepackage[margin=0pt]{geometry}' >>$@
	@/bin/echo '\usepackage{graphicx,color}'       >>$@
	@/bin/echo '\begin{document}'                  >>$@
	@/bin/echo '\pagestyle{empty}'                 >>$@
	@/bin/echo '\input{doc/figures/$(*)-raw.tex}'  >>$@
	@/bin/echo '\end{document}'	               >>$@

doc/demo/%.jpg : doc/demo/%.eps
	$(call C,CONVERT) -density 400 -resample $(screen_dpi)  "$<" -colorspace rgb jpg:$@

doc/demo/%.png : doc/demo/%.eps
	$(call C,CONVERT) -density 400 -resample $(screen_dpi)  "$<" -colorspace rgb png:$@

doc/%.pdf : doc/%.eps
	$(call C,EPSTOPDF) --outfile=$@ $<

#
# Other documentation related targets
#

doc-clean:
doc-archclean:
doc-distclean:
	rm -f  docsrc/*.pyc
	rm -rf doc

# --------------------------------------------------------------------
#                                                       Debug Makefile
# --------------------------------------------------------------------

.PHONY: doc-info
doc-info :
	$(call echo-title,Documentation)
	$(call dump-var,man_src)
	$(call dump-var,fig_src)
	$(call dump-var,demo_src)
	$(call dump-var,bin_src)
	$(call dump-var,bin_tgt)
	$(call dump-var,bin_dep)
	$(call dump-var,pdf_tgt)
	$(call dump-var,eps_tgt)
	$(call dump-var,png_tgt)
	$(call dump-var,jpg_tgt)
	$(call dump-var,man_tgt)
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
