# compil test
   if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 OLD)
    endif(COMMAND cmake_policy)

#------------------------------------------------
#Create all target tests executables
#------------------------------------------------
FOREACH(test_target test_anywave test_fft test_messaging test_win test_parabolic_regression test_mp test_mask test_behaviour_in_noise test_convolution test_windowLen)
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
FOREACH(MPTK_MATLABTESTS TestDictionaries TestBooks TestSignal TestDecompReconst)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/matlabTests/SpecificsMatlabTests/${MPTK_MATLABTESTS}.m.in ${MPTK_MATLABSPECTEST_PATH}/${MPTK_MATLABTESTS}.m @ONLY)
ENDFOREACH(MPTK_MATLABTESTS)

#------------------------------------------------
# Generate the tests scripts
#------------------------------------------------
FOREACH(MPTK_SCRIPTS TestExecCycle TestMatlab TestStdin)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/script/${MPTK_SCRIPTS}${BATCHEXT}.in ${MPTK_BINARY_DIR}/bin/script/${MPTK_SCRIPTS}${BATCHEXT} @ONLY)
ENDFOREACH(MPTK_SCRIPTS)


#------------------------------------------------
# Copy necessary files for standard tests
#------------------------------------------------
# File used for test_mp
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/dic_test.xml ${MPTK_REFERENCE_PATH}/dictionary/dic_test.xml COPYONLY)
# File used for test_fft
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/2_cosines.flt ${MPTK_REFERENCE_PATH}/signal/2_cosines.flt COPYONLY)
# File used for test_convolution
SET(MPTK_ANYWAVE_RESULT_1 ${MPTK_REFERENCE_PATH}/misc/anywave_results_1.bin PARENT_SCOPE)
SET(MPTK_ANYWAVE_RESULT_2 ${MPTK_REFERENCE_PATH}/misc/anywave_results_2.bin PARENT_SCOPE)
SET(MPTK_ANYWAVE_RESULT_3 ${MPTK_REFERENCE_PATH}/misc/anywave_results_3.bin PARENT_SCOPE)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_results_1.bin ${MPTK_REFERENCE_PATH}/misc/anywave_results_1.bin COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_results_2.bin ${MPTK_REFERENCE_PATH}/misc/anywave_results_2.bin COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/tests/signals/anywave_results_3.bin ${MPTK_REFERENCE_PATH}/misc/anywave_results_3.bin COPYONLY)