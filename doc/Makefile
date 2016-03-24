CODEDIR = code-doc
BLDDIR = build

all: manual.pdf delta.pdf $(CODEDIR)

$(CODEDIR):
	doxygen doxygen.cfg
	$(MAKE) -C $(CODEDIR)/latex
	cp $(CODEDIR)/latex/refman.pdf code.pdf

delta.pdf: delta.tex
	mkdir -p $(BLDDIR)
	pdflatex -output-directory $(BLDDIR) delta.tex
	pdflatex -output-directory $(BLDDIR) delta.tex
	pdflatex -output-directory $(BLDDIR) delta.tex
	cp $(BLDDIR)/delta.pdf delta.pdf

manual.pdf: manual.tex
	mkdir -p $(BLDDIR)
	pdflatex -output-directory $(BLDDIR) manual.tex
	pdflatex -output-directory $(BLDDIR) manual.tex
	pdflatex -output-directory $(BLDDIR) manual.tex
	cp $(BLDDIR)/manual.pdf manual.pdf

clean:
	rm -rf $(CODEDIR)
	rm -rf $(BLDDIR)
	rm -rf code.pdf
	rm -rf delta.pdf
	rm -rf manual.pdf

.PHONY: $(CODEDIR) manual.pdf delta.pdf clean
