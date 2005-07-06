#############################################################################
##                                                                         ##
##                               Makefile.am                               ##
##                                                                         ##
##                      Matching Pursuit Testing Suite                     ##
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
##   $Date: 2005/07/04 09:55:24 $
##   $Revision: 1.18 $
##

noinst_PROGRAMS = test_win test_fft test_mp

test_win_SOURCES = test_win.c
test_win_LDADD = @LIBDSP_WIN_LIB@ -lm
test_win_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a

test_fft_SOURCES = test_fft.cpp
test_fft_LDADD = -lm @LIBMP_LIB@ @LIBDSP_WIN_LIB@ @FFTW3_LIBS@
test_fft_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a

test_mp_SOURCES = test_mp.cpp
test_mp_LDADD = @LIBMP_LIB@ -lm @FFTW3_LIBS@ @LIBDSP_WIN_LIB@ @LIBSNDFILE_LIBS@
test_mp_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a

SUBDIRS = signals
