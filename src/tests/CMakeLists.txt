# compil test
   if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 OLD)
    endif(COMMAND cmake_policy)

# Loop on all target tests
FOREACH(test_target 
        test_anywave test_fft test_messaging test_win test_parabolic_regression test_mp test_mask test_behaviour_in_noise)

ADD_EXECUTABLE(${test_target} ${MPTK_SOURCE_DIR}/src/tests/${test_target}.cpp)
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(${test_target} PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic -Wno-deprecated -fPIC")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF(MINGW)
SET_TARGET_PROPERTIES(${test_target} PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -Wno-deprecated")
ELSE(MINGW)
SET_TARGET_PROPERTIES(${test_target} PROPERTIES COMPILE_FLAGS "${SHARED_FLAGS} -rdynamic")
ENDIF(MINGW)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
TARGET_LINK_LIBRARIES(${test_target} mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ADD_DEPENDENCIES(${test_target} mptk)

ENDFOREACH(test_target)

IF(LINUX)
ADD_CUSTOM_COMMAND(TARGET test_messaging
                     POST_BUILD
                     COMMAND ${MPTK_BINARY_DIR}/bin/test_messaging)
ELSE(LINUX)
IF(MINGW)
ADD_CUSTOM_COMMAND(TARGET test_messaging
                     POST_BUILD
                  COMMAND ${MPTK_BINARY_DIR}/bin/test_messaging)
ENDIF(MINGW)  
ENDIF(LINUX)

####
#### Copy necessary files
####

SET(MPTK_ANYWAVE_TEST_TABLE_BIN ${MPTK_REFERENCE_PATH}/signal/anywave_table_data.bin)


CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_signal.wav ${MPTK_REFERENCE_PATH}/signal/anywave_signal.wav
                 COPYONLY)               
# File used for test_mp
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/dico_test.xml ${MPTK_REFERENCE_PATH}/dictionary/dico_test.xml
                 COPYONLY)
INSTALL(FILES
	${MPTK_REFERENCE_PATH}/dictionary/dico_test.xml
	DESTINATION
	mptk/reference/dictionary)


# File used for test_fft
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/2_cosines.flt ${MPTK_REFERENCE_PATH}/signal/2_cosines.flt
                 COPYONLY)     
INSTALL(FILES
	${MPTK_REFERENCE_PATH}/signal/2_cosines.flt
	DESTINATION
	mptk/reference/signal)

