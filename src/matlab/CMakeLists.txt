# CMakeLists.txt managing the Matlab files in 
# src/matlab
# src/matlab/scripts
# src/matlab/scripts/contrib
# src/matlab/scripts/contrib/lam
# src/matlab/experimental
#

# Do we really need this ?
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0002 OLD)
endif(COMMAND cmake_policy)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

############################################
#                  -1-
# Are we asked to build Matlab MEX files ? 
# If yes, we need to find the compiler, etc.
############################################
IF(BUILD_MATLAB_MEX_FILES OR BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)

# Test if we are in in X86_64 and then set the -fPIC compil tag
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
 ADD_DEFINITIONS(-fPIC)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 

#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(MPTK4MATLAB_COMPILE_FLAGS " -fPIC")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(MPTK4MATLAB_COMPILE_FLAGS " -Wno-deprecated -fPIC")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(MPTK4MATLAB_COMPILE_FLAGS "")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(MPTK4MATLAB_COMPILE_FLAGS " -Wno-deprecated")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 



# Set a few variables and the MEX file compiler
#  MATLAB_INCLUDE_DIR: include path for mex.h, engine.h
#  MATLAB_LIBRARIES:   required libraries: libmex, etc
#  MATLAB_MEX_LIBRARY: path to libmex.lib
#  MATLAB_MX_LIBRARY:  path to libmx.lib
#  MATLAB_ENG_LIBRARY: path to libeng.lib
#  MEX_COMPILER

# SET MEX files extension (we could use the mexext program)
# and find MEX compiler
IF(UNIX)
IF(APPLE)
# For MacOS, need to determine Intel/PPC architecture
IF(CMAKE_OSX_ARCHITECTURES MATCHES i386)
   SET(MEX_EXTENSION mexmaci)
ELSE(CMAKE_OSX_ARCHITECTURES MATCHES i386)
   SET(MEX_EXTENSION mexmac)
ENDIF(CMAKE_OSX_ARCHITECTURES MATCHES i386)
   SET(MATLAB_ROOT 
   /Applications/MATLAB_R2008a/
   /Applications/MATLAB_R2007b/
   /Applications/MATLAB_R2007a/
   /Applications/MATLAB74/
    )
   FIND_PROGRAM(MEX_COMPILER mex PATH 
   /Applications/MATLAB_R2008a/bin/
   /Applications/MATLAB_R2007b/bin/
   /Applications/MATLAB_R2007a/bin/
   /Applications/MATLAB74/bin/
   NO_DEFAULT_PATH)
   FIND_PATH(MATLAB_INCLUDE_DIR
   "mex.h"
   /Applications/MATLAB_R2008a/extern/include/
   /Applications/MATLAB_R2007b/extern/include/
   /Applications/MATLAB_R2007a/extern/include/
   /Applications/MATLAB74/extern/include/
   NO_DEFAULT_PATH)
   FIND_LIBRARY(MATLAB_MEX_LIBRARY mex 
   /Applications/MATLAB_R2008a/bin/maci/
   /Applications/MATLAB_R2007b/bin/maci/
   /Applications/MATLAB_R2007a/bin/maci/
   /Applications/MATLAB74/bin/maci/
   NO_DEFAULT_PATH)
   FIND_LIBRARY(MATLAB_MX_LIBRARY mx 
   /Applications/MATLAB_R2008a/bin/maci/
   /Applications/MATLAB_R2007b/bin/maci/
   /Applications/MATLAB_R2007a/bin/maci/
   /Applications/MATLAB74/bin/maci/
   NO_DEFAULT_PATH)
   FIND_LIBRARY(MATLAB_ENG_LIBRARY eng 
   /Applications/MATLAB_R2008a/bin/maci/
   /Applications/MATLAB_R2007b/bin/maci/
   /Applications/MATLAB_R2007a/bin/maci/
   /Applications/MATLAB74/bin/maci/
   NO_DEFAULT_PATH)
   SET(MATLAB_LIBRARIES
   ${MATLAB_MEX_LIBRARY}
   ${MATLAB_MX_LIBRARY}
   ${MATLAB_ENG_LIBRARY}
   )   
ELSE(APPLE)
   SET(MEX_EXTENSION mexglx)
   INCLUDE(FindMatlab)
   FIND_PROGRAM(MEX_COMPILER mex)
