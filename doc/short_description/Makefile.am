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

EXTRA_DIST = latex_src/short_description.tex \
             $(PACKAGE)-$(VERSION)-short_description.ps $(PACKAGE)-$(VERSION)-short_description.pdf

all: $(PACKAGE)-$(VERSION)-short_description.ps $(PACKAGE)-$(VERSION)-short_description.pdf

latex_src/short_description.dvi: $(top_srcdir)/doc/short_description/latex_src/short_description.tex
	cp -rf $(top_srcdir)/doc/short_description/latex_src/ ./;  \
	if test -d latex_src/.svn; then rm -rf latex_src/.svn; fi;  \
	cd latex_src; latex short_description; latex short_description;

$(PACKAGE)-$(VERSION)-short_description.ps: latex_src/short_description.dvi
	@if [ "$(DVIPS)" != "no" ]; \
	then cd latex_src; dvips short_description.dvi -o ../$(PACKAGE)-$(VERSION)-short_description.ps; \
	else echo "Dvips was not found. Can't make a postscript short-description."; \
	fi;

$(PACKAGE)-$(VERSION)-short_description.pdf: $(PACKAGE)-$(VERSION)-short_description.ps
	@if [ "$(PS2PDF)" != "no" ]; \
	then ps2pdf ./$(PACKAGE)-$(VERSION)-short_description.ps ./$(PACKAGE)-$(VERSION)-short_description.pdf; \
	else echo "Ps2pdf was not found. Can't make a pdf short-description."; \
	fi;

clean:
	rm -rf ./latex_src/*.{aux,toc,bbl,blg,log,dvi}
	rm -f ./$(PACKAGE)-$(VERSION)-short_description.ps ./$(PACKAGE)-$(VERSION)-short_description.pdf

## DIST HOOK:
dist-hook:
	rm -rf `find $(distdir) -name '.svn'`;
	rm -rf $(distdir)/latex_src/*.{aux,toc,bbl,blg,log,dvi}
