#------------------------------------------------
# Build mdp executable
# 
##Define macro to find source:
MACRO(GET_MPD_CPP_SOURCES out)
  SET(${out}
    ${UTILS_SOURCE_DIR}/mpd.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
  )
ENDMACRO(GET_MPD_CPP_SOURCES)
GET_MPD_CPP_SOURCES(MPD_EXE_SOURCES)
ADD_CUSTOM_TARGET(mpd-executable DEPENDS ${MPD_EXE_SOURCES})
IF(BUILD_MULTITHREAD)
ADD_EXECUTABLE(mpd_multithread ${MPD_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpd_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -lpthread")
TARGET_LINK_LIBRARIES(mpd_multithread mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpd_multithread mpd-executable)
ELSE(BUILD_MULTITHREAD)
ADD_EXECUTABLE(mpd ${MPD_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpd PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
TARGET_LINK_LIBRARIES(mpd mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpd mpd-executable)
ENDIF(BUILD_MULTITHREAD)
#For win32 and plateform and MINGW build command, copy the dll files in the build dir
IF (MINGW)
#=== Copy the dll in the bin folder===
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${PTHREAD_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/pthreadVC2.dll"
        )
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${FFTW3_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/libfftw3-3.dll"
        ) 
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${SNDFILE_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/libsndfile-1.dll"
        )              
        

 ENDIF (MINGW)

#ADD_DEPENDENCIES(mpd mpd-executable)
#------------------------------------------------
# Build mdp_demix executable
#
MACRO(GET_MPD_DEMIX_CPP_SOURCES out)
  SET(${out}
    ${UTILS_SOURCE_DIR}/mpd_demix.cpp
      )
ENDMACRO(GET_MPD_DEMIX_CPP_SOURCES)
GET_MPD_DEMIX_CPP_SOURCES(MPD_DEMIX_EXE_SOURCES)
ADD_CUSTOM_TARGET(mpd_demix-executable DEPENDS ${MPD_DEMIX_EXE_SOURCES})
IF(BUILD_MULTITHREAD)
ADD_EXECUTABLE(mpd_demix_multithread ${MPD_DEMIX_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpd_demix_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -lpthread")
TARGET_LINK_LIBRARIES(mpd_demix_multithread mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpd_demix_multithread mpd_demix-executable)
ELSE(BUILD_MULTITHREAD)
ADD_EXECUTABLE(mpd_demix ${MPD_DEMIX_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpd_demix PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
TARGET_LINK_LIBRARIES(mpd_demix mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpd_demix mpd_demix-executable)
ENDIF(BUILD_MULTITHREAD)
#SET_TARGET_PROPERTIES(mpd_demix PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -pthread")
#TARGET_LINK_LIBRARIES(mpd_demix mptk dsp_windows fftw3.a sndfile.a libpthread.so)
#ADD_DEPENDENCIES(mpd_demix mpd_demix-executable)
#------------------------------------------------
# Build mdr executable
#
MACRO(GET_MPR_CPP_SOURCES out)
  SET(${out}
    ${UTILS_SOURCE_DIR}/mpr.cpp
      )
ENDMACRO(GET_MPR_CPP_SOURCES)
GET_MPR_CPP_SOURCES(MPR_EXE_SOURCES)
ADD_CUSTOM_TARGET(mpr-executable DEPENDS ${MPR_EXE_SOURCES})
ADD_EXECUTABLE(mpr ${MPR_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpr PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
TARGET_LINK_LIBRARIES(mpr mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpr mpr-executable)
#------------------------------------------------
# Build mpf executable
#
MACRO(GET_MPF_CPP_SOURCES out)
  SET(${out}
    ${UTILS_SOURCE_DIR}/mpf.cpp
      )
ENDMACRO(GET_MPF_CPP_SOURCES)
GET_MPF_CPP_SOURCES(MPF_EXE_SOURCES)
ADD_CUSTOM_TARGET(mpf-executable DEPENDS ${MPF_EXE_SOURCES})
ADD_EXECUTABLE(mpf ${MPF_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpf PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
TARGET_LINK_LIBRARIES(mpf mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpf mpf-executable)
#------------------------------------------------
# Build mpcat executable
#
MACRO(GET_MPCAT_CPP_SOURCES out)
  SET(${out}
    ${UTILS_SOURCE_DIR}/mpcat.cpp
      )
ENDMACRO(GET_MPCAT_CPP_SOURCES)
GET_MPCAT_CPP_SOURCES(MPCAT_EXE_SOURCES)
ADD_CUSTOM_TARGET(mpcat-executable DEPENDS ${MPCAT_EXE_SOURCES})
ADD_EXECUTABLE(mpcat ${MPCAT_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpcat PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
TARGET_LINK_LIBRARIES(mpcat mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpcat mpcat-executable)
#------------------------------------------------
# Build mpview executable
#
MACRO(GET_MPVIEW_CPP_SOURCES out)
  SET(${out}
    ${UTILS_SOURCE_DIR}/mpview.cpp
      )
ENDMACRO(GET_MPVIEW_CPP_SOURCES)
GET_MPVIEW_CPP_SOURCES(MPVIEW_EXE_SOURCES)
ADD_CUSTOM_TARGET(mpview-executable DEPENDS ${MPVIEW_EXE_SOURCES})
ADD_EXECUTABLE(mpview ${MPVIEW_EXE_SOURCES})
SET_TARGET_PROPERTIES(mpview PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
TARGET_LINK_LIBRARIES(mpview mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpview mpview-executable)
#------------------------------------------------
# Build MptkGuiApp executable
#
#Set header files
SET(MPTK_GUI_HEADER
${GUI_SOURCE_DIR}/MptkGuiApp.h 
${GUI_SOURCE_DIR}/MptkGuiGlobalConstants.h 
${GUI_SOURCE_DIR}/MptkGuiFrame.h 
${GUI_SOURCE_DIR}/MptkGuiAudio.h 
${GUI_SOURCE_DIR}/MptkGuiCallback.h 
${GUI_SOURCE_DIR}/MptkGuiColormaps.h 
${GUI_SOURCE_DIR}/MptkGuiOpenDialog.h 
${GUI_SOURCE_DIR}/MptkGuiMPSettingsDialog.h 
${GUI_SOURCE_DIR}/MptkGuiExtendedView.h 
${GUI_SOURCE_DIR}/MptkGuiSignalView.h 
${GUI_SOURCE_DIR}/MptkGuiExtendedSignalView.h 
${GUI_SOURCE_DIR}/MptkGuiSpectrogramView.h
${GUI_SOURCE_DIR}/MptkGuiSpectrogramDessin.h
${GUI_SOURCE_DIR}/MptkGuiSashWindow.h 
${GUI_SOURCE_DIR}/MptkGuiTFView.h
${GUI_SOURCE_DIR}/MptkGuiAtomView.h
${GUI_SOURCE_DIR}/MptkGuiAtomDessin.h
${GUI_SOURCE_DIR}/MptkGuiDessin.h 
${GUI_SOURCE_DIR}/MptkGuiExtendedTFView.h 
${GUI_SOURCE_DIR}/MptkGuiZoomEvent.h 
${GUI_SOURCE_DIR}/MptkGuiDeleteViewEvent.h 
${GUI_SOURCE_DIR}/MptkGuiHandlers.h 
${GUI_SOURCE_DIR}/MptkGuiColorMapView.h 
${GUI_SOURCE_DIR}/MptkGuiConsoleView.h 
${GUI_SOURCE_DIR}/MptkGuiUpDownPanel.h 
${GUI_SOURCE_DIR}/MptkGuiUpEvent.h 
${GUI_SOURCE_DIR}/MptkGuiDownEvent.h 
${GUI_SOURCE_DIR}/MptkGuiConsoleView.h 
${GUI_SOURCE_DIR}/MptkGuiCMapZoomEvent.h 
${GUI_SOURCE_DIR}/MptkGuiSaveDialog.h 
${GUI_SOURCE_DIR}/MptkGuiResizeTFMapEvent.h 
${GUI_SOURCE_DIR}/MptkGuiLimitsSettingsDialog.h 
${GUI_SOURCE_DIR}/MptkGuiListenFinishedEvent.h 
${GUI_SOURCE_DIR}/MptkGuiSettingEvent.h 
${GUI_SOURCE_DIR}/MptkGuiSettingUpdateEvent.h 
${GUI_SOURCE_DIR}/gpl.h 
${GUI_SOURCE_DIR}/MptkGuiLicenseDialog.h
)
SET(MPTK_GUI_SOURCES 
	${GUI_SOURCE_DIR}/MptkGuiApp.cpp       
	${GUI_SOURCE_DIR}/MptkGuiFrame.cpp     
	${GUI_SOURCE_DIR}/MptkGuiAudio.cpp    
	${GUI_SOURCE_DIR}/MptkGuiCallback.cpp  
	${GUI_SOURCE_DIR}/MptkGuiColormaps.cpp 
	${GUI_SOURCE_DIR}/MptkGuiOpenDialog.cpp 
	${GUI_SOURCE_DIR}/MptkGuiMPSettingsDialog.cpp 
	${GUI_SOURCE_DIR}/MptkGuiSignalView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiExtendedSignalView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiSpectrogramView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiSpectrogramDessin.cpp
	${GUI_SOURCE_DIR}/MptkGuiSashWindow.cpp 
	${GUI_SOURCE_DIR}/MptkGuiTFView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiAtomView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiAtomDessin.cpp 
	${GUI_SOURCE_DIR}/MptkGuiDessin.cpp 
	${GUI_SOURCE_DIR}/MptkGuiExtendedTFView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiZoomEvent.cpp 
	${GUI_SOURCE_DIR}/MptkGuiDeleteViewEvent.cpp 
	${GUI_SOURCE_DIR}/MptkGuiHandlers.cpp 
	${GUI_SOURCE_DIR}/MptkGuiColorMapView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiConsoleView.cpp 
	${GUI_SOURCE_DIR}/MptkGuiUpDownPanel.cpp 
	${GUI_SOURCE_DIR}/MptkGuiUpEvent.cpp 
	${GUI_SOURCE_DIR}/MptkGuiDownEvent.cpp 
	${GUI_SOURCE_DIR}/MptkGuiCMapZoomEvent.cpp 
	${GUI_SOURCE_DIR}/MptkGuiSaveDialog.cpp 
	${GUI_SOURCE_DIR}/MptkGuiResizeTFMapEvent.cpp 
	${GUI_SOURCE_DIR}/MptkGuiLimitsSettingsDialog.cpp 
	${GUI_SOURCE_DIR}/MptkGuiListenFinishedEvent.cpp 
	${GUI_SOURCE_DIR}/MptkGuiSettingEvent.cpp	
	${GUI_SOURCE_DIR}/MptkGuiSettingUpdateEvent.cpp	
	${GUI_SOURCE_DIR}/MptkGuiLicenseDialog.cpp 
)
# If we need to build the GUI
IF(BUILD_GUI)

IF(WIN32)

	MACRO(GET_MP_GUI_CPP_SOURCES out)
	SET(${out}
	${MPTK_GUI_HEADER}
	${MPTK_GUI_SOURCES}
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_lib.c        
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_unix_oss/pa_unix.c     
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_unix_oss/pa_unix_oss.c
	)
	ENDMACRO(GET_MP_GUI_CPP_SOURCES)

	GET_MP_GUI_CPP_SOURCES(MP_GUI_CPP_SOURCES)
	ADD_CUSTOM_TARGET(mpgui-executable DEPENDS ${MP_GUI_CPP_SOURCES})
	INCLUDE(${MPTK_SOURCE_DIR}/CMake/UsewxWidgets.cmake)
	ADD_EXECUTABLE(MptkGuiApp ${MP_GUI_CPP_SOURCES})
	SET_TARGET_PROPERTIES(MptkGuiApp PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
	TARGET_LINK_LIBRARIES(MptkGuiApp mptk dsp_windows ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE} ${wxWidgets_LIBRARIES})

ELSE(WIN32)
IF(UNIX)
	IF(APPLE)

	MACRO(GET_MP_GUI_CPP_SOURCES out)
	SET(${out}
	${MPTK_GUI_HEADER}
	${MPTK_GUI_SOURCES}
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_mac_core/pa_mac_core.c 
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_lib.c        
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_convert.c
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_trace.c
	${GUI_SOURCE_DIR}/portaudio_v18_1/pablio/ringbuffer.c
	)
	ENDMACRO(GET_MP_GUI_CPP_SOURCES)

	ELSE(APPLE)

	MACRO(GET_MP_GUI_CPP_SOURCES out)
	SET(${out}
	${MPTK_GUI_HEADER}
	${MPTK_GUI_SOURCES}
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_lib.c        
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_unix_oss/pa_unix.c     
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_unix_oss/pa_unix_oss.c
	)
	ENDMACRO(GET_MP_GUI_CPP_SOURCES)

	ENDIF(APPLE)

	GET_MP_GUI_CPP_SOURCES(MP_GUI_CPP_SOURCES)
	ADD_CUSTOM_TARGET(mpgui-executable DEPENDS ${MP_GUI_CPP_SOURCES})
	INCLUDE(${MPTK_SOURCE_DIR}/CMake/UsewxWidgets.cmake)
	ADD_EXECUTABLE(MptkGuiApp ${MP_GUI_CPP_SOURCES})
	IF(APPLE)
	SET_TARGET_PROPERTIES(MptkGuiApp PROPERTIES LINK_FLAGS "${SHARED_FLAGS} -framework CoreFoundation -framework CoreAudio -framework AudioUnit -framework AudioToolbox")
	ENDIF(APPLE)
	SET_TARGET_PROPERTIES(MptkGuiApp PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS}")
	TARGET_LINK_LIBRARIES(MptkGuiApp mptk dsp_windows ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE} ${wxWidgets_LIBRARIES})

	#------------------------------------------------
	# Define install target:
	#
	INSTALL(TARGETS
	  MptkGuiApp
	   RUNTIME DESTINATION ${BIN_HOME}
	   )
ENDIF(UNIX)
ENDIF(WIN32)

ENDIF(BUILD_GUI)

#------------------------------------------------
# Define install target:
#
IF(BUILD_MULTITHREAD)
INSTALL(TARGETS
  mpd_multithread
  mpd_demix_multithread
  mpr
  mpf
  mpcat
  mpview
 RUNTIME DESTINATION ${BIN_HOME}
)
#Install dll in the destination folder for Win32 plateform
IF(MINGW)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/pthreadVC2.dll" DESTINATION "${BIN_HOME}")
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libfftw3-3.dll" DESTINATION "${BIN_HOME}")
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libsndfile-1.dll" DESTINATION "${BIN_HOME}")
ENDIF(MINGW)
ELSE(BUILD_MULTITHREAD)
INSTALL(TARGETS
  mpd
  mpd_demix
  mpr
  mpf
  mpcat
  mpview
 RUNTIME DESTINATION ${BIN_HOME}
)
#Install dll in the destination folder for Win32 plateform
IF(MINGW)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/pthreadVC2.dll" DESTINATION "${BIN_HOME}")
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libfftw3-3.dll" DESTINATION "${BIN_HOME}")
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libsndfile-1.dll" DESTINATION "${BIN_HOME}")
ENDIF(MINGW)
ENDIF(BUILD_MULTITHREAD)

## Cpack RULES
# Define the rules for packing files
INCLUDE(InstallRequiredSystemLibraries)
#SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
#SET(CPACK_GENERATOR "TGZ")
#SET(CPACK_PACKAGE_NAME "${PACKAGENAME}")
#SET(CPACK_PACKAGE_VENDOR "METISS Project IRISA")
#SET(CPACK_INSTALL_CMAKE_PROJECTS "${MPTK_BINARY_DIR};MPTK;ALL;/")
#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Matching Pursuit Tool Kit")
#SET(CPACK_PACKAGE_FILE_NAME "${PACKAGENAMEFULL}")
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${MPTK_SOURCE_DIR} .")
#SET(CPACK_PACKAGE_VENDOR "METISS Project IRISA")
#SET(CPACK_PACKAGE_VERSION ${BUILDVERSION})
#SET(CPACK_RESOURCE_FILE_LICENSE "/udd/broy/workspace/MPTK-trunk/COPYING")
#SET(CPACK_RESOURCE_FILE_README "/udd/broy/local/share/CMake/Templates/CPack.GenericDescription.txt")
#SET(CPACK_RESOURCE_FILE_WELCOME "/udd/broy/local/share/CMake/Templates/CPack.GenericWelcome.txt")
#SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGENAMEFULL}-Source")
#SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${MPTK_BINARY_DIR}/CPackSourceConfig1.cmake")
#SET(CPACK_SOURCE_TOPLEVEL_TAG "Linux-Source")
#SET(CPACK_SYSTEM_NAME "Linux")
#SET(CPACK_TOPLEVEL_TAG "Linux")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${MPTK_SOURCE_DIR}/README")
#SET(CPACK_RESOURCE_FILE_LICENSE "${MPTK_SOURCE_DIR}/COPYING")
#IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
#  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
#  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\mpd.exe;bin\\\\mpcat.exe; ")
#  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} MPTK")
#  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
#  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
#  SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
#SET(CPACK_NSIS_MODIFY_PATH ON)
#ELSE(WIN32 AND NOT UNIX)
#  SET(CPACK_STRIP_FILES "bin/mpd;bin/mpcat")
#  SET(CPACK_SOURCE_STRIP_FILES "")
#  SET(CPACK_IGNORE_FILES "/CMake/;/CMakeFiles/;/_CPack_Packages/;/src/;/doc/;/bin/make_regression_constants;/install_manifest_/;/www/;/CVS//;/.svn/;.cdtprojects;.project;/.settings/")
#ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "mpd;mpd" )


#;\\.#;/#;.*~;cscope.*;\\.swp$
#SET(CPACK_INSTALLED_DIRECTORIES "/udd/broy/workspace/MPTKtrunk;/")
#SET(CPACK_INSTALL_CMAKE_PROJECTS "")
#SET(CPACK_NSIS_DISPLAY_NAME "MPTK .")
#SET(CPACK_OUTPUT_CONFIG_FILE "${MPTK_BINARY_DIR}/CPackConfig.cmake")
#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MPTK")
#INCLUDE(CPack)




