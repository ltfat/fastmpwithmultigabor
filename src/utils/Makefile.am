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
## SVN log:
##
##   $Author$
##   $Date$
##   $Revision$
##

bin_PROGRAMS = mpd mpd_demix mpr mpcat mpf mpview

noinst_HEADERS = getopt.h

mpd_SOURCES = mpd.cpp getopt.c getopt1.c getopt.h
mpd_LDADD = @LOCAL_BUILD_LIBS@
mpd_DEPENDENCIES = @LOCAL_BUILD_LIBS@

mpd_demix_SOURCES = mpd_demix.cpp
mpd_demix_LDADD = @LOCAL_BUILD_LIBS@
mpd_demix_DEPENDENCIES = @LOCAL_BUILD_LIBS@

mpr_SOURCES = mpr.cpp
mpr_LDADD = @LOCAL_BUILD_LIBS@
mpr_DEPENDENCIES = @LOCAL_BUILD_LIBS@

mpf_SOURCES = mpf.cpp
mpf_LDADD = @LOCAL_BUILD_LIBS@
mpf_DEPENDENCIES = @LOCAL_BUILD_LIBS@

mpcat_SOURCES = mpcat.cpp
mpcat_LDADD = @LOCAL_BUILD_LIBS@
mpcat_DEPENDENCIES = @LOCAL_BUILD_LIBS@

mpview_SOURCES = mpview.cpp
mpview_LDADD = @LOCAL_BUILD_LIBS@
mpview_DEPENDENCIES = @LOCAL_BUILD_LIBS@