ENDIF(APPLE)

ELSE(UNIX)
IF(WIN32) 
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
   INCLUDE(FindMatlab)
   SET(MEX_EXTENSION mexw32)
   FIND_PROGRAM(MEX_COMPILER mex)
   INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})
   MESSAGE("Please select Visual Studio as the mex compiler. From within Matlab, type the command line 'mex -setup' and choose Visual Studio in the list.")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
   MESSAGE("Matlab Mex files are only supported by MS Visual Studio")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF(WIN32)
ENDIF(UNIX)
ENDIF(BUILD_MATLAB_MEX_FILES OR BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)

MESSAGE("MEX_COMPILER:                                         = ${MEX_COMPILER}")
MESSAGE("MATLAB_INCLUDE_DIR: include path for mex.h, engine.h = ${MATLAB_INCLUDE_DIR}")
MESSAGE("MATLAB_LIBRARIES: required libraries : libmex, etc    = ${MATLAB_LIBRARIES}")
MESSAGE("MATLAB_MEX_LIBRARY: path to libmex.lib                = ${MATLAB_MEX_LIBRARY}") 
MESSAGE("MATLAB_MX_LIBRARY:  path to libmx.lib                 = ${MATLAB_MX_LIBRARY}")
MESSAGE("MATLAB_ENG_LIBRARY: path to libeng.lib                = ${MATLAB_ENG_LIBRARY}")





############
# Set source and output path
###########
SET(MPTK_MATLAB_SRC_DIR ${MPTK_SOURCE_DIR}/src/matlab)
SET(MPTK_MATLAB_OUTPUT_DIR ${MPTK_BINARY_DIR}/src/matlab)
SET(MPTK_MATLAB_INSTALL_DIR matlab)

############################################
#                  -2-
# Main script files, always installed
############################################
# README file
CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/README ${MPTK_MATLAB_OUTPUT_DIR}/README  
               COPYONLY)           
INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/README 
        DESTINATION ${MPTK_MATLAB_INSTALL_DIR})

# Getting started script
FOREACH(file GettingStarted.m)
    SET(MPTK_CONFIG_FILENAME ${MPTK_BINARY_DIR}/bin/path.xml)
    CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/${file}.in ${MPTK_MATLAB_OUTPUT_DIR}/${file}
                  @ONLY)
    SET(MPTK_CONFIG_FILENAME ${CMAKE_INSTALL_PREFIX}/bin/path.xml)
    CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/${file}.in ${MPTK_MATLAB_OUTPUT_DIR}/for_install_only/${file}
                   @ONLY)     
    INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/for_install_only/${file}
            DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(file)


# SCRIPT FILES
# List of script files to be copied and installed
FOREACH(scriptfile bookover.m bookplot.m mpview.m create_synthetic_data.m savetable.m)
   # Copy the script
   CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/${scriptfile} ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}
                  COPYONLY)
   # Set rule to install it
   INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}
           DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(scriptfile)

# Read/Write deprecated script files 
FOREACH(scriptfile  dictread dictwrite bookread bookwrite)
   # Copy the script
   CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/${scriptfile}.m ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}_deprecated.m
                 COPYONLY)
   # Set rule to install it
   INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}_deprecated.m
           DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(scriptfile)




############################################
#                  -3-
# Build the stable MEX files if requested
# ... and if possible!
############################################

IF(BUILD_MATLAB_MEX_FILES AND MEX_COMPILER)   

# mptk4matlab library:
SET(MPTK4MATLAB_SOURCES
classes/mptk4matlab.h
classes/mptk4matlab.cpp
classes/mexDict.cpp
classes/mexSignal.cpp
experimental/classes/mxBook.h
experimental/classes/mxBook.cpp)

ADD_LIBRARY(mptk4matlab SHARED ${MPTK4MATLAB_SOURCES})

SET_TARGET_PROPERTIES(mptk4matlab PROPERTIES
		      COMPILE_FLAGS " -DC_BUILDING_mptk4matlab ${SHARED_FLAGS} ${MPTK4MATLAB_COMPILE_FLAGS} -I${MATLAB_INCLUDE_DIR}  -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/"
    PREFIX ""
    OUTPUT_NAME "libmptk4matlab")
    TARGET_LINK_LIBRARIES(mptk4matlab  mptk ${MATLAB_LIBRARIES} ${MATLAB_MEX_LIBRARY}) 
    ADD_DEPENDENCIES(mptk4matlab mptk) 

