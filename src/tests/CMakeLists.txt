# compil test
   if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 OLD)
    endif(COMMAND cmake_policy)

#------------------------------------------------
#Create all target tests executables
#------------------------------------------------
FOREACH(test_target test_anywave test_fft test_messaging test_win test_parabolic_regression test_mp test_mask test_behaviour_in_noise)
	ADD_EXECUTABLE(${test_target} ${MPTK_SOURCE_DIR}/src/tests/${test_target}.cpp)
	SET_TARGET_PROPERTIES(${test_target} PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS}")
	TARGET_LINK_LIBRARIES(${test_target} mptk dsp_windows ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
	ADD_DEPENDENCIES(${test_target} mptk)
ENDFOREACH(test_target)

#------------------------------------------------
# Copy the Matlab tests
#------------------------------------------------
SET(MPTK_MATLABALLTEST_PATH ${MPTK_BINARY_DIR}/bin/matlabTests)
SET(MPTK_MATLABALLTEST_NAME runAllMatlabTests)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/matlabTests/${MPTK_MATLABALLTEST_NAME}.m.in ${MPTK_MATLABALLTEST_PATH}/${MPTK_MATLABALLTEST_NAME}.m @ONLY)

SET(MPTK_MATLABSPECTEST_PATH ${MPTK_MATLABALLTEST_PATH}/SpecificsMatlabTests)
FOREACH(MPTK_MATLABTESTS TestDictread)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/matlabTests/SpecificsMatlabTests/${MPTK_MATLABTESTS}.m.in ${MPTK_MATLABSPECTEST_PATH}/${MPTK_MATLABTESTS}.m @ONLY)
ENDFOREACH(MPTK_MATLABTESTS)

#------------------------------------------------
# Generate the tests scripts
#------------------------------------------------
FOREACH(MPTK_SCRIPTS TestExecCycle TestMatlab)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/script/${MPTK_SCRIPTS}${BATCHEXT}.in ${MPTK_BINARY_DIR}/bin/script/${MPTK_SCRIPTS}${BATCHEXT} @ONLY)
ENDFOREACH(MPTK_SCRIPTS)


#------------------------------------------------
# Copy necessary files for standard tests
#------------------------------------------------
# File used for mpd test
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_signal.wav ${MPTK_REFERENCE_PATH}/signal/anywave_signal.wav COPYONLY)        
# File used for test_mp
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/dico_test.xml ${MPTK_REFERENCE_PATH}/dictionary/dico_test.xml COPYONLY)
INSTALL(FILES ${MPTK_REFERENCE_PATH}/dictionary/dico_test.xml DESTINATION mptk/reference/dictionary)
# File used for test_fft
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/2_cosines.flt ${MPTK_REFERENCE_PATH}/signal/2_cosines.flt COPYONLY)     
INSTALL(FILES ${MPTK_REFERENCE_PATH}/signal/2_cosines.flt DESTINATION mptk/reference/signal)

