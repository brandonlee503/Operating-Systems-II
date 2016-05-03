LATEX	= latex -shell-escape
#BIBTEX	= bibtex
DVIPS	= dvips
DVIPDF  = dvipdft
XDVI	= xdvi -gamma 4
GH		= gv

EXAMPLES = $(wildcard *.c)
SRC	:= $(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
TRG	= $(SRC:%.tex=%.dvi)
PSF	= $(SRC:%.tex=%.ps)
PDF	= $(SRC:%.tex=%.pdf)

all: concurrency3

concurrency3: concurrency3.o
	gcc concurrency3.o -o assignment3 -pthread

concurrency3.o:
	gcc -c concurrency3.c -g

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg assignment3 *.o *.out *.log *.aux *~

.PHONY: all show clean ps pdf showps
