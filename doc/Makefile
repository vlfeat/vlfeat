# file:        Makefile
# author:      Andrea Vedaldi
# description: LaTeX --> pdf, html

all: vlfeat.pdf html/vlfeat.html

aux/vlfeat.tex : vlfeat.tex
	mkdir -p aux
	ln -sf ../figures      aux/figures
	ln -sf ../vlfeat.tex   aux/vlfeat.tex
	ln -sf ../vl.cfg       aux/vl.cfg
	ln -sf ../vlfeat.bbl   aux/vlfeat.bbl

html/vlfeat.html : vlfeat.tex aux/vlfeat.tex vl.cfg
	make -C figures all
	mkdir -p html
	cd aux ; htlatex $< "vl,html,2" "" "-d../html/"
	mkdir -p html/figures
	rsync figures/*.png html/figures

vlfeat.pdf : vlfeat.tex aux/vlfeat.tex
	make -C figures all
	cd aux ; pdflatex -shell-escape $<
	cd aux ; pdflatex -shell-escape $<
	cd aux ; pdflatex -shell-escape $<
	cat < aux/vlfeat.pdf > vlfeat.pdf

.PHONY: fig
fig:
	make -C ../figures all

.PHONY: clean
clean:
	make -C figures clean
	rm -rf aux

.PHONY: distclean
distclean: clean
	make -C figures distclean
	rm -rf html vlfeat.pdf

