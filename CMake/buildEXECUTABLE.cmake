CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/utils/readme.txt ${MPTK_BINARY_DIR}/bin/readme.txt COPYONLY)
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
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpd_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mpd_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpd_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(mpd_multithread mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpd_multithread mpd-executable)
ELSE(BUILD_MULTITHREAD)
ADD_EXECUTABLE(mpd ${MPD_EXE_SOURCES})
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpd PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mpd PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpd PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
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
                 ADD_CUSTOM_COMMAND (
                        TARGET mpd
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND}
                        ARGS -E copy "${MPTK_BINARY_DIR}/lib/libmptk.dll" 
"${MPTK_BINARY_DIR}/bin/libmptk.dll"
       
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
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpd_demix_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mpd_demix_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpd_demix_multithread PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(mpd_demix_multithread mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpd_demix_multithread mpd_demix-executable)
ELSE(BUILD_MULTITHREAD)
ADD_EXECUTABLE(mpd_demix ${MPD_DEMIX_EXE_SOURCES})
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpd_demix PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mpd_demix PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpd_demix PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
TARGET_LINK_LIBRARIES(mpd_demix mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpd_demix mpd_demix-executable)
ENDIF(BUILD_MULTITHREAD)
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
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpr PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
IF(MINGW)
SET_TARGET_PROPERTIES(mpr PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpr PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
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
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpf PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mpf PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpf PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
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
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpcat PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mpcat PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpcat PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
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
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mpview PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mpview PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mpview PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(mpview mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mpview mpview-executable)
#------------------------------------------------
# Build mppitch executable
#
IF(BUILD_EXPERIMENTAL)
MACRO(GET_MPPITCH_CPP_SOURCES out)
  SET(${out}
    ${UTILS_SOURCE_DIR}/experimental/mppitch.cpp
    ${UTILS_SOURCE_DIR}/getopt.c
    ${UTILS_SOURCE_DIR}/getopt1.c
    ${UTILS_SOURCE_DIR}/getopt.h
    ${MPTK_BINARY_DIR}/src/libmptk/mptk.h
      )
ENDMACRO(GET_MPPITCH_CPP_SOURCES)
GET_MPPITCH_CPP_SOURCES(MPPITCH_EXE_SOURCES)
ADD_CUSTOM_TARGET(mppitch-executable DEPENDS ${MPPITCH_EXE_SOURCES})
ADD_EXECUTABLE(mppitch ${MPPITCH_EXE_SOURCES})
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(mppitch PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(mppitch PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(mppitch PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(mppitch mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(mppitch mppitch-executable)


ADD_EXECUTABLE(createDefaultDict ${UTILS_SOURCE_DIR}/experimental/createdefaultdict.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(createDefaultDict PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(createDefaultDict PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(createDefaultDict PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(createDefaultDict mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})



ENDIF(BUILD_EXPERIMENTAL)


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
 RUNTIME DESTINATION bin
)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/readme.txt" DESTINATION bin)
#Install dll in the destination folder for Win32 plateform
IF(MINGW)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/pthreadVC2.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libfftw3-3.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libsndfile-1.dll" DESTINATION bin)
ENDIF(MINGW)
ELSE(BUILD_MULTITHREAD)
INSTALL(TARGETS
  mpd
  mpd_demix
  mpr
  mpf
  mpcat
  mpview
 RUNTIME DESTINATION bin
)
#Install dll in the destination folder for Win32 plateform
IF(MINGW)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/pthreadVC2.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libfftw3-3.dll" DESTINATION bin)
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libsndfile-1.dll" DESTINATION bin)
ENDIF(MINGW)
ENDIF(BUILD_MULTITHREAD)
