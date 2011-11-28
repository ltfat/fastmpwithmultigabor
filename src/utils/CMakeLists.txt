if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#------------------------------------------------
# Build base64 library
#------------------------------------------------
ADD_SUBDIRECTORY (libbase64)

#------------------------------------------------
# Build dsp_windows library
#------------------------------------------------
ADD_SUBDIRECTORY (libdsp_windows)

#------------------------------------------------
# Build tiny_xml library
#------------------------------------------------
ADD_SUBDIRECTORY (libtinyxml)

#------------------------------------------------
# Build md5sum library
#------------------------------------------------
ADD_SUBDIRECTORY (libmd5sum)

#------------------------------------------------
# Build getopt library
#------------------------------------------------
ADD_SUBDIRECTORY (libgetopt)

#------------------------------------------------
# Build main executables      
#------------------------------------------------
SET(EXECUTABLES mpd mpd_demix mpr mpf mpcat mpview gpd cmpd)
SET(EXEC_TARGETS ${EXECUTABLES} PARENT_SCOPE)
FOREACH(LoopTargets ${EXECUTABLES})
	SET(${LoopTargets}_EXE_SOURCES
    		${UTILS_SOURCE_DIR}/${LoopTargets}.cpp
    		${MPTK_BINARY_DIR}/src/libmptk/mptk.h )
	ADD_EXECUTABLE(${LoopTargets} ${${LoopTargets}_EXE_SOURCES})
	SET_TARGET_PROPERTIES(${LoopTargets} PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")
	TARGET_LINK_LIBRARIES(${LoopTargets} mptk dsp_windows getopt)
	INSTALL(TARGETS ${LoopTargets} RUNTIME DESTINATION bin)
ENDFOREACH(LoopTargets)

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
		TARGET_LINK_LIBRARIES(${target} mptk dsp_windows getopt)
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
                            ARGS -E copy "${MPTK_BINARY_DIR}/lib/${MPTK_DEBUG_RELEASE}/mptk.dll" 
                            "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/mptk.dll")
ENDIF(WIN32)

#------------------------------------------------
# Define install target:
#------------------------------------------------
#Install dll in the destination folder for Win32 plateform
IF(WIN32)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/pthreadVC2.dll" DESTINATION bin)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libfftw3-3.dll" DESTINATION bin)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libsndfile-1.dll" DESTINATION bin)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/pthreadVC2.dll" DESTINATION mptk/matlab)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libfftw3-3.dll" DESTINATION mptk/matlab)
	INSTALL(FILES "${MPTK_BINARY_DIR}/bin/${MPTK_DEBUG_RELEASE}/libsndfile-1.dll" DESTINATION mptk/matlab)
ENDIF(WIN32)