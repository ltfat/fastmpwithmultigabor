if(COMMAND cmake_policy)
      cmake_policy(SET CMP0002 OLD)
endif(COMMAND cmake_policy)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

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
INCLUDE(FindMatlab)
FIND_PROGRAM(MEX_COMPILER mex)
ELSE(APPLE)
SET(MEX_EXTENSION mexglx)
INCLUDE(FindMatlab)
FIND_PROGRAM(MEX_COMPILER mex)
ENDIF(APPLE)
ELSE(UNIX)
IF(WIN32)
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
SET(MEX_EXTENSION mexw32)
INCLUDE(FindMatlab)
FIND_PROGRAM(MEX_COMPILER mex)
MESSAGE("Please set mex compiler with Visual Studio using Matlab command line and mex -setup command")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
MESSAGE("Matlab Mex files are only supported by MS Visual Studio")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF(WIN32)
ENDIF(UNIX)

# COMPILE MEX Files
IF(MEX_COMPILER)

 ADD_CUSTOM_TARGET(mptk_getinfo_exp_matlab ALL echo
        DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/experimental/mptk_getinfo_exp.${MEX_EXTENSION}
  )
 ADD_CUSTOM_TARGET(bookread_exp_matlab ALL echo
        DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.${MEX_EXTENSION}
  )
 ADD_CUSTOM_TARGET(bookwrite_exp_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.${MEX_EXTENSION}
  )
 ADD_CUSTOM_TARGET(bookmpr_exp_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.${MEX_EXTENSION}
  )
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
IF(CMAKE_CONFIGURATION_TYPE MATCHES Debug)

 ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/mptk_getinfo_exp.${MEX_EXTENSION}
   POST_BUILD
   COMMAND   ${MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/mptk_getinfo_exp.cpp  -L${MPTK_BINARY_DIR}/lib/debug -lmptk
   COMMENT   "MEX"
 )
  
 ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.${MEX_EXTENSION}
   POST_BUILD
   COMMAND   ${MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookread_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
   COMMENT   "MEX"
 )
  
ADD_CUSTOM_COMMAND(
    OUTPUT  ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.${MEX_EXTENSION}
    POST_BUILD
    COMMAND   ${MEX_COMPILER} -v
    ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookwrite_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
    COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
    OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.${MEX_EXTENSION}
    POST_BUILD
    COMMAND   ${MEX_COMPILER} -v
    ARGS      -g -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookmpr_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
    COMMENT   "MEX"
  )
  
ELSE(CMAKE_CONFIGURATION_TYPE MATCHES Debug)
  ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/mptk_getinfo_exp.${MEX_EXTENSION}
   POST_BUILD
   COMMAND   ${MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/mptk_getinfo_exp.cpp  -L${MPTK_BINARY_DIR}/lib/release -lmptk
   COMMENT   "MEX"
 )

 ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.${MEX_EXTENSION}
   POST_BUILD
   COMMAND   ${MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookread_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
   COMMENT   "MEX"
 )
  
ADD_CUSTOM_COMMAND(
    OUTPUT  ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.${MEX_EXTENSION}
    POST_BUILD
    COMMAND   ${MEX_COMPILER} -v
    ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookwrite_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
    COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
    OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.${MEX_EXTENSION}
    POST_BUILD
    COMMAND   ${MEX_COMPILER} -v
    ARGS      -g -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookmpr_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
    COMMENT   "MEX"
  )

ENDIF(CMAKE_CONFIGURATION_TYPE MATCHES Debug)
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")

 ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/mptk_getinfo_exp.${MEX_EXTENSION}
   COMMAND   ${MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/mptk_getinfo_exp.cpp  -L${MPTK_BINARY_DIR}/lib -lmptk
   COMMENT   "MEX"
 )

 ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.${MEX_EXTENSION}
   COMMAND   ${MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookread_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
   COMMENT   "MEX"
 )
  
ADD_CUSTOM_COMMAND(
    OUTPUT  ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.${MEX_EXTENSION}
    POST_BUILD
    COMMAND   ${MEX_COMPILER} -v
    ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookwrite_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
    COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
    OUTPUT ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.${MEX_EXTENSION}
    POST_BUILD
    COMMAND   ${MEX_COMPILER} -v
    ARGS      -g -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/experimental/bookmpr_exp.cpp ${MPTK_SOURCE_DIR}/src/matlab/experimental/classes/mxBook.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
    COMMENT   "MEX"
  )

ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")

  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/dictcreate_gui.m ${MPTK_BINARY_DIR}/src/matlab/experimental/dictcreate_gui.m
                 COPYONLY) 
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/mptk_getinfo_exp.m ${MPTK_BINARY_DIR}/src/matlab/experimental/mptk_getinfo_exp.m
                 COPYONLY) 
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
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/experimental/README ${MPTK_BINARY_DIR}/src/matlab/experimental/README
                 COPYONLY) 
                 
  INSTALL(FILES ${MPTK_BINARY_DIR}/src/matlab/experimental/mptk_getinfo_exp.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/experimental/mptk_getinfo_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/dictcreate_gui.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookread_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookwrite_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookmpr_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/bookedit_exp.m
              ${MPTK_BINARY_DIR}/src/matlab/experimental/MPtoolbaricons.mat
              ${MPTK_BINARY_DIR}/src/matlab/experimental/README
  DESTINATION matlab/experimental)

ELSE(MEX_COMPILER) 
MESSAGE("MEX compiler not fond, please set the path to Mex compiler or Matlab Mex files will not be compiled" )
ENDIF(MEX_COMPILER)
ENDIF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)



