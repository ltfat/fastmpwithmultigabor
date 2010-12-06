if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/utils/readme.txt ${MPTK_BINARY_DIR}/bin/readme.txt COPYONLY)

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
FOREACH(target mpd mpd_demix mpr mpf mpcat mpview gpd)
	SET(${target}_EXE_SOURCES
    		${UTILS_SOURCE_DIR}/${target}.cpp
    		${MPTK_BINARY_DIR}/src/libmptk/mptk.h )
	ADD_EXECUTABLE(${target} ${${target}_EXE_SOURCES})
	SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")
	TARGET_LINK_LIBRARIES(${target} mptk dsp_windows getopt)
	INSTALL(TARGETS ${target} RUNTIME DESTINATION bin)
ENDFOREACH(target)

#------------------------------------------------
# Build experimental      
#------------------------------------------------
IF(BUILD_EXPERIMENTAL)
	# Build experimental targets
	FOREACH(target mppitch createdefaultdict)
		SET(${target}_EXE_SOURCES
    			${UTILS_SOURCE_DIR}/experimental/${target}.cpp
			${MPTK_BINARY_DIR}/src/libmptk/mptk.h)
		ADD_EXECUTABLE(${target} ${${target}_EXE_SOURCES})
		SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")
		TARGET_LINK_LIBRARIES(${target} mptk dsp_windows getopt ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
	ENDFOREACH(target)
ENDIF(BUILD_EXPERIMENTAL)

#For win32 and platforms and MINGW build command, copy the dll files in the build dir
IF(WIN32)
      #=== Copy the dll in the bin folder===
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${PTHREAD_LIBRARY_FILE}/../pthreadVC2.dll" 
                            "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/pthreadVC2.dll")
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${FFTW3_LIBRARY_FILE}/../libfftw3-3.dll" 
                            "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libfftw3-3.dll") 
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${SNDFILE_LIBRARY_FILE}/../libsndfile-1.dll" 
                            "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libsndfile-1.dll")              
      ADD_CUSTOM_COMMAND (
                            TARGET mpd
                            POST_BUILD
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy "${MPTK_BINARY_DIR}/lib/debug/mptk.dll" 
                            "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/mptk.dll")
ENDIF(WIN32)

#------------------------------------------------
# Define install target:
#------------------------------------------------
INSTALL(FILES "${MPTK_BINARY_DIR}/bin/readme.txt" DESTINATION bin)
#Install dll in the destination folder for Win32 plateform
IF(WIN32)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/pthreadVC2.dll" DESTINATION bin)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libfftw3-3.dll" DESTINATION bin)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libsndfile-1.dll" DESTINATION bin)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/pthreadVC2.dll" DESTINATION mptk/matlab)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libfftw3-3.dll" DESTINATION mptk/matlab)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libsndfile-1.dll" DESTINATION mptk/matlab)
ENDIF(WIN32)