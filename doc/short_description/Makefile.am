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

EXTRA_DIST = short_description.tex

## DIST HOOK:
#
#  Before packaging, force a complete re-compiling of the doc.
#
dist-hook:
	cd $(top_srcdir)/doc/short_description; latex short_description; latex short_description; \
	dvips short_description -o; ps2pdf short_description.ps short_description.pdf;
	cp -f $(top_srcdir)/doc/short_description/short_description.ps  $(distdir)/$(PACKAGE)-$(VERSION)-short_description.ps;
	cp -f $(top_srcdir)/doc/short_description/short_description.pdf $(distdir)/$(PACKAGE)-$(VERSION)-short_description.pdf;
