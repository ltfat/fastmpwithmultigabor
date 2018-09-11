# CMakeLists.txt managing the Matlab files in
# src/matlab
# src/matlab/scripts
# src/matlab/scripts/experimental
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
	# Set a few variables and the MEX file compiler
	#  MATLAB_INCLUDE_DIR: include path for mex.h, engine.h
	#  MATLAB_LIBRARIES:   required libraries: libmex, etc
	#  MATLAB_MEX_LIBRARY: path to libmex.lib
	#  MATLAB_MX_LIBRARY:  path to libmx.lib
	#  MATLAB_ENG_LIBRARY: path to libeng.lib
	#  MATLAB_MEX_COMPILER
	INCLUDE(${MPTK_SOURCE_DIR}/CMake/FindMatlab.cmake)
ENDIF(BUILD_MATLAB_MEX_FILES OR BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)

#############################
#            -2-
# Set source and output path
#############################
SET(MPTK_MATLAB_SRC_DIR ${MPTK_SOURCE_DIR}/src/matlab)
SET(MPTK_MATLAB_OUTPUT_DIR ${MPTK_BINARY_DIR}/mptk/matlab)
SET(MPTK_MATLAB_INSTALL_DIR mptk/matlab)

############################################
#                  -3-
# Main script files, always installed
############################################
# README file
CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/README ${MPTK_MATLAB_OUTPUT_DIR}/README COPYONLY)           
INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/README DESTINATION ${MPTK_MATLAB_INSTALL_DIR})

# Getting started script
FOREACH(file GettingStarted.m)
    SET(MPTK_CONFIG_FILENAME ${MPTK_BINARY_DIR}/mptk/path.xml)
    CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/${file}.in ${MPTK_MATLAB_OUTPUT_DIR}/${file} @ONLY)
    SET(MPTK_CONFIG_FILENAME ${CMAKE_INSTALL_PREFIX}/mptk/path.xml)
    CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/${file}.in ${MPTK_BINARY_DIR}/install/mptk/matlab/${file} @ONLY)     
    INSTALL(FILES ${MPTK_BINARY_DIR}/install/mptk/matlab/${file} DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(file)


# List of stable script files to be copied and installed
SET(MATSCRIPT bookover bookplot mpview bookedit dictcreate_gui)
SET(MATSCRIPT_EXEC ${MATSCRIPT} PARENT_SCOPE)
FOREACH(scriptfile ${MATSCRIPT})
   # Copy the script
   CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/${scriptfile}.m ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}.m COPYONLY)
   # Set rule to install it
   INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}.m DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(scriptfile)

# List of stable matrix script files to be copied and installed
SET(MATTOOLBAR MPtoolbaricons.mat)
FOREACH(matscriptfile ${MATTOOLBAR})
   # Copy the script
   CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/${matscriptfile} ${MPTK_MATLAB_OUTPUT_DIR}/${matscriptfile} COPYONLY)
   # Set rule to install it
   INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${matscriptfile} DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(matscriptfile)

#SET(MAT_LATEX_UTILS latexdoc_block_xmltags)
#SET(MATSCRIPT_LATEX ${MAT_LATEX_UTILS} PARENT_SCOPE)
#FOREACH(matscriptfile ${MAT_LATEX_UTILS})
#   # Copy the script
#   CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/${matscriptfile} ${MPTK_MATLAB_OUTPUT_DIR}/${matscriptfile} COPYONLY)
#   # Set rule to install it
#   INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${matscriptfile} DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
#ENDFOREACH(matscriptfile)

IF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)
# List of experimental script files to be copied and installed
FOREACH(scriptfile createrandomdictionary.m create_synthetic_data.m savetable.m tableread.m test_anywave.m test_mpd_demix.m)
   # Copy the script
   CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/experimental/${scriptfile} ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile} COPYONLY)
   # Set rule to install it
   INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile} DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(scriptfile)

ENDIF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)