# List of mex files to be compiled and installed
FOREACH(target dictread dictwrite sigread sigwrite bookread bookwrite)
   ADD_CUSTOM_TARGET(matlab_${target} 
   	             ALL echo
                     DEPENDS  
		     ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION} 
		     ${MPTK_MATLAB_SRC_DIR}/${target}.cpp 
		     ${target}.cpp 
		     mptk4matlab)

#   ADD_EXECUTABLE(${target}.${MEX_EXTENSION} ${MPTK_MATLAB_SRC_DIR}/${target}.cpp)

   ## ======> What is the use of this line ?
   SET_TARGET_PROPERTIES(matlab_${target} PROPERTIES GENERATED true )

   # Command to generate the MEX-binary
   ADD_CUSTOM_COMMAND(
    OUTPUT ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
    POST_BUILD
    COMMAND   ${MEX_COMPILER} -v
    ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ -I${MPTK_MATLAB_SRC_DIR}/classes/ -I${MPTK_MATLAB_SRC_DIR}/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ ${MPTK_MATLAB_SRC_DIR}/${target}.cpp -L${MPTK_LIB_DIR} -lmptk -lmptk4matlab
    COMMENT   "MEX")

    # Copy the MEX-wrapper
    CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/${target}.m ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m
                   COPYONLY) 
    # Set rules to install the MEX-binary and MEX-wrapper
    INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION} ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m
            DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(target)

############################################
#                  -4-
# Install poor man's replacement to stable
# MEX files, only if the MEX ones are not.
############################################
ELSE(BUILD_MATLAB_MEX_FILES AND MEX_COMPILER)   
  # If no MEX compiler is found, or MEX files were not wanted just copy the scripts
  IF (BUILD_MATLAB_MEX_FILES)
  MESSAGE("MEX compiler not found, please set the path to Mex compiler or Matlab Mex files will not be compiled" )
  MESSAGE("Please note that on Microsoft Windows platform, only Visual Studio supports Matlab Mex compiler" )
  ENDIF (BUILD_MATLAB_MEX_FILES)

  MESSAGE("Warning: Matlab scripts implementing 'poor man' versions of MEX-files will be installed, unless you set BUILD_MATLAB_MEX_FILES to ON.")
  MESSAGE("Warning: They may not be maintained and compatible with evolving MPTK data formats.")
 
# List of script files to be copied and installed as cheap replacements to MEX-files
FOREACH(scriptfile  dictread.m dictwrite.m bookread.m bookwrite.m)
   # Copy the script
   CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/${scriptfile} ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}
                 COPYONLY)
   # Set rule to install it
   INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}
           DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(scriptfile)

ENDIF(BUILD_MATLAB_MEX_FILES AND MEX_COMPILER)   

############################################
#                  -4-
# Matlab experimental files, if requested
############################################

IF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)

# The MEX files and the files that depend on them ... if possible
IF(MEX_COMPILER)

# List of mex files to be compiled
FOREACH(target mptk4matlab_getinfo_exp mextest loadenv_exp releaseenv_exp bookrecons_exp atomrecons_exp)
   ADD_CUSTOM_TARGET(matlab_${target} 
                     ALL echo
                     DEPENDS 
		     ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
		     ${MPTK_MATLAB_SRC_DIR}/experimental/${target}.cpp 
		     experimental/${target}.cpp 
		     mptk4matlab)

   SET_TARGET_PROPERTIES(matlab_${target} PROPERTIES GENERATED true )

   # Command to generate MEX-binary  
   ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
   POST_BUILD
   COMMAND   ${MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${MPTK_SOURCE_DIR}/src/tinyxml/ -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${MPTK_SOURCE_DIR}/src/libdsp_windows/ -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_MATLAB_SRC_DIR}/experimental/classes/ -I${MPTK_MATLAB_SRC_DIR}/classes/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/  -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ ${MPTK_MATLAB_SRC_DIR}/experimental/${target}.cpp  -L${MPTK_LIB_DIR} -lmptk -lmptk4matlab
   COMMENT   "MEX")

  # Copy the MEX-wrapper
  CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/experimental/${target}.m ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m
                 COPYONLY) 
  # Set rules to install the MEX-binary and MEX-wrapper
  INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION} ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m
          DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(target)

