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
##   $Date: 2005/07/04 15:30:17 $
##   $Revision: 1.11 $
##

SUBDIRS = src doc

EXTRA_DIST = README INSTALL COPYING CHANGES AUTHORS make_configure.sh doc extras

# DIST-HOOK:
#
# Before packaging, remove the unnecessary files from the doc and extras directories.
#
dist-hook:
	rm -rf `find $(distdir) -name '\.svn' -print , -name auto -print , -name '*~' -print`
	rm -rf `find $(distdir) -name '*.log' -print , -name '*.bbl' -print , -name '*.blg' -print`
	rm -rf `find $(distdir) -name '*.toc' -print , -name '*.dvi' -print , -name '*.aux' -print`
