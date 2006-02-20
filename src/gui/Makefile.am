#############################################################################
##                                                                         ##
##                               Makefile.am                               ##
##                                                                         ##
##                        Matching Pursuit Utilities                       ##
##                                                                         ##
## R�i Gribonval                                                         ##
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
## This part of the project has been realised as an IFSIC/DIIC3
## student's project by:
##
##   Nicolas Bonnet
##   Benjamin Boutier
##   Vincent Chapon
##   Sylvestre Cozic
##
##
## SVN log:
##
##   $Author: sacha $
##   $Date$
##   $Revision$
##

mptk_gui_headers = MptkGuiApp.h \
	MptkGuiGlobalConstants.h \
	MptkGuiFrame.h \
	MptkGuiAudio.h \
	MptkGuiCallback.h \
	MptkGuiColormaps.h \
	MptkGuiOpenDialog.h \
	MptkGuiMPSettingsDialog.h \
	MptkGuiExtendedView.h \
	MptkGuiSignalView.h \
	MptkGuiExtendedSignalView.h \
	MptkGuiSpectrogramView.h\
	MptkGuiSpectrogramDessin.h\
	MptkGuiSashWindow.h \
	MptkGuiTFView.h \
	MptkGuiAtomView.h \
	MptkGuiAtomDessin.h \
	MptkGuiDessin.h \
	MptkGuiExtendedTFView.h \
	MptkGuiZoomEvent.h \
	MptkGuiDeleteViewEvent.h \
	MptkGuiColorMapView.h \
	MptkGuiConsoleView.h \
	MptkGuiUpDownPanel.h \
	MptkGuiUpEvent.h \
	MptkGuiDownEvent.h
	MptkGuiConsoleView.h \
	MptkGuiCMapZoomEvent.h \
	MptkGuiSaveDialog.h \
	MptkGuiResizeTFMapEvent \
	MptkGuiLimitsSettingsDialog.h \
	MptkGuiListenFinishedEvent.h	\
	MptkGuiSettingEvent.h \
	MptkGuiSettingUpdateEvent.h \
	gpl.h \
	MptkGuiLicenseDialog.h

bin_PROGRAMS = MptkGuiApp

noinst_HEADERS = $(mptk_gui_headers)

if GNU
portaudio_src = portaudio_v18_1/pa_common/pa_lib.c        \
		portaudio_v18_1/pa_unix_oss/pa_unix.c     \
		portaudio_v18_1/pa_unix_oss/pa_unix_oss.c
endif
if OSX
portaudio_src = portaudio_v18_1/pa_mac_core/pa_mac_core.c \
        portaudio_v18_1/pa_common/pa_lib.c     \
        portaudio_v18_1/pa_common/portaudio.h  \
        portaudio_v18_1/pa_common/pa_host.h    \
        portaudio_v18_1/pa_common/pa_convert.c \
        portaudio_v18_1/pa_common/pa_trace.c   \
        portaudio_v18_1/pa_common/pa_trace.h   \
        portaudio_v18_1/pablio/ringbuffer.c    \
        portaudio_v18_1/pablio/ringbuffer.h
endif
if WIN
portaudio_src = portaudio_v18_1/pa_common/pa_lib.c \
        portaudio_v18_1/pa_common/portaudio.h      \
        portaudio_v18_1/pa_common/pa_host.h        \
	portaudio_v18_1/pa_win_wmme/pa_win_wmme.c
endif

MptkGuiApp_SOURCES = $(mptk_gui_headers) \
	MptkGuiApp.cpp       \
	MptkGuiFrame.cpp     \
	MptkGuiAudio.cpp     \
	MptkGuiCallback.cpp  \
	MptkGuiColormaps.cpp \
	MptkGuiOpenDialog.cpp \
	MptkGuiMPSettingsDialog.cpp \
	MptkGuiSignalView.cpp \
	MptkGuiExtendedSignalView.cpp \
	MptkGuiSpectrogramView.cpp \
	MptkGuiSpectrogramDessin.cpp\
	MptkGuiSashWindow.cpp \
	MptkGuiTFView.cpp \
	MptkGuiAtomView.cpp \
	MptkGuiAtomDessin.cpp \
	MptkGuiDessin.cpp \
	MptkGuiExtendedTFView.cpp \
	MptkGuiZoomEvent.cpp \
	MptkGuiDeleteViewEvent.cpp \
	MptkGuiHandlers.cpp \
	MptkGuiColorMapView.cpp \
	MptkGuiConsoleView.cpp \
	MptkGuiUpDownPanel.cpp \
	MptkGuiUpEvent.cpp \
	MptkGuiDownEvent.cpp \
	MptkGuiCMapZoomEvent.cpp \
	MptkGuiSaveDialog.cpp \
	MptkGuiResizeTFMapEvent.cpp \
	MptkGuiLimitsSettingsDialog.cpp \
	MptkGuiListenFinishedEvent.cpp \
	MptkGuiSettingEvent.cpp	\
	MptkGuiSettingUpdateEvent.cpp	\
	MptkGuiLicenseDialog.cpp \
	$(portaudio_src)


MptkGuiApp_CXXFLAGS = @WXWIDGETS_CXXFLAGS@ -I$(srcdir)/portaudio_v18_1/pa_common/ -I$(srcdir)/portaudio_v18_1/pablio/
MptkGuiApp_CFLAGS = -I$(srcdir)/portaudio_v18_1/pa_common/  -I$(srcdir)/portaudio_v18_1/pablio/
MptkGuiApp_LDADD = @LIBS@ @LOCAL_BUILD_LIBS@ @WXWIDGETS_LIBS@ @PA_FLAGS@
MptkGuiApp_DEPENDENCIES = @LOCAL_BUILD_LIBS@