############################################
#                  -4-
# Build the stable MEX files if requested
# ... and if possible!
############################################

IF(BUILD_MATLAB_MEX_FILES AND MATLAB_MEX_COMPILER)   

# mptk4matlab library:
SET(MPTK4MATLAB_SOURCES
classes/mptk4matlab.h
classes/mptk4matlab.cpp
classes/mexDict.cpp
classes/mexSignal.cpp
classes/mexAnywaveTable.cpp
experimental/classes/mxBook.h
experimental/classes/mxBook.cpp)

ADD_LIBRARY(mptk4matlab SHARED ${MPTK4MATLAB_SOURCES})

SET_TARGET_PROPERTIES(mptk4matlab PROPERTIES
		      COMPILE_FLAGS " -DC_BUILDING_mptk4matlab ${CMAKE_CXX_FLAGS} -I${MATLAB_INCLUDE_DIR}"
                      PREFIX ""
                      OUTPUT_NAME "libmptk4matlab"
                      )
                    
TARGET_LINK_LIBRARIES(mptk4matlab mptk ${MATLAB_MEX_LIBRARY} ${MATLAB_MX_LIBRARY} ${MATLAB_ENG_LIBRARY})
ADD_DEPENDENCIES(mptk4matlab mptk)
INSTALL(TARGETS
  mptk4matlab
 LIBRARY DESTINATION lib
 ARCHIVE DESTINATION lib
 RUNTIME DESTINATION lib
)

