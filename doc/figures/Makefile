# file:        Makefile
# author:      Andrea Vedaldi
# description: Fig + LaTeX --> png, eps, pdf

fig_sources  := $(wildcard *.fig)
demo_sources := $(wildcard demo/*.eps)

pdf_targets  := $(fig_sources:.fig=.pdf)
eps_targets  := $(fig_sources:.fig=.eps)
png_targets  := $(fig_sources:.fig=.png)
demo_targets := $(demo_sources:.eps=.pdf)

%.png : %.dvi
	dvipng -D 75 -T tight -o $@ $<

%.eps : %.dvi
	dvips -E -o $@ $<

%.pdf : %.eps
	epstopdf --outfile=$@ $<

%-raw.tex : %.fig
	fig2dev -L pstex_t -p $*-raw.ps $< $@

%-raw.ps : %.fig
	fig2dev -L pstex $< $@

%.dvi %.aux %.log :  %.tex %-raw.tex %-raw.ps
	latex $*.tex
	rm $*.log $*.aux

%.tex :
	echo -n -e "		                      \n\
	\\\\documentclass[landscape]{article}         \n\
	\\\\usepackage[margin=0pt]{geometry}          \n\
	\\\\usepackage{graphicx,color}                \n\
	\\\\begin{document}	                      \n\
	\\\\pagestyle{empty}                          \n\
	\\\\input{$*-raw.tex}                         \n\
	\\\\end{document}	                      \n" > $@

demo/%.pdf : demo/%.eps
	epstopdf --outfile=$@ $<

.PHONY: all
all: $(png_targets) $(eps_targets) $(pdf_targets) $(demo_targets)

.PHONY: info
info:
	@echo fig_targets =
	@echo $(fig_targets) | rs -j
	@echo demo_targets =
	@echo $(demo_targets) | rs -j

.PHONY: clean
clean:
	rm -f  *-raw.tex *-raw.ps *.tex *.log *.aux *.bak *.dvi
	rm -f $(pdf_targets)
	rm -f $(eps_targets) 
	rm -f $(png_targets) 
	rm -f $(demo_targets)

.PHONY: distclean
distclean: clean
	rm -f $(png_targets)
	rm -f $(demo_sources)
