if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/utils/readme.txt ${MPTK_BINARY_DIR}/bin/readme.txt COPYONLY)

#In case of 64 bits plateform we have to compil with -fPIC flag
# THIS SHOULD BE INHERITED FROM CALLING CMakeLists.txt !!!!

IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(EXECUTABLE_COMPILE_FLAGS " -fPIC")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(EXECUTABLE_COMPILE_FLAGS "-rdynamic -Wno-deprecated -fPIC")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(EXECUTABLE_COMPILE_FLAGS "")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
# Removed the -Wno-deprecated to remove a Warning on Mac (gcc 4.0.1) about
# -Wno-deprecated being meaningless for C-code, only meaningful for C++/ObjC++
#    SET(EXECUTABLE_COMPILE_FLAGS  "-rdynamic -Wno-deprecated")
    SET(EXECUTABLE_COMPILE_FLAGS  "-rdynamic")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 

#------------------------------------------------
# Build mdp executable
# 
SET(mpd_EXE_SOURCES
    ${UTILS_SOURCE_DIR}/mpd.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
  )
#------------------------------------------------
# Build mdp_demix executable
#
SET(mpd_demix_EXE_SOURCES 
    ${UTILS_SOURCE_DIR}/mpd_demix.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )
#------------------------------------------------
# Build mdr executable
#
SET(mpr_EXE_SOURCES 

    ${UTILS_SOURCE_DIR}/mpr.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )
#------------------------------------------------
# Build mpf executable
#
  SET(mpf_EXE_SOURCES
    ${UTILS_SOURCE_DIR}/mpf.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )
#------------------------------------------------
# Build mpcat executable
#
  SET(mpcat_EXE_SOURCES
    ${UTILS_SOURCE_DIR}/mpcat.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )
#------------------------------------------------
# Build mpview executable
#
  SET(mpview_EXE_SOURCES
    ${UTILS_SOURCE_DIR}/mpview.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )
      
FOREACH(target mpd mpd_demix mpr mpf mpcat mpview)
       ADD_EXECUTABLE(${target} ${${target}_EXE_SOURCES})
      SET_TARGET_PROPERTIES(${target} PROPERTIES
			    COMPILE_FLAGS "${SHARED_FLAGS} ${EXECUTABLE_COMPILE_FLAGS}")
      TARGET_LINK_LIBRARIES(${target} mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
      INSTALL(
      TARGETS ${target}
      RUNTIME DESTINATION bin
      )
ENDFOREACH(target)
IF(BUILD_EXPERIMENTAL)
#------------------------------------------------
# Build mppitch executable
#
  SET(mppitch_EXE_SOURCES
    ${UTILS_SOURCE_DIR}/experimental/mppitch.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )
#------------------------------------------------
# Build create default dict executable
#
  SET(createdefaultdict_EXE_SOURCES
    ${UTILS_SOURCE_DIR}/experimental/createdefaultdict.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )

FOREACH(target mppitch createdefaultdict)
       ADD_EXECUTABLE(${target} ${${target}_EXE_SOURCES})
      SET_TARGET_PROPERTIES(${target} PROPERTIES
			    COMPILE_FLAGS "${SHARED_FLAGS} ${EXECUTABLE_COMPILE_FLAGS}")
      TARGET_LINK_LIBRARIES(${target} mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ENDFOREACH(target)
ENDIF(BUILD_EXPERIMENTAL)

#For win32 and plateform and MINGW build command, copy the dll files in the build dir
IF (WIN32)
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
IF(BUILD_DEBUG)          
#=== Copy the dll in the bin folder===
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${PTHREAD_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/debug/pthreadVC2.dll"
        )
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${FFTW3_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/debug/libfftw3-3.dll"
        ) 
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${SNDFILE_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/debug/libsndfile-1.dll"
        )              
                 ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${MPTK_BINARY_DIR}/lib/debug/mptk.dll" 
"${MPTK_BINARY_DIR}/bin/debug/mptk.dll")
       
           
ELSE(BUILD_DEBUG)     
#=== Copy the dll in the bin folder===
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${PTHREAD_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/release/pthreadVC2.dll"
        )
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${FFTW3_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/release/libfftw3-3.dll"
        ) 
                ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${SNDFILE_LIBRARY_FILE}" 
"${MPTK_BINARY_DIR}/bin/release/libsndfile-1.dll"
        )              
                 ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${MPTK_BINARY_DIR}/lib/release/mptk.dll" 
"${MPTK_BINARY_DIR}/bin/release/mptk.dll"
       
       )        
ENDIF(BUILD_DEBUG) 
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
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
                 ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${MPTK_BINARY_DIR}/lib/libmptk.dll" 
"${MPTK_BINARY_DIR}/bin/libmptk.dll"
       
       )          
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
 ENDIF (WIN32)

#------------------------------------------------
# Define install target:
#
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/readme.txt" DESTINATION bin)
#Install dll in the destination folder for Win32 plateform
IF(WIN32)
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
IF(BUILD_DEBUG)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/pthreadVC2.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/libfftw3-3.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/libsndfile-1.dll" DESTINATION bin)
ELSE(BUILD_DEBUG)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/pthreadVC2.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/libfftw3-3.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/libsndfile-1.dll" DESTINATION bin)
ENDIF(BUILD_DEBUG)
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/pthreadVC2.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libfftw3-3.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libsndfile-1.dll" DESTINATION bin)
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF(WIN32)