# List of mex files to be compiled and installed
SET(MAT_EXECUTABLES getmptkinfo dictread dictwrite sigread sigwrite bookread bookwrite mprecons mpdecomp cmpdecomp)
SET(MAT_EXEC_TARGETS ${MAT_EXECUTABLES} PARENT_SCOPE)
FOREACH(target ${MAT_EXECUTABLES})
   ADD_CUSTOM_TARGET(matlab_${target} 
   	             ALL echo
                     DEPENDS
		     ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
		     ${MPTK_MATLAB_SRC_DIR}/${target}.cpp
		     mptk4matlab)
 
   ## ======> What is the use of this line ?
   SET_TARGET_PROPERTIES(matlab_${target} PROPERTIES GENERATED true)

	# Command to generate the MEX-binary
	IF(UNIX)
		IF(APPLE)
			ADD_CUSTOM_COMMAND(
			OUTPUT ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
			POST_BUILD
			COMMAND   ${MATLAB_MEX_COMPILER} ${MATLAB_COMPILATION}
			ARGS      -v -largeArrayDims -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${LIBTINYXML_INCLUDE_DIR} -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${LIBDSP_INCLUDE_DIR} -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ -I${MPTK_MATLAB_SRC_DIR}/classes/ -I${MPTK_MATLAB_SRC_DIR}/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -outdir ${MPTK_MATLAB_OUTPUT_DIR} ${MPTK_MATLAB_SRC_DIR}/${target}.cpp -L${MPTK_LIB_DIR} -lmptk -lmptk4matlab
   # FOR SAFETY, SHOULD USE FLAG -headerpad_max_install_names
			COMMAND   install_name_tool -change ARGS ${MPTK_LIB_DIR}/libmptk4matlab.dylib libmptk4matlab.dylib ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
			COMMAND   install_name_tool -change ARGS ${MPTK_LIB_DIR}/libmptk.dylib libmptk.dylib ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
			COMMENT   "MEX")
		ELSE(APPLE)
			#ADD_CUSTOM_COMMAND(
			#OUTPUT ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
			#POST_BUILD
			#COMMAND   ${MATLAB_MEX_COMPILER} ${MATLAB_COMPILATION}
			#ARGS -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${LIBTINYXML_INCLUDE_DIR} -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${LIBDSP_INCLUDE_DIR} -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ -I${MPTK_MATLAB_SRC_DIR}/classes/ -I${MPTK_MATLAB_SRC_DIR}/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -outdir ${MPTK_MATLAB_OUTPUT_DIR} ${MPTK_MATLAB_SRC_DIR}/${target}.cpp  -L${MPTK_LIB_DIR} -lmptk -lmptk4matlab
   # FOR SAFETY, SHOULD USE FLAG -headerpad_max_install_names
			#COMMENT   "MEX")
			
			#g++ equivalent of Matlab mex command
			ADD_CUSTOM_COMMAND(
			OUTPUT ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
			POST_BUILD

			COMMAND ${CMAKE_CXX_COMPILER} 
			ARGS "-c" "-DHAVE_FFTW3" "-D_GNU_SOURCE" "-DMATLAB_MEX_FILE"  "-ansi" "-fexceptions" "-fPIC" "-fno-omit-frame-pointer" "-pthread" "-O3" "-DNDEBUG" 
			"${MPTK_MATLAB_SRC_DIR}/${target}.cpp"
			"-I${MATLAB_INCLUDE_DIR}" "-I${MPTK_BINARY_DIR}" "-I${LIBTINYXML_INCLUDE_DIR}" "-I/usr/include/" "-I${MPTK_SOURCE_DIR}" "-I${LIBDSP_INCLUDE_DIR}" "-I${MPTK_SOURCE_DIR}/src/libmptk/" "-I${FFTW3_INCLUDE_DIR}" "-I${MPTK_BINARY_DIR}/src/libmptk/" "-I${MPTK_MATLAB_SRC_DIR}/classes/" "-I${MPTK_MATLAB_SRC_DIR}/experimental/classes/" "-I${MPTK_SOURCE_DIR}/src/plugin/base/" "-I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/"
			"-o" "${MPTK_MATLAB_SRC_DIR}/${target}.o"

			#COMMAND ${CMAKE_CXX_COMPILER} 
			#ARGS -pthread -Wl,--no-undefined  -shared -O3 -Wl,--version-script,${MATLAB_ROOT}/extern/lib/glnxa64/mexFunction.map ${MPTK_MATLAB_SRC_DIR}/${target}.o /usr/lib64/libstdc++.so -lmptk -lmptk4matlab -L/usr/lib64 -Wl,-rpath-link,${MATLAB_ROOT}/bin/glnxa64 -L${MATLAB_ROOT}/bin/glnxa64 -lmx -lmex -lmat -lm -lstdc++ -o ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
			COMMAND ${CMAKE_CXX_COMPILER}
			ARGS  "-v" "-pthread" "-Wl,--no-undefined"  "-shared" "-O3" "-Wl,--version-script,${MATLAB_ROOT}/extern/lib/glnxa64/mexFunction.map" 
			"${MPTK_MATLAB_SRC_DIR}/${target}.o" 
			"-L${MPTK_LIB_DIR}" "-lmptk" "-lmptk4matlab" "-L${MATLAB_ROOT}/bin/glnxa64" "-lmx" "-lmex" "-lmat" "-lstdc++" "-lm" 
			"-Wl,-rpath=${MPTK_LIB_DIR}"
			"-o" "${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}"

			COMMAND rm 
			ARGS "-f" "${MPTK_MATLAB_SRC_DIR}/${target}.o"
	
   # FOR SAFETY, SHOULD USE FLAG -headerpad_max_install_names
			COMMENT   "MEX")

		ENDIF(APPLE)
	ELSE(UNIX)
          ADD_CUSTOM_COMMAND(
          OUTPUT ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
          POST_BUILD
          COMMAND   ${MATLAB_MEX_COMPILER} ${MATLAB_COMPILATION}
          ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${LIBTINYXML_INCLUDE_DIR} -I${MPTK_SOURCE_DIR} -I${LIBDSP_INCLUDE_DIR} -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${FFTW3_INCLUDE_DIR} -I${PTHREAD_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/ -I${MPTK_MATLAB_SRC_DIR}/classes/ -I${MPTK_MATLAB_SRC_DIR}/experimental/classes/ -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -L${MPTK_LIB_DIR} -L${MPTK_LIB_DIR} -outdir ${MPTK_MATLAB_OUTPUT_DIR} ${MPTK_MATLAB_SRC_DIR}/${target}.cpp -lmptk -lmptk4matlab
          COMMENT   "MEX")
   ENDIF(UNIX)
   
    # Set rules to install the MEX-binary and MEX-wrapper
    CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/${target}.m ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m COPYONLY)    
    INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION} ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m DESTINATION ${MPTK_MATLAB_INSTALL_DIR})

