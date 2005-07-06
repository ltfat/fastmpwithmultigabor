#############################################################################
##                                                                         ##
##                               Makefile.am                               ##
##                                                                         ##
##                        Matching Pursuit Utilities                       ##
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
##   $Date: 2005/07/04 09:55:31 $
##   $Revision: 1.13 $
##

bin_PROGRAMS = mpd mpd_demix mpr mpcat mpf
noinst_PROGRAMS = mpview

noinst_HEADERS = system.h getopt.h

mpd_SOURCES = mpd.cpp getopt.c getopt1.c getopt.h
mpd_LDADD = @LIBMP_LIB@ -lm @FFTW3_LIBS@ @LIBDSP_WIN_LIB@ @LIBSNDFILE_LIBS@
mpd_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a

mpd_demix_SOURCES = mpd_demix.cpp
mpd_demix_LDADD = @LIBMP_LIB@ -lm @FFTW3_LIBS@ @LIBDSP_WIN_LIB@ @LIBSNDFILE_LIBS@
mpd_demix_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a

mpr_SOURCES = mpr.cpp
mpr_LDADD = @LIBMP_LIB@ -lm @FFTW3_LIBS@ @LIBDSP_WIN_LIB@ @LIBSNDFILE_LIBS@
mpr_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a

mpf_SOURCES = mpf.cpp
mpf_LDADD = @LIBMP_LIB@ -lm @FFTW3_LIBS@ @LIBDSP_WIN_LIB@ @LIBSNDFILE_LIBS@
mpf_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a

mpcat_SOURCES = mpcat.cpp
mpcat_LDADD = @LIBMP_LIB@ -lm @FFTW3_LIBS@ @LIBDSP_WIN_LIB@ @LIBSNDFILE_LIBS@
mpcat_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a

mpview_SOURCES = mpview.cpp
mpview_LDADD = @LIBMP_LIB@ -lm @FFTW3_LIBS@ @LIBDSP_WIN_LIB@ @LIBSNDFILE_LIBS@
mpview_DEPENDENCIES = @LIBDSP_WIN_DIR@/libdsp_windows.a @LIBMP_DIR@/libmptk.a
