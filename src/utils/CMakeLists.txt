if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/utils/readme.txt ${MPTK_BINARY_DIR}/bin/readme.txt COPYONLY)

#In case of 64 bits plateform we have to compil with -fPIC flag
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
	IF( CMAKE_CXX_COMPILER STREQUAL "cl")
		SET(EXECUTABLE_CXX_COMPILE_FLAGS " -fPIC")
	ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
		SET(EXECUTABLE_CXX_COMPILE_FLAGS "-rdynamic -Wno-deprecated -fPIC")
		SET(EXECUTABLE_C_COMPILE_FLAGS "-rdynamic -fPIC")
	ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
	IF( CMAKE_CXX_COMPILER STREQUAL "cl")
		SET(EXECUTABLE_CXX_COMPILE_FLAGS "")
	ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
		SET(EXECUTABLE_CXX_COMPILE_FLAGS  "-rdynamic -Wno-deprecated")
		SET(EXECUTABLE_C_COMPILE_FLAGS  "-rdynamic")
	ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 

	
#------------------------------------------------
# Build getopt library
#------------------------------------------------
SET(GETOPT_SOURCES
	${UTILS_SOURCE_DIR}/getopt.c
    	${UTILS_SOURCE_DIR}/getopt1.c
    	${UTILS_SOURCE_DIR}/getopt.h )
ADD_LIBRARY(getopt STATIC ${GETOPT_SOURCES})
SET_TARGET_PROPERTIES(getopt PROPERTIES
	COMPILE_FLAGS "${EXECUTABLE_C_COMPILE_FLAGS}"
	PREFIX ""
	OUTPUT_NAME "libgetopt" )

#------------------------------------------------
# Build main executables      
#------------------------------------------------
FOREACH(target mpd mpd_demix mpr mpf mpcat mpview)
	SET(${target}_EXE_SOURCES
    		${UTILS_SOURCE_DIR}/${target}.cpp
    		${MPTK_BINARY_DIR}/src/libmptk/mptk.h )
	ADD_EXECUTABLE(${target} ${${target}_EXE_SOURCES})
	SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "${EXECUTABLE_CXX_COMPILE_FLAGS}")
	TARGET_LINK_LIBRARIES(${target} mptk dsp_windows getopt )
	INSTALL(
		TARGETS ${target}
		RUNTIME DESTINATION bin)
ENDFOREACH(target)

#------------------------------------------------
# Build experimental      
#------------------------------------------------
IF(BUILD_EXPERIMENTAL)
	# Build experimental targets
	FOREACH(target mppitch createdefaultdict)
		SET(${target}_EXE_SOURCES
    			${UTILS_SOURCE_DIR}/experimental/${target}.cpp
   			${UTILS_SOURCE_DIR}/getopt.c
			${UTILS_SOURCE_DIR}/getopt1.c
			${UTILS_SOURCE_DIR}/getopt.h
			${MPTK_BINARY_DIR}/src/libmptk/mptk.h)
		ADD_EXECUTABLE(${target} ${${target}_EXE_SOURCES})
		SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} ${EXECUTABLE_C_COMPILE_FLAGS}")
		TARGET_LINK_LIBRARIES(${target} mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
	ENDFOREACH(target)
ENDIF(BUILD_EXPERIMENTAL)

#For win32 and platforms and MINGW build command, copy the dll files in the build dir
IF (WIN32)
  IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    IF(BUILD_DEBUG)          
      #=== Copy the dll in the bin folder===
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${PTHREAD_LIBRARY_FILE}/../pthreadVC2.dll" 
                            "${MPTK_BINARY_DIR}/bin/debug/pthreadVC2.dll")
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${FFTW3_LIBRARY_FILE}/../libfftw3-3.dll" 
                            "${MPTK_BINARY_DIR}/bin/debug/libfftw3-3.dll") 
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${SNDFILE_LIBRARY_FILE}/../libsndfile-1.dll" 
                            "${MPTK_BINARY_DIR}/bin/debug/libsndfile-1.dll")              
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
                            ARGS -E copy "${PTHREAD_LIBRARY_FILE}/../pthreadVC2.dll" 
                            "${MPTK_BINARY_DIR}/bin/release/pthreadVC2.dll")
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${FFTW3_LIBRARY_FILE}/../libfftw3-3.dll" 
                            "${MPTK_BINARY_DIR}/bin/release/libfftw3-3.dll") 
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${SNDFILE_LIBRARY_FILE}/../libsndfile-1.dll" 
                            "${MPTK_BINARY_DIR}/bin/release/libsndfile-1.dll")              
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${MPTK_BINARY_DIR}/lib/release/mptk.dll" 
                            "${MPTK_BINARY_DIR}/bin/release/mptk.dll")        
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
#------------------------------------------------
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/readme.txt" DESTINATION bin)
#Install dll in the destination folder for Win32 plateform
IF(WIN32)
  IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    IF(BUILD_DEBUG)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/pthreadVC2.dll" DESTINATION bin)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/libfftw3-3.dll" DESTINATION bin)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/libsndfile-1.dll" DESTINATION bin)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/pthreadVC2.dll" DESTINATION mptk/matlab)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/libfftw3-3.dll" DESTINATION mptk/matlab)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/debug/libsndfile-1.dll" DESTINATION mptk/matlab)
    ELSE(BUILD_DEBUG)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/pthreadVC2.dll" DESTINATION bin)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/libfftw3-3.dll" DESTINATION bin)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/libsndfile-1.dll" DESTINATION bin)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/pthreadVC2.dll" DESTINATION mptk/matlab)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/libfftw3-3.dll" DESTINATION mptk/matlab)
      INSTALL(FILES "${MPTK_BINARY_DIR}/bin/release/libsndfile-1.dll" DESTINATION mptk/matlab)
    ENDIF(BUILD_DEBUG)
  ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
    INSTALL(FILES "${MPTK_BINARY_DIR}/bin/pthreadVC2.dll" DESTINATION bin)
    INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libfftw3-3.dll" DESTINATION bin)
    INSTALL(FILES "${MPTK_BINARY_DIR}/bin/libsndfile-1.dll" DESTINATION bin)
  ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF(WIN32)