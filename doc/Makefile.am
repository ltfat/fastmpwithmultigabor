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

SUBDIRS = userman short_description

EXTRA_DIST = doxygen_config refman

refman:
	@if [ "$(DOXYGEN)" != "no" ]; then \
	cd $(top_srcdir); doxygen doc/doxygen_config; \
	else \
	mkdir $(top_srcdir)/doc/refman; \
	echo "Doxygen was not found. The old docs (or no docs) will be re-included in the package."; \
	fi

## DIST HOOK:
#
#  Before packaging, force a complete re-compiling of the doc.
#
dist-hook:
	@if [ "$(DOXYGEN)" != "no" ]; then \
	cd $(top_srcdir); doxygen doc/doxygen_config; \
	else \
	mkdir $(top_srcdir)/doc/refman; \
	echo "Doxygen was not found. The old docs (or no docs) will be re-included in the package."; \
	fi
	cd $(top_srcdir)/doc/refman/latex; latex refman; latex refman; bibtex refman; \
	latex refman; latex refman; dvips refman -o; ps2pdf refman.ps refman.pdf;
	cp -rfu $(top_srcdir)/doc/refman/latex/refman.ps  $(distdir)/refman/$(PACKAGE)-$(VERSION)-refman.ps;
	cp -rfu $(top_srcdir)/doc/refman/latex/refman.pdf $(distdir)/refman/$(PACKAGE)-$(VERSION)-refman.pdf;