ENDFOREACH(target)

## $MATLAB_ROOT/sys/os/glnxa64 won't be added to LD_LIBRARY_PATH (in Matlab)
#SET(FICHIER_TMP_MATLAB ${MATLAB_ROOT}/bin/matlab)
#EXECUTE_PROCESS(
#	COMMAND chmod u+w ${FICHIER_TMP_MATLAB}
#	COMMAND "sed" "-i" "s/LD_LIBRARY_PATH=$LDPATH_MATLAB:$LD_LIBRARY_PATH.*/LD_LIBRARY_PATH=$LD_LIBRARY_PATH/" ${FICHIER_TMP_MATLAB}
#	COMMAND chmod u-w ${FICHIER_TMP_MATLAB}
#)


#Install mandatory dll for windows
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
IF(BUILD_DEBUG)
  #=== Copy the dll in the bin folder===
  ADD_CUSTOM_COMMAND (
    TARGET mptk4matlab
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy "${MPTK_BINARY_DIR}/lib/Debug/mptk.dll" "${MPTK_MATLAB_OUTPUT_DIR}/mptk.dll"
    COMMENT "Copying ${MPTK_BINARY_DIR}/lib/Debug/mptk.dll to ${MPTK_MATLAB_OUTPUT_DIR}/mptk.dll"
  )
  ADD_CUSTOM_COMMAND (
    TARGET mptk4matlab
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy "${MPTK_BINARY_DIR}/lib/Debug/libmptk4matlab.dll" "${MPTK_MATLAB_OUTPUT_DIR}/libmptk4matlab.dll"
    COMMENT "Copying ${MPTK_BINARY_DIR}/lib/Debug/libmptk4matlab.dll to ${MPTK_MATLAB_OUTPUT_DIR}/libmptk4matlab.dll"
  )

ELSE(BUILD_DEBUG) # Release
  #=== Copy the dll in the bin folder===
  ADD_CUSTOM_COMMAND (
        TARGET mptk4matlab
	POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy "${MPTK_BINARY_DIR}/lib/Release/mptk.dll" "${MPTK_MATLAB_OUTPUT_DIR}/mptk.dll"
  	COMMENT "Copying ${MPTK_BINARY_DIR}/lib/Release/mptk.dll to ${MPTK_MATLAB_OUTPUT_DIR}/mptk.dll"
  )
  ADD_CUSTOM_COMMAND (
   	TARGET mptk4matlab
   	POST_BUILD
   	COMMAND ${CMAKE_COMMAND}
   	ARGS -E copy "${MPTK_BINARY_DIR}/lib/Release/libmptk4matlab.dll" "${MPTK_MATLAB_OUTPUT_DIR}/libmptk4matlab.dll"
   	COMMENT "Copying ${MPTK_BINARY_DIR}/lib/Release/libmptk4matlab.dll to ${MPTK_MATLAB_OUTPUT_DIR}/libmptk4matlab.dll"
  )
ENDIF(BUILD_DEBUG)

