CODEDIR = code-doc
BLDDIR = build

all: manual.pdf $(CODEDIR)

$(CODEDIR):
	doxygen doxygen.cfg
	$(MAKE) -C $(CODEDIR)/latex
	cp $(CODEDIR)/latex/refman.pdf code.pdf

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
	rm -rf manual.pdf

.PHONY: $(CODEDIR) clean
