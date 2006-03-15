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
## SVN log:
##
##   $Author$
##   $Date$
##   $Revision$
##

SUBDIRS = signals ref

noinst_PROGRAMS = test_win test_fft test_parabolic_regression test_mp test_mask \
	 test_messaging test_anywave test_behaviour_in_noise test_convolution

test_win_SOURCES = test_win.c
test_win_LDADD = ../libdsp_windows/libdsp_windows.a
test_win_DEPENDENCIES = ../libdsp_windows/libdsp_windows.a

test_fft_SOURCES = test_fft.cpp
test_fft_LDADD = @LOCAL_BUILD_LIBS@
test_fft_DEPENDENCIES = @LOCAL_BUILD_LIBS@

test_parabolic_regression_SOURCES = test_parabolic_regression.cpp
test_parabolic_regression_LDADD = @LOCAL_BUILD_LIBS@
test_parabolic_regression_DEPENDENCIES = @LOCAL_BUILD_LIBS@

test_mp_SOURCES = test_mp.cpp
test_mp_LDADD = @LOCAL_BUILD_LIBS@
test_mp_DEPENDENCIES = @LOCAL_BUILD_LIBS@

test_mask_SOURCES = test_mask.cpp
test_mask_LDADD = @LOCAL_BUILD_LIBS@
test_mask_DEPENDENCIES = @LOCAL_BUILD_LIBS@

test_messaging_SOURCES = test_messaging.cpp
test_messaging_LDADD = @LOCAL_BUILD_LIBS@
test_messaging_DEPENDENCIES = @LOCAL_BUILD_LIBS@

test_anywave_SOURCES = test_anywave.cpp
test_anywave_LDADD = @LOCAL_BUILD_LIBS@
test_anywave_DEPENDENCIES = @LOCAL_BUILD_LIBS@

test_behaviour_in_noise_SOURCES = test_behaviour_in_noise.cpp
test_behaviour_in_noise_LDADD = @LOCAL_BUILD_LIBS@
test_behaviour_in_noise_DEPENDENCIES = @LOCAL_BUILD_LIBS@

test_convolution_SOURCES = test_convolution.cpp
test_convolution_LDADD = @LOCAL_BUILD_LIBS@
test_convolution_DEPENDENCIES = @LOCAL_BUILD_LIBS@

TESTS = test_mpcycle.sh
TESTS_ENVIRONMENT = $(SHELL)

EXTR_DIST = test_mpcycle.sh.in
DISTCLEANFILES = test_mpcycle.sh

# Make the regen-test-ref target if you want to re-sync the test suite references
# with the most recent build.
regen-test-ref: regen-test-ref-mpd regen-test-ref-mpr regen-test-ref-mpf

regen-test-ref-mpd: $(top_builddir)/src/utils/mpd
	$(top_builddir)/src/utils/mpd --verbose -s 10 -R 10 -D $(srcdir)/signals/dico_test.xml \
		$(srcdir)/signals/mix_58_stereo.wav $(srcdir)/ref/book_mix_58_stereo.bin.ref \
		$(srcdir)/ref/res_mix_58_stereo.wav.ref >& $(srcdir)/ref/log_mpd_mix_58_stereo.ref;

regen-test-ref-mpr: $(top_builddir)/src/utils/mpr
	$(top_builddir)/src/utils/mpr --verbose $(srcdir)/ref/book_mix_58_stereo.bin.ref \
		$(srcdir)/ref/recons_mix_58_stereo.wav.ref $(srcdir)/ref/res_mix_58_stereo.wav.ref \
		>& $(srcdir)/ref/log_mpr_mix_58_stereo.ref;

regen-test-ref-mpf: $(top_builddir)/src/utils/mpf
	$(top_builddir)/src/utils/mpf --Freq=[0:110] --len=[0:256] $(srcdir)/ref/book_mix_58_stereo.bin.ref \
		>& $(srcdir)/ref/log_mpf_book_mix_58_stereo.bin.ref;
