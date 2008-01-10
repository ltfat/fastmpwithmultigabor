# Include src folder to compiling proccess
INCLUDE (FindQt)
find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
INCLUDE (${QT_USE_FILE})

# find mingw32.dll for windows
IF(MINGW)
FIND_LIBRARY(MINGW_LIBRARY_FILE mingwm10
$ENV{LIB}
$ENV{SystemDrive}/WINDOWS/system32
$ENV{SystemDrive}/MinGW/bin
)
IF(MINGW_LIBRARY_FILE)
  MESSAGE(STATUS "mingw32.dll library found !!")
ELSE(MINGW_LIBRARY_FILE)
  MESSAGE(STATUS "mingw32.dll library not found !!")
ENDIF(MINGW_LIBRARY_FILE)  
ENDIF(MINGW)
# copyfiles for demo:
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/dictionary/dic_gabor_two_scales.xml ${MPTK_BINARY_DIR}/bin/reference/dictionary/dic_gabor_two_scales.xml
                 COPYONLY) 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/signal/glockenspiel.wav ${MPTK_BINARY_DIR}/bin/reference/signal/glockenspiel.wav
                 COPYONLY) 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/signal/mix_58_stereo.wav ${MPTK_BINARY_DIR}/bin/reference/signal/mix_58_stereo.wav
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/mixer/mix_58_mixer.txt ${MPTK_BINARY_DIR}/bin/reference/mixer/mix_58_mixer.txt
                 COPYONLY) 

# the variable "qtproject_SRCS" contains all .cpp files of this project
IF(WIN32)
SET(port_audio-files
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_lib.c 
    ${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/portaudio.h      
    ${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_host.h        
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_win_wmme/pa_win_wmme.c
)
ELSE(WIN32)
IF(UNIX)
	IF(APPLE)
SET(port_audio-files
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_mac_core/pa_mac_core.c 
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_lib.c        
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_convert.c
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_trace.c
	${GUI_SOURCE_DIR}/portaudio_v18_1/pablio/ringbuffer.c
	)
ELSE(APPLE)
SET(port_audio-files
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_common/pa_lib.c        
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_unix_oss/pa_unix.c     
	${GUI_SOURCE_DIR}/portaudio_v18_1/pa_unix_oss/pa_unix_oss.c
	)
ENDIF(APPLE)
ENDIF(UNIX)
ENDIF(WIN32)

set(qtproject_SRCS
${port_audio-files}
presentation/main.cpp
presentation/main_window.cpp
presentation/dialog.cpp
core/gui_callback.cpp
core/gui_callback_demix.cpp
core/gui_callback_demo.cpp
core/gui_callback_reconstruction.cpp
core/gui_audio.cpp  
)

SET(qtproject_UIS
presentation/MPTK_GUI_APP.ui
)

SET(qtproject_HDRS
presentation/main_window.h
presentation/dialog.h
core/gui_callback_abstract.h
core/gui_callback_reconstruction.h
core/gui_callback.h
core/gui_callback_demix.h
core/gui_callback_demo.h
core/gui_audio.h
)

SET(qtproject_RCCS presentation/mptkgui.qrc)

# tell cmake to create .moc files for all files in the variable qtproject_SRCS that require such a file.
# note: this assumes that you use #include "header.moc" in your files
QT4_AUTOMOC(${qtproject_SRCS})
	
QT4_WRAP_UI(qtproject_UIS_H ${qtproject_UIS})

IF(QT_WRAP_CPP)	
QT4_WRAP_CPP(qtproject_MOC ${qtproject_UIS_H} ${qtproject_HDRS})
ENDIF(QT_WRAP_CPP)

QT4_ADD_RESOURCES(qtproject_RCC_SRCS ${qtproject_RCCS})
	
ADD_DEFINITIONS(-DQT_NO_DEBUG)




IF(WIN32)
# create an executable file named "MPTK-GUI" from the source files in the variable "qtproject_SRCS".
ADD_EXECUTABLE(MPTK-GUI ${qtproject_SRCS} ${qtproject_MOC} ${qtproject_RCC_SRCS})
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(MPTK-GUI PROPERTIES LINK_FLAGS "${SHARED_FLAGS} -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
SET_TARGET_PROPERTIES(MPTK-GUI PROPERTIES LINK_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 

# link the "MPTK-GUI" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
target_link_libraries(MPTK-GUI ${QT_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY} ${QT_QTMAIN_LIBRARY}  ${QT_QTCORE_LIBRARY} mptk -lgdi32 -lcomdlg32 -loleaut32 -limm32 -lwinmm -lwinspool -lws2_32 -lole32 -luuid -luser32 -ladvapi32)
ADD_CUSTOM_COMMAND (
                    TARGET MPTK-GUI
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND}
                    ARGS -E copy "${MINGW_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/mingwm10.dll"
        )
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/mingwm10.dll" DESTINATION bin)
ELSE(WIN32)
IF(APPLE)
ADD_EXECUTABLE(MPTK-GUI MACOSX_BUNDLE  ${qtproject_SRCS} ${qtproject_MOC} ${qtproject_RCC_SRCS})
SET_TARGET_PROPERTIES(MPTK-GUI PROPERTIES LINK_FLAGS "${SHARED_FLAGS} -framework CoreFoundation -framework CoreAudio -framework AudioUnit -framework AudioToolbox -Wno-deprecated")
target_link_libraries(MPTK-GUI ${QT_LIBRARIES} mptk)
ELSE(APPLE)
add_executable(MPTK-GUI ${qtproject_SRCS} ${qtproject_MOC} ${qtproject_RCC_SRCS})
target_link_libraries(MPTK-GUI ${QT_LIBRARIES} mptk)
ENDIF(APPLE)
ENDIF(WIN32)
INSTALL(TARGETS
MPTK-GUI RUNTIME DESTINATION bin
)
INSTALL(FILES
${MPTK_BINARY_DIR}/bin/reference/dictionary/dic_gabor_two_scales.xml DESTINATION bin/reference/dictionary)
INSTALL(FILES
${MPTK_BINARY_DIR}/bin/reference/signal/glockenspiel.wav ${MPTK_BINARY_DIR}/bin/reference/signal/glockenspiel.wav DESTINATION bin/reference/signal)
INSTALL(FILES
${MPTK_BINARY_DIR}/bin/reference/mixer/mix_58_mixer.txt DESTINATION bin/reference/mixer)


