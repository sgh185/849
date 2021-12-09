LATEX2E = xelatex 
BIBTEX = bibtex
DVIPS = dvips
XDVI = xdvi
PS2PDF = ps2pdf -dMaxSubsetPct=100 -dCompatibilityLevel=1.2 -dSubsetFonts=true -dEmbedAllFonts=true  -dPDFSETTINGS=/prepress

TEXDOCS = paper.tex 
WORDDOCS= 
DVIDOCS = $(TEXDOCS:.tex=.dvi)
ERASEABLEPSDOCS=$(TEXDOCS:.tex=.ps)
PSDOCS  = $(ERASEABLEPSDOCS) $(WORDDOCS:.doc=.ps) 
PDFDOCS = $(PSDOCS:.ps=.pdf)

all: $(PDFDOCS)  

all: paper.pdf 

%.pdf: %.tex 
	$(LATEX2E) $<

force:
	$(LATEX2E) paper
	$(LATEX2E) paper

bib: 
	$(LATEX2E) paper
	$(LATEX2E) paper
	-$(BIBTEX)  paper
	$(LATEX2E) paper
	$(LATEX2E) paper

clean:
	-rm -f *.out *.blg *.dvi *.log *.bbl *.aux paper.pdf abstract.pdf $(ERASEABLEPSDOCS) 
