######
######   LOOKING FOR EXTERNAL LIBRARIES
######

#------------------------------------------------ 
# Finding fftw3
INCLUDE (${MPTK_SOURCE_DIR}/CMake/FindFFTW3.cmake)
IF(NOT FFTW3_INCLUDE_FOUND)
	MESSAGE(STATUS "Please set FFTW3_INCLUDE_DIR or code using FFTW3 will not be compiled")
ENDIF(NOT FFTW3_INCLUDE_FOUND)
IF(NOT FFTW3_LIB_FOUND)
	MESSAGE(STATUS "Please set FFTW3_LIBRARY_FILE or code using FFTW3 will not be compiled")
ENDIF(NOT FFTW3_LIB_FOUND)
#------------------------------------------------
# Finding sndfile
INCLUDE (${MPTK_SOURCE_DIR}/CMake/FindsndFILE.cmake)
IF(NOT SNDFILE_INCLUDE_FOUND)
  MESSAGE(STATUS "Please set SNDFILE_INCLUDE_DIR or code using sndFILE will not be compiled")
ENDIF(NOT SNDFILE_INCLUDE_FOUND)
IF(NOT SNDFILE_LIB_FOUND)
  MESSAGE(STATUS "Please set SNDFILE_LIBRARY_FILE or code using sndFILE will not be compiled")
ENDIF(NOT SNDFILE_LIB_FOUND)
#------------------------------------------------
# Finding PTHREAD
INCLUDE (${MPTK_SOURCE_DIR}/CMake/FindPTHREAD.cmake)
IF(NOT PTHREAD_FOUND)
  MESSAGE(STATUS "Please set  PTHREAD_LIBRARY_FILE or multithread will be disable")
ENDIF(NOT PTHREAD_FOUND)
#------------------------------------------------
# Finding Doxygen
IF(BUILD_DOCUMENTATION)
INCLUDE (${MPTK_SOURCE_DIR}/CMake/FindDoxygen.cmake)
IF(NOT DOXYGEN_FOUND)
  MESSAGE(STATUS "Please set DOXYGEN path to doxygen exe or Documentation will not be generated")
ENDIF(NOT DOXYGEN_FOUND)
IF(NOT DOT_FOUND)
  MESSAGE(STATUS "Please set DOT path to dot exe or Graph for Documentation will not be generated")
ENDIF(NOT DOT_FOUND)
IF(NOT DOT_PATH_FOUND)
  MESSAGE(STATUS "Please set DOT path to dot or Graph for Documentation will not be generated")
ENDIF(NOT DOT_PATH_FOUND)
ENDIF(BUILD_DOCUMENTATION)


