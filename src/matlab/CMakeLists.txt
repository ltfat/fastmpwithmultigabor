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

# SET path for install
IF(MEX_COMPILER)   
ADD_CUSTOM_TARGET(dictread_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/dictread.${MEX_EXTENSION}
    )
SET_TARGET_PROPERTIES(dictread_matlab PROPERTIES GENERATED true )
ADD_CUSTOM_TARGET(dictwrite_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/dictwrite.${MEX_EXTENSION}
    )
ADD_CUSTOM_TARGET(bookread_matlab ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/src/matlab/bookread.${MEX_EXTENSION}
    )
SET_TARGET_PROPERTIES(bookread_matlab PROPERTIES GENERATED true )
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
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/bookread.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookread.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/bookwrite.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3  -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookwrite.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/dictread.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictread.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/dictwrite.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictwrite.cpp -L${MPTK_BINARY_DIR}/lib/debug -lmptk
      COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/loadenv.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/loadenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
  
ADD_CUSTOM_COMMAND(
 OUTPUT ${MPTK_BINARY_DIR}/src/matlab/releaseenv.${MEX_EXTENSION}
 POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/releaseenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )

ELSE(CMAKE_CONFIGURATION_TYPE MATCHES Debug)

ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/bookread.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookread.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/bookwrite.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3  -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookwrite.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/dictread.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictread.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/dictwrite.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictwrite.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )

ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/loadenv.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/loadenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )
  
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/releaseenv.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/releaseenv.cpp -L${MPTK_BINARY_DIR}/lib/release -lmptk
      COMMENT   "MEX"
  )

ENDIF(CMAKE_CONFIGURATION_TYPE MATCHES Debug)
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")

ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/bookread.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookread.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/bookwrite.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/bookwrite.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/dictread.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictread.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/dictwrite.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/dictwrite.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/loadenv.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
  ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ ${MPTK_SOURCE_DIR}/src/matlab/loadenv.cpp -L${MPTK_BINARY_DIR}/lib -lmptk
      COMMENT   "MEX"
  )
ADD_CUSTOM_COMMAND(
  OUTPUT ${MPTK_BINARY_DIR}/src/matlab/releaseenv.${MEX_EXTENSION}
  POST_BUILD
  COMMAND   ${MEX_COMPILER} -v
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
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/create_synthetic_data.m ${MPTK_BINARY_DIR}/src/matlab/create_synthetic_data.m
                 COPYONLY) 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/savetable.m ${MPTK_BINARY_DIR}/src/matlab/savetable.m
                 COPYONLY)   
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/README ${MPTK_BINARY_DIR}/src/matlab/README
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
              ${MPTK_BINARY_DIR}/src/matlab/create_synthetic_data.m
              ${MPTK_BINARY_DIR}/src/matlab/savetable.m
              ${MPTK_BINARY_DIR}/src/matlab/README
DESTINATION matlab)

ELSE(MEX_COMPILER) 
MESSAGE("MEX compiler not fond, please set the path to Mex compiler or Matlab Mex files will not be compiled" )
MESSAGE("Please not that on Microsft Windows platform, only Visual Studio supports Matlab Mex compiler" )
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
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/create_synthetic_data.m ${MPTK_BINARY_DIR}/src/matlab/create_synthetic_data.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/savetable.m ${MPTK_BINARY_DIR}/src/matlab/savetable.m
                 COPYONLY)           
 INSTALL(FILES ${MPTK_BINARY_DIR}/src/matlab/dictread.m
              ${MPTK_BINARY_DIR}/src/matlab/dictwrite.m
              ${MPTK_BINARY_DIR}/src/matlab/bookread.m
              ${MPTK_BINARY_DIR}/src/matlab/bookwrite.m
              ${MPTK_BINARY_DIR}/src/matlab/bookover.m
              ${MPTK_BINARY_DIR}/src/matlab/bookplot.m
              ${MPTK_BINARY_DIR}/src/matlab/mpview.m
              ${MPTK_BINARY_DIR}/src/matlab/create_synthetic_data.m
              ${MPTK_BINARY_DIR}/src/matlab/savetable.m
DESTINATION matlab)
ENDIF(MEX_COMPILER)
ENDIF(BUILD_MATLAB_MEX_FILES)

IF(INSTALL_MAT2MPTK)                      
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/InstallMat2MPTK.in.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/InstallMat2MPTK.m @ONLY)                            
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/InstallMat2MPTK.in.install.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/for_install_only/InstallMat2MPTK.m @ONLY)     
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/GenerDict.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/GenerDict.m
                 COPYONLY)   
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/Mat2MPTKLoadSettings.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/Mat2MPTKLoadSettings.m
                 COPYONLY)      
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/Mat2MPTKUpdateOptions.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/Mat2MPTKUpdateOptions.m
                 COPYONLY)  
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/Mat2MPTKdemo.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/Mat2MPTKdemo.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpd.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpd.m
                 COPYONLY)  
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/ReadDecay.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/ReadDecay.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpf.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpf.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpf_wrap.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpf_wrap.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpd_wrap.in.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpd_wrap.m @ONLY)   
  
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpd_wrap.in.install.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/for_install_only/mpd_wrap.m @ONLY)  
 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookread.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/bookread.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookwrite.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/bookwrite.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/bookplot.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/bookplot.m
                 COPYONLY)
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpr.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpr.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpr_wrap.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpr_wrap.m
                 COPYONLY)      
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpcat.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpcat.m
                 COPYONLY) 
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpcat_wrap.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpcat_wrap.m
                 COPYONLY)  
  CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpd_demix.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpd_demix.m
                 COPYONLY)                
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpd_demix_wrap.in.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpd_demix_wrap.m @ONLY)   
  
 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/matlab/scripts/contrib/lam/mpd_demix_wrap.in.install.m ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/for_install_only/mpd_demix_wrap.m @ONLY)          

INSTALL(FILES ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/for_install_only/InstallMat2MPTK.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/GenerDict.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/Mat2MPTKLoadSettings.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/Mat2MPTKUpdateOptions.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/Mat2MPTKdemo.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpd.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/for_install_only/mpd_wrap.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpf.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpf_wrap.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/bookread.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/bookwrite.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/bookplot.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpr.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpr_wrap.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpcat.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpcat_wrap.m
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/mpd_demix.m	
              ${MPTK_BINARY_DIR}/src/matlab/mat2mptk/for_install_only/mpd_demix_wrap.m
DESTINATION matlab/mat2mptk)
       
ENDIF(INSTALL_MAT2MPTK)  



# GOTO EXPERIMENTAL MEX FILES SUBDIRECTORY
ADD_SUBDIRECTORY (experimental)


