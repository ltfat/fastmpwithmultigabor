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
##   $Author$
##   $Date$
##   $Revision$
##

EXTRA_DIST = latex_src $(PACKAGE)-$(VERSION)-userman.ps $(PACKAGE)-$(VERSION)-userman.pdf

all: $(PACKAGE)-$(VERSION)-userman.ps $(PACKAGE)-$(VERSION)-userman.pdf

latex_src/userman.dvi: $(top_srcdir)/doc/userman/latex_src/userman.tex
	cp -rf $(top_srcdir)/doc/userman/latex_src/ ./;  \
	if test -d latex_src/.svn; then rm -rf latex_src/.svn; fi;  \
	cd latex_src; latex userman; latex userman; bibtex userman; \
	latex userman; latex userman;

$(PACKAGE)-$(VERSION)-userman.ps: latex_src/userman.dvi
	@if [ "$(DVIPS)" != "no" ]; \
	then cd latex_src; dvips userman.dvi -o ../$(PACKAGE)-$(VERSION)-userman.ps; \
	else echo "Dvips was not found. Can't make a postscript user manual."; \
	fi;

$(PACKAGE)-$(VERSION)-userman.pdf: $(PACKAGE)-$(VERSION)-userman.ps
	@if [ "$(PS2PDF)" != "no" ]; \
	then ps2pdf ./$(PACKAGE)-$(VERSION)-userman.ps ./$(PACKAGE)-$(VERSION)-userman.pdf; \
	else echo "Ps2pdf was not found. Can't make a pdf user manual."; \
	fi;

clean:
	rm -rf ./latex_src/*.{aux,toc,bbl,blg,log,dvi}
	rm -f ./$(PACKAGE)-$(VERSION)-userman.ps ./$(PACKAGE)-$(VERSION)-userman.pdf

## DIST HOOK:
dist-hook:
	rm -rf `find $(distdir) -name '.svn'`;
	rm -rf $(distdir)/latex_src/*.{aux,toc,bbl,blg,log,dvi}
