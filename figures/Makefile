# file:        Makefile
# author:      Andrea Vedaldi
# description: Convert from XFIG to LATEX

fig_sources := $(wildcard *.fig)
pdf_targets := $(fig_sources:.fig=.pdf)
png_targets := $(fig_sources:.fig=.png)

%.png : %.pdf
	convert -trim -density 100 $< $@

%.pdf : %.tex %-raw.tex %-raw.pdf
	pdflatex $*.tex
	rm $*.log $*.aux

%-raw.tex : %.fig
	fig2dev -L pdftex_t -p $*-raw.pdf $< $@

%-raw.pdf : %.fig
	fig2dev -L pdftex $< $@

%.tex : Makefile
	echo -n -e "		           \n\
	\\\\documentclass{article}         \n\
	\\\\usepackage{graphicx,color}     \n\
	\\\\begin{document}	           \n\
	\\\\pagestyle{empty}               \n\
	\\\\input{$*-raw.tex}              \n\
	\\\\end{document}	           \n" > $@

.PHONY: all
all: $(png_targets)

.PHONY: clean
clean:
	rm -f $(pdf_targets) *-raw.tex *-raw.pdf *.tex *.log *.aux *.bak

.PHONY: distclean
distclean: clean
	rm -f $(png_targets)
