# Build Matlab MEX EXPERIMENTAL files
# Test if we are in in X86_64 and then set the -fPIC compil tag
IF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
 ADD_DEFINITIONS(-fPIC)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
# MATLAB_INCLUDE_DIR: include path for mex.h, engine.h
#  MATLAB_LIBRARIES:   required libraries: libmex, etc
#  MATLAB_MEX_LIBRARY: path to libmex.lib
#  MATLAB_MX_LIBRARY:  path to libmx.lib
#  MATLAB_ENG_LIBRARY: path to libeng.lib

# SET MEX files extension
IF(UNIX)
IF(APPLE)
IF(CMAKE_OSX_ARCHITECTURES MATCHES i386)
SET(MEX_EXTENSION mexmaci)
ELSE(CMAKE_OSX_ARCHITECTURES MATCHES i386)
SET(MEX_EXTENSION mexmac)
ENDIF(CMAKE_OSX_ARCHITECTURES MATCHES i386)
ELSE(APPLE)
SET(MEX_EXTENSION mexglx)
ENDIF(APPLE)
INCLUDE(FindMatlab)
FIND_PROGRAM(MEX_COMPILER mex)

# COMPILE MEX Files
IF(MEX_COMPILER)

 ADD_CUSTOM_TARGET(bookread_exp_matlab ALL echo
        DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.${MEX_EXTENSION}
  )
 ADD_CUSTOM_TARGET(bookwrite_exp_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.${MEX_EXTENSION}
  )
 ADD_CUSTOM_TARGET(bookmpr_exp_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.${MEX_EXTENSION}
  )
 
 ADD_CUSTOM_COMMAND(
   TARGET bookread_exp_matlab
   COMMAND   ${MEX_COMPILER}
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookread_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
   COMMENT   "MEX"
 )
  
ADD_CUSTOM_COMMAND(
    TARGET bookwrite_exp_matlab
    COMMAND   ${MEX_COMPILER}
    ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookwrite_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
    COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
    TARGET bookmpr_exp_matlab
    COMMAND   ${MEX_COMPILER}
    ARGS      -g -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookmpr_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
    COMMENT   "MEX"
  )

  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/bookread_exp.m ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.m
                 COPYONLY) 
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/bookwrite_exp.m ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.m
                 COPYONLY) 
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/bookedit_exp.m ${MPTK_BINARY_DIR}/src/matlab/experimental/bookedit_exp.m
                 COPYONLY) 
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/bookedit_exp.m ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.m
                 COPYONLY) 
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/MPtoolbaricons.mat ${MPTK_BINARY_DIR}/src/matlab/experimental/MPtoolbaricons.mat
                 COPYONLY) 


ELSE(MEX_COMPILER) 
MESSAGE("MEX compiler not fond, please set the path to Mex compiler or Matlab Mex files will not be compiled" )
ENDIF(MEX_COMPILER)

ELSE(UNIX) # INSTALL SCRIPT VERSION FOR WINDOWS


#Not actually implemented on windows
#FIND_PROGRAM(MEX_COMPILER mex)
#IF(MEX_COMPILER)
#ADD_CUSTOM_TARGET(dictread_matlab ALL echo
#      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/dictread.dll
#    )
#  ADD_CUSTOM_COMMAND(
#TARGET dictread_matlab
#  COMMAND   ${MEX_COMPILER}
#  ARGS      -f ${MPTK_SOURCE_DIR}/src/matlab/mexopts.bat -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictread.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
#      COMMENT   "MEX"
#  )


ENDIF(MEX_COMPILER)
ENDIF(UNIX)
IF(BUILD_SHARED_LIBS)
IF(UNIX)
  INSTALL(FILES ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookedit_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/MPtoolbaricons.mat
  DESTINATION matlab/experimental)

ELSE(UNIX)
  INSTALL(FILES ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.m
                ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.m
                ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.m
                ${MPTK_BINARY_DIR}/src/matlab/experimental/bookedit_exp.m
                ${MPTK_BINARY_DIR}/src/matlab/experimental/MPtoolbaricons.mat
  DESTINATION matlab/experimental)

ENDIF(UNIX)
ENDIF(BUILD_SHARED_LIBS)
ENDIF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)