# The list of scripts and data files which rely on the MEX_files
FOREACH(file dictcreate_gui.m  bookedit_exp.m  MPtoolbaricons.mat debugbook_exp.m)  
  CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/experimental/${file} ${MPTK_MATLAB_OUTPUT_DIR}/${file}
                 COPYONLY) 
  INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${file}
          DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(file)
# The README, renames to avoid erasing other README from Mat2MPTK or standard scripts
CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/experimental/README ${MPTK_MATLAB_OUTPUT_DIR}/README_EXP
                 COPYONLY) 
  INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/README_EXP
          DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ELSE(MEX_COMPILER) 
    MESSAGE("MEX compiler not found, please set the path to Mex compiler or Matlab Mex experimental files will not be compiled" )
ENDIF(MEX_COMPILER)
ENDIF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)

############################################
#                  -5-
# Mat2MPTK, if requested
############################################
# If we are asked to install Mat2MPTK 
# TO BE COMMENTED!

IF(INSTALL_MAT2MPTK)            

SET(MAT2MPTK_SRCPATH ${MPTK_MATLAB_SRC_DIR}/scripts/contrib/lam)

# Files to configure
FOREACH(file InstallMat2MPTK mpd_wrap mpd_demix_wrap)
     CONFIGURE_FILE(${MAT2MPTK_SRCPATH}/${file}.in.m ${MPTK_MATLAB_OUTPUT_DIR}/${file}.m 
                    @ONLY)
     CONFIGURE_FILE(${MAT2MPTK_SRCPATH}/${file}.in.install.m ${MPTK_MATLAB_OUTPUT_DIR}/for_install_only/${file}.m 
                    @ONLY)     
     INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/for_install_only/${file}.m  
             DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(file)

# Mat2MPTK scripts
FOREACH(file GenerDict.m Mat2MPTKLoadSettings.m Mat2MPTKUpdateOptions.m Mat2MPTKdemo.m mpd.m ReadDecay.m mpf.m mpf_wrap.m mpr.m mpr_wrap.m mpcat.m mpcat_wrap.m mpd_demix.m)
     CONFIGURE_FILE(${MAT2MPTK_SRCPATH}/${file} ${MPTK_MATLAB_OUTPUT_DIR}/${file} 
                    COPYONLY)   
     INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${file}   
             DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(file)

# README
CONFIGURE_FILE(${MAT2MPTK_SRCPATH}/README ${MPTK_MATLAB_OUTPUT_DIR}/REAME_MAT2MPTK
               COPYONLY)   
INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/REAME_MAT2MPTK
        DESTINATION ${MPTK_MATLAB_INSTALL_DIR})

ENDIF(INSTALL_MAT2MPTK)  


#
# Add an executable to debug Matlab from an IDE
# 
#IF(MEX_COMPILER)
#SET(MATLABROOT /Applications/MATLAB_R2008a)
# The command line should be
#SET(MATLAB_EXE_PATH ${MATLABROOT}/bin/maci/MATLAB)
# Launched in the directory
#SET(MATLAB_EXE_PWD ${MPTK_MATLAB_OUTPUT_DIR})
# The environment variables on maci
#MATLAB ${MATLABROOT}
#AUTOMOUNT_MAP: private ->
#ARCH: maci
#XKEYSYMDB: ${MATLABROOT}/X11/app-defaults/XKeysymDB
#XAPPLRESDIR: ${MATLABROOT}/X11/app-defaults
#MATLABPATH: ${MATLABROOT}/toolbox/local
#DYLD_LIBRARY_PATH: ${MATLABROOT}/sys/os/maci:${MATLABROOT}/bin/maci:${MATLABROOT}/extern/lib/maci
#TOOLBOX: ${MATLABROOT}/toolbox
#MPTK_CONFIG_FILENAME : ${MPTK_BINARY_DIR}/bin/path.xml

# And the arguments, although the first one seems bugged
#-display :0.0
#-nodesktop

# ADD_EXECUTABLE(debug_matlab IMPORTED) ?????

# ENDIF(MEX_COMPILER)
