#############################################################################
##                                                                         ##
##                               Makefile.am                               ##
##                                                                         ##
##                        Matching Pursuit Library                         ##
##                                                                         ##
## Rémi Gribonval                                                          ##
## Sacha Krstulovic                                        Mon Feb 21 2005 ##
## ----------------------------------------------------------------------- ##
##                                                                         ##
##  Copyright (C) 2005 IRISA                                               ##
##                                                                         ##
##  This program is free software; you can redistribute it and/or modify   ##
##  it under the terms of the GNU General Public License as published by   ##
##  the Free Software Foundation; either version 2, or (at your option)    ##
##  any later version.                                                     ##
##                                                                         ##
##  This program is distributed in the hope that it will be useful,        ##
##  but WITHOUT ANY WARRANTY; without even the implied warranty of         ##
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          ##
##  GNU General Public License for more details.                           ##
##                                                                         ##
##  You should have received a copy of the GNU General Public License      ##
##  along with this program; if not, write to the Free Software            ##
##  Foundation, Inc., 59 Temple Place - Suite 330,                         ##
##  Boston, MA 02111-1307, USA.                                            ##
##                                                                         ##
#############################################################################
##
## SVN log:
##
##   $Author: sacha $
##   $Date$
##   $Revision$
##

EXTRA_DIST = doxygen_config.in html latex_src $(PACKAGE)-$(VERSION)-refman.ps $(PACKAGE)-$(VERSION)-refman.pdf
DISTCLEANFILES = doxygen_config

all: html $(PACKAGE)-$(VERSION)-refman.ps $(PACKAGE)-$(VERSION)-refman.pdf

html:
	@if [ "$(DOXYGEN)" != "no" ]; \
	then doxygen doxygen_config;  \
	else \
	  echo "Doxygen was not found.";        \
	  if test -d '$(top_srcdir)/doc/refman/html'; \
	  then cp -rf $(top_srcdir)/doc/refman .; echo "An older reference manual has been re-included."; \
	  else echo "No reference manual can be re-generated."; \
	  fi; \
	fi;

latex_src/refman.tex: html

latex_src/refman.dvi: latex_src/refman.tex
	@if [ "$(LATEX)" != "no" ]; then \
	  if test -d 'latex_src'; then \
	    cd latex_src/; latex refman; latex refman; bibtex refman; \
	    latex refman; latex refman; \
	  else echo "No latex source found for the reference manual. Can't compile it."; \
	  fi; \
	else echo "Latex was not found. Can't compile the reference manual."; \
	fi;

latex_src/refman.ps: latex_src/refman.dvi
	@if [ "$(DVIPS)" != "no" ]; \
	then cd latex_src/; dvips refman -o; \
	else echo "Dvips was not found. Can't make a postscript reference manual."; \
	fi;

$(PACKAGE)-$(VERSION)-refman.ps: latex_src/refman.ps
	cp -f latex_src/refman.ps  ./$(PACKAGE)-$(VERSION)-refman.ps;

$(PACKAGE)-$(VERSION)-refman.pdf: $(PACKAGE)-$(VERSION)-refman.ps
	@if [ "$(PS2PDF)" != "no" ]; \
	then ps2pdf ./$(PACKAGE)-$(VERSION)-refman.ps ./$(PACKAGE)-$(VERSION)-refman.pdf; \
	else echo "Ps2pdf was not found. Can't make a pdf reference manual."; \
	fi;

clean:
	@if [ "$(DOXYGEN)" = "no" ]; \
	then  rm -rf ./latex_src/*.{aux,toc,bbl,blg,log,dvi} ./latex_src/refman.ps; \
	else  rm -rf ./latex_src ./html; \
	fi
	rm -f ./$(PACKAGE)-$(VERSION)-refman.ps ./$(PACKAGE)-$(VERSION)-refman.pdf

## DIST HOOK:
dist-hook:
	rm -rf `find $(distdir) -name '.svn'`;
	rm -rf $(distdir)/latex_src/*.{aux,toc,bbl,blg,log,dvi} $(distdir)/latex_src/refman.ps
