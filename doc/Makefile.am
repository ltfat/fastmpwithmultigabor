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
## CVS log:
##
##   $Author: sacha $
##   $Date: 2005/07/04 15:30:21 $
##   $Revision: 1.5 $
##

EXTRA_DIST = doxygen_config

## DIST HOOK:
#
#  Before packaging, force a complete re-compiling of the doc.
#
dist-hook:
	@if [ "$(DOXYGEN)" != "no" ]; then \
	cd $(top_srcdir); doxygen doc/doxygen_config; \
	else \
	echo "Doxygen was not found. The old docs will be re-included in the package."; \
	fi
	cd $(top_srcdir)/doc/refman/latex; latex refman; latex refman; bibtex refman; \
	latex refman; latex refman; dvips refman -o; ps2pdf refman.ps refman.pdf; \
	mv refman.ps  $(PACKAGE)-$(VERSION)-refman.ps; \
	mv refman.pdf $(PACKAGE)-$(VERSION)-refman.pdf;
	cd $(top_srcdir)/doc/userman; latex userman; latex userman; bibtex userman; \
	latex userman; latex userman; dvips userman -o; ps2pdf userman.ps userman.pdf; \
	mv userman.ps  $(PACKAGE)-$(VERSION)-userman.ps; \
	mv userman.pdf $(PACKAGE)-$(VERSION)-userman.pdf;
	cd $(top_srcdir)/doc/short_description; latex short_description; latex short_description; \
	dvips short_description -o; ps2pdf short_description.ps short_description.pdf; \
	mv short_description.ps  $(PACKAGE)-$(VERSION)-short_description.ps; \
	mv short_description.pdf $(PACKAGE)-$(VERSION)-short_description.pdf;
