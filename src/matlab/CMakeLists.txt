# Build Matlab MEX files
# Test if we are in in X86_64 and then set the -fPIC compil tag
IF(BUILD_MATLAB_MEX_FILES)
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

IF(MEX_COMPILER)



ADD_CUSTOM_TARGET(dictread_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/dictread.${MEX_EXTENSION}
    )
ADD_CUSTOM_TARGET(dictwrite_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/dictwrite.${MEX_EXTENSION}
    )
ADD_CUSTOM_TARGET(bookread_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/bookread.${MEX_EXTENSION}
    )
ADD_CUSTOM_TARGET(bookwrite_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/bookwrite.${MEX_EXTENSION}
    )
ADD_CUSTOM_TARGET(loadenv_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/loadenv.${MEX_EXTENSION}
    )
ADD_CUSTOM_TARGET(releaseenv_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/releaseenv.${MEX_EXTENSION}
    )
    
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
IF (CMAKE_CONFIGURATION_TYPE MATCHES Debug)
ADD_CUSTOM_COMMAND(
TARGET bookread_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookread.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET bookwrite_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3  -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookwrite.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET dictread_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictread.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET dictwrite_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictwrite.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
TARGET loadenv_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/loadenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
  
ADD_CUSTOM_COMMAND(
TARGET releaseenv_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/releaseenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )

ELSE(CMAKE_CONFIGURATION_TYPE MATCHES Debug)

ADD_CUSTOM_COMMAND(
TARGET bookread_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookread.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET bookwrite_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3  -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookwrite.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET dictread_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictread.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET dictwrite_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictwrite.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
TARGET loadenv_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/loadenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
  
ADD_CUSTOM_COMMAND(
TARGET releaseenv_matlab
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/releaseenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )

ENDIF(CMAKE_CONFIGURATION_TYPE MATCHES Debug)
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")

ADD_CUSTOM_COMMAND(
TARGET bookread_matlab
  COMMAND   ${MEX_COMPILER}
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookread.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET bookwrite_matlab
  COMMAND   ${MEX_COMPILER}
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookwrite.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET dictread_matlab
  COMMAND   ${MEX_COMPILER}
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictread.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET dictwrite_matlab
  COMMAND   ${MEX_COMPILER}
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictwrite.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET loadenv_matlab
  COMMAND   ${MEX_COMPILER}
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/loadenv.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
TARGET releaseenv_matlab
  COMMAND   ${MEX_COMPILER}
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/releaseenv.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )


ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/bookread.m ${MPTK_BINARY_DIR}/src/matlab/bookread.m
                 COPYONLY) 

        
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/bookwrite.m ${MPTK_BINARY_DIR}/src/matlab/bookwrite.m
                 COPYONLY) 

                 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/dictread.m ${MPTK_BINARY_DIR}/src/matlab/dictread.m
                 COPYONLY) 

CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/dictwrite.m ${MPTK_BINARY_DIR}/src/matlab/dictwrite.m
                 COPYONLY)   
                 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/loadenv.m ${MPTK_BINARY_DIR}/src/matlab/loadenv.m
                 COPYONLY)   

CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/releaseenv.m ${MPTK_BINARY_DIR}/src/matlab/releaseenv.m
                 COPYONLY)

CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookover.m ${MPTK_BINARY_DIR}/src/matlab/bookover.m
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookplot.m ${MPTK_BINARY_DIR}/src/matlab/bookplot.m
                 COPYONLY) 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/mpview.m ${MPTK_BINARY_DIR}/src/matlab/mpview.m
                 COPYONLY) 
INSTALL(FILES ${MPTK_BINARY_DIR}/src/matlab/dictread.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/dictread.m
              ${MPTK_BINARY_DIR}/src/matlab/dictwrite.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/dictwrite.m
              ${MPTK_BINARY_DIR}/src/matlab/bookread.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/bookread.m
              ${MPTK_BINARY_DIR}/src/matlab/bookwrite.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/bookwrite.m
              ${MPTK_BINARY_DIR}/src/matlab/loadenv.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/loadenv.m
              ${MPTK_BINARY_DIR}/src/matlab/releaseenv.${MEX_EXTENSION}
              ${MPTK_BINARY_DIR}/src/matlab/releaseenv.m
              ${MPTK_BINARY_DIR}/src/matlab/bookover.m
              ${MPTK_BINARY_DIR}/src/matlab/bookplot.m
              ${MPTK_BINARY_DIR}/src/matlab/mpview.m
DESTINATION matlab)
ELSE(MEX_COMPILER) 
MESSAGE("MEX compiler not fond, please set the path to Mex compiler or Matlab Mex files will not be compiled" )
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/dictread.m ${MPTK_BINARY_DIR}/src/matlab/dictread.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/dictwrite.m ${MPTK_BINARY_DIR}/src/matlab/dictwrite.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookwrite.m ${MPTK_BINARY_DIR}/src/matlab/bookwrite.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookread.m ${MPTK_BINARY_DIR}/src/matlab/bookread.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookover.m ${MPTK_BINARY_DIR}/src/matlab/bookover.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookplot.m ${MPTK_BINARY_DIR}/src/matlab/bookplot.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/mpview.m ${MPTK_BINARY_DIR}/src/matlab/mpview.m
                 COPYONLY) 
 INSTALL(FILES ${MPTK_BINARY_DIR}/src/matlab/dictread.m
              ${MPTK_BINARY_DIR}/src/matlab/dictwrite.m
              ${MPTK_BINARY_DIR}/src/matlab/bookread.m
              ${MPTK_BINARY_DIR}/src/matlab/bookwrite.m
              ${MPTK_BINARY_DIR}/src/matlab/bookover.m
              ${MPTK_BINARY_DIR}/src/matlab/bookplot.m
              ${MPTK_BINARY_DIR}/src/matlab/mpview.m
DESTINATION matlab)
ENDIF(MEX_COMPILER)
ENDIF(BUILD_MATLAB_MEX_FILES)

# GOTO EXPERIMENTAL MEX FILES SUBDIRECTORY
ADD_SUBDIRECTORY (experimental)


