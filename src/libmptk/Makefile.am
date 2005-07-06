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
##   $Date: 2005/07/04 13:38:01 $
##   $Revision: 1.3 $
##

export_headers = mp_types.h \
	win_server.h      \
	fft_interface.h   \
	general.h \
	signal.h  \
	tfmap.h   \
	atom.h  block.h \
	book.h dict.h   \
	atom_classes/gabor_atom.h atom_classes/gabor_block.h         \
	atom_classes/harmonic_atom.h atom_classes/harmonic_block.h         \
	atom_classes/dirac_atom.h atom_classes/dirac_block.h         \
	atom_classes/TEMPLATE_atom.h atom_classes/TEMPLATE_block.h   \
	atom_classes/block_io_interface.h \
	atom_classes/atom_classes.h
## NOTE: in the above list, the order of the headers is important.

BUILT_SOURCES = mptk.h
CLEANFILES = $(srcdir)/mptk.h

EXTRA_DIST = HOWTO_ADD_A_CLASS DEVEL_NOTES

lib_LIBRARIES = libmptk.a
include_HEADERS = mptk.h
noinst_HEADERS = system.h header.h $(export_headers)

libmptk_a_SOURCES = mptk.h header.h $(export_headers)\
	general.cpp \
	signal.cpp  \
	win_server.cpp \
	fft_interface.cpp \
	block.cpp atom.cpp atom_classes/atom_classes.cpp \
        atom_classes/gabor_atom.cpp atom_classes/gabor_block.cpp         \
        atom_classes/harmonic_atom.cpp atom_classes/harmonic_block.cpp         \
        atom_classes/dirac_atom.cpp atom_classes/dirac_block.cpp         \
        atom_classes/TEMPLATE_atom.cpp atom_classes/TEMPLATE_block.cpp   \
	atom_classes/dict_scanner.lpp \
	atom_classes/block_io_interface.cpp \
	dict.cpp book.cpp tfmap.cpp

mptk.h: Makefile.am $(srcdir)/header.h $(srcdir)/../../AUTHORS $(export_headers)
	( head -n 36 $(srcdir)/header.h; \
	echo; echo; \
	echo "/* * * * * * * * * * * * * * * * * * * * * * *";\
	cat $(srcdir)/../../AUTHORS; \
	echo "* * * * * * * * * * * * * * * * * * * * * * */";\
	echo; echo; \
	echo "#ifndef __mptk_h_";\
	echo "#define __mptk_h_";\
	echo; echo; \
	for header in $(export_headers); do  \
                echo;  \
                tail -n +30 $(srcdir)/$$header ;  \
        done; \
	echo; echo; \
	echo "#endif /* __mptk_h_ */"; \
	echo;echo;) > $(srcdir)/$@;