# Removed ${MPTK_MATLAB_OUTPUT_DIR}/mptk.dll because installer can't find it ...
INSTALL(FILES  ${MPTK_MATLAB_OUTPUT_DIR}/libmptk4matlab.dll DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
INSTALL(FILES  ${MPTK_MATLAB_OUTPUT_DIR}/mptk.dll DESTINATION ${MPTK_MATLAB_INSTALL_DIR})

ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")

############################################
#                  -5-
# Install poor man's replacement to stable
# MEX files, only if the MEX ones are not.
############################################
ELSE(BUILD_MATLAB_MEX_FILES AND MATLAB_MEX_COMPILER)   

	# If no MEX compiler is found, or MEX files were not wanted just copy the scripts
	IF(BUILD_MATLAB_MEX_FILES)
		MESSAGE("Warning: Matlab MEX compiler is not found, please set the path to Mex compiler or Matlab Mex files will not be compiled")
	ENDIF(BUILD_MATLAB_MEX_FILES)
	MESSAGE("Warning: Matlab scripts implementing 'poor man' versions of MEX-files will be installed, unless you set BUILD_MATLAB_MEX_FILES to ON.")
 
	# List of script files to be copied and installed as cheap replacements to MEX-files
	FOREACH(scriptfile  dictread.m dictwrite.m bookread.m bookwrite.m)
		# Copy the script
		CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/scripts/obsolete/${scriptfile} ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile} COPYONLY)
		# Set rule to install it
		INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${scriptfile}
		DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
	ENDFOREACH(scriptfile)

ENDIF(BUILD_MATLAB_MEX_FILES AND MATLAB_MEX_COMPILER)   

############################################
#                  -6-
# Matlab experimental files, if requested
############################################

IF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)

# The MEX files and the files that depend on them ... if possible
IF(MATLAB_MEX_COMPILER)

# List of mex files to be compiled
FOREACH(target mextest)
   ADD_CUSTOM_TARGET(matlab_${target} 
                     ALL echo
                     DEPENDS 
		     ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
		     ${MPTK_MATLAB_SRC_DIR}/experimental/${target}.cpp 
#		     experimental/${target}.cpp 
		     mptk4matlab)

   SET_TARGET_PROPERTIES(matlab_${target} PROPERTIES GENERATED true )

   # Command to generate MEX-binary  
   ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION}
   POST_BUILD
   COMMAND   ${MATLAB_MEX_COMPILER} -v
   ARGS      -O -DHAVE_FFTW3 -I${MATLAB_INCLUDE_DIR} -I${MPTK_BINARY_DIR} -I${LIBTINYXML_INCLUDE_DIR} -I/usr/include/ -I${MPTK_SOURCE_DIR} -I${LIBDSP_INCLUDE_DIR} -I${MPTK_SOURCE_DIR}/src/libmptk/ -I${MPTK_MATLAB_SRC_DIR}/experimental/classes/ -I${MPTK_MATLAB_SRC_DIR}/classes/ -I${FFTW3_INCLUDE_DIR} -I${PTHREAD_INCLUDE_DIR} -I${MPTK_BINARY_DIR}/src/libmptk/  -I${MPTK_SOURCE_DIR}/src/plugin/base/ -I${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/ -outdir ${MPTK_MATLAB_OUTPUT_DIR} ${MPTK_MATLAB_SRC_DIR}/experimental/${target}.cpp  -L${MPTK_LIB_DIR} -L${MPTK_LIB_DIR} -lmptk -lmptk4matlab
   COMMENT   "MEX")

  # Copy the MEX-wrapper
  CONFIGURE_FILE(${MPTK_MATLAB_SRC_DIR}/experimental/${target}.m ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m
                 COPYONLY) 

  # Set rules to install the MEX-binary and MEX-wrapper
  INSTALL(FILES ${MPTK_MATLAB_OUTPUT_DIR}/${target}.${MEX_EXTENSION} ${MPTK_MATLAB_OUTPUT_DIR}/${target}.m
          DESTINATION ${MPTK_MATLAB_INSTALL_DIR})
ENDFOREACH(target)

# The list of scripts and data files which rely on the MEX_files
FOREACH(file debugbook_exp.m)  
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
ELSE(MATLAB_MEX_COMPILER) 
    MESSAGE("MEX compiler not found, please set the path to Mex compiler or Matlab Mex experimental files will not be compiled" )
ENDIF(MATLAB_MEX_COMPILER)
ENDIF(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL)

############################################
#                  -7-
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
