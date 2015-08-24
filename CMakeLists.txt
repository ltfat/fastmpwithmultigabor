## Main CMakeLists.txt of the MPTK project
## Other important files for CMake are
#  - CMakeLists.txt files in the sub-directories
#  - CMake/* files, which define some tools to search for libraries, etc.

PROJECT(MPTK)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0002 OLD)
endif(COMMAND cmake_policy)


##############################################################################
# Setting compilation option
##############################################################################
# Allows building executable:
OPTION(BUILD_EXECUTABLE "Build executable." ON)
# Allows building of shared libraries:
OPTION(BUILD_SHARED_LIBS "Build shared libraries." ON)
# Enable testing:
OPTION(BUILD_TESTING "Build Tests" ON) 
#Allows generating the doc:
OPTION(BUILD_DOCUMENTATION "Generating the doxygen documentation." OFF)
# Allows multithreading:
OPTION(BUILD_MULTITHREAD "Enable multithread." OFF)
# Allows verbose:
OPTION(BUILD_VERBOSE "Enable verbose option when compile (-v)." ON)
# Allows MPTK Debug:
OPTION(BUILD_DEBUG "Enable MPTK Debug mode." OFF)
# Allows Matlab MEX files :
OPTION(BUILD_MATLAB_MEX_FILES "Enable building Matlab MEX files." ON)
# Allows build python extension :
OPTION(BUILD_PYTHON_FILES "Enable building python files." ON)
# Allows build experimental feature :
OPTION(BUILD_EXPERIMENTAL "Enable building experimental files." OFF)
# Allows build experimental feature :
OPTION(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL "Enable building Matlab experimental MEX files." OFF)
# Allows build RPM experimental feature :
IF(UNIX)
OPTION(INSTALL_MAT2MPTK "Install Mat2MPTK toolkit" OFF)
# Compilation of modules:
OPTION(BUILD_GUI "Build experimental Qt Graphical user interface" OFF) 
#Allows generating the doc for GUI:
OPTION(BUILD_DOCUMENTATION_GUI "Generating the doxygen documentation for GUI." OFF)
ENDIF(UNIX)
#You can reference the special $ENV{} variable to get environment values
#like any other CMake variable.

##############################################################################
# Setting global variables
##############################################################################
# Choose a default CMAKE_INSTALL_PREFIX on Windows.  CMake 2.2 and
# above already choose a good default, so force a change only if the
# current value looks like the CMake 2.0 default.



IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/output" CACHE STRING "default install path" FORCE )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)



IF(UNIX)
	SET(BATCHEXT ".sh")
	SET(MPTK_USER_LOCAL_PATH $ENV{HOME}/local/bin)
	SET(MPTK_USER_INCLUDE_PATH $ENV{HOME}/local/include)
	SET(MPTK_USER_LIB_PATH $ENV{HOME}/local/lib)
ELSE(UNIX)
	IF(WIN32)
		SET(BATCHEXT ".bat")
		IF(CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
			IF("$ENV{ProgramFiles}" MATCHES "^$")
				IF("$ENV{SystemDrive}" MATCHES "^$")
					SET(MPTK_GENERIC_PROGRAM_FILES "C:/Program Files")
				ELSE("$ENV{SystemDrive}" MATCHES "^$")
					SET(MPTK_GENERIC_PROGRAM_FILES "$ENV{SystemDrive}/Program Files")
				ENDIF("$ENV{SystemDrive}" MATCHES "^$")
			ELSE("$ENV{ProgramFiles}" MATCHES "^$")
				SET(MPTK_GENERIC_PROGRAM_FILES "$ENV{ProgramFiles}")
			ENDIF("$ENV{ProgramFiles}" MATCHES "^$")
			SET(CMAKE_INSTALL_PREFIX "${MPTK_GENERIC_PROGRAM_FILES}/${PROJECT_NAME}" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
			SET(MPTK_GENERIC_SYSTEM_DRIVE)
		ENDIF(CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
		MARK_AS_ADVANCED(CLEAR CMAKE_INSTALL_PREFIX)
		# Recherche des librairies : Suffixes ".dll" et prefixes : "lib"
		SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
		SET(CMAKE_FIND_LIBRARY_PREFIXES "lib;")
	ENDIF(WIN32)
ENDIF(UNIX)

##############################################################################
# Setting coverage options :
##############################################################################
IF(BUILD_COVERAGE AND APPLE)
	SET(COVERAGE_FLAGS -fprofile-arcs -ftest-coverage)
	SET(COVERAGE_LIB -lgcov)
ENDIF(BUILD_COVERAGE AND APPLE)

##############################################################################
# Setting the compilation flags in dynamic format:
##############################################################################
IF(WIN32)
	IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64" )
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
	ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64" )
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64" )
ELSE(WIN32)
	IF(APPLE)
		IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -single_module -headerpad_max_install_names ${COVERAGE_LIB}") 
			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
		ELSE(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -single_module -headerpad_max_install_names ${COVERAGE_LIB}") 
		ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
	ELSE(APPLE)
		IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wl,--export-dynamic")
			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
		ELSE(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export-dynamic")
		ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
	ENDIF(APPLE)
ENDIF(WIN32)

##############################################################################
# Dash server testing :
##############################################################################
IF(BUILD_TESTING AND DASH_TESTING)
	INCLUDE(${MPTK_SOURCE_DIR}/CMake/Pipol.cmake)
	IF(PIPOL_IMAGE)
		SET(BUILDNAME "${PIPOL_IMAGE_NAME}")
		SET(SITE "${PIPOL_SITE}")
	ELSE(PIPOL_IMAGE)
		SET(BUILDNAME "${CMAKE_SYSTEM_NAME}")
	ENDIF(PIPOL_IMAGE)
ENDIF(BUILD_TESTING AND DASH_TESTING)

##############################################################################
# Setting compilation options :
##############################################################################
#Select the ADD_DEFINITIONS according to selected options
IF(WIN32)
  # With Visual Studio 2005, Microsoft deprecates the standard C library, for
	# example fopen() and sprintf(), to non-portable functions fopen_s() and
	# sprintf_s(). These functions are considered by Microsoft more secure. This is
	# a worthwhile exercise ! The use of these deprecated functions causes a lot of
	# warnings. To suppress it, we add the _CRT_SECURE_NO_DEPRECATE preprocessor
	# definition -fprofile-arcs -ftest-coverage
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
  IF(BUILD_DEBUG) 
    # Add debug mode for mex files
    SET(MATLAB_COMPILATION "-g")		
  ENDIF(BUILD_DEBUG)
ELSE(WIN32)
	# Case of other compiler/environment : tags depend on build mode (debug/release
	IF(BUILD_DEBUG) 
		# Case of debug mode
		IF (BUILD_VERBOSE)
			ADD_DEFINITIONS(-g -O3 -Wall -pipe -v ${COVERAGE_FLAGS})
		ELSE(BUILD_VERBOSE)
			ADD_DEFINITIONS(-g -O3 -quiet -pipe ${COVERAGE_FLAGS})
		ENDIF(BUILD_VERBOSE)
		# Add debug mode for mex files
		SET(MATLAB_COMPILATION "-g")		
	ELSE(BUILD_DEBUG)
		# Case of release mode
		IF (BUILD_VERBOSE)
			ADD_DEFINITIONS( -O3 -Wall -pipe -v -DNDEBUG ${COVERAGE_FLAGS})
		ELSE(BUILD_VERBOSE)
			ADD_DEFINITIONS( -O3 -quiet -pipe -DNDEBUG ${COVERAGE_FLAGS})
		ENDIF(BUILD_VERBOSE)
		# Add debug mode for mex files
		SET(MATLAB_COMPILATION "-v")		
	ENDIF(BUILD_DEBUG)
ENDIF(WIN32)

##############################################################################
#Setting package versions informations according to text files in /version
##############################################################################
INCLUDE(${MPTK_SOURCE_DIR}/version/buildversion.cmake)
FILE(READ ${MPTK_SOURCE_DIR}/version/AUTHORS.txt AUTHORS)
SET(BUILDVERSION_GUILLEMETS \"${BUILDVERSION_MAJOR}.${BUILDVERSION_MINOR}.${BUILDVERSION_PATCH}\")
SET(BUILDVERSION "${BUILDVERSION_MAJOR}.${BUILDVERSION_MINOR}.${BUILDVERSION_PATCH}")

##############################################################################
#Define the target for install
##############################################################################
SET(MPTK_FFTW_WISDOM_FILE ${MPTK_BINARY_DIR}/mptk/default_fftw_wisdom_file)
SET(MPTK_FFTW_WISDOM_FILE_INSTALL ${CMAKE_INSTALL_PREFIX}/mptk/default_fftw_wisdom_file)
SET(MPTK_REFERENCE_PATH ${MPTK_BINARY_DIR}/mptk/reference)
SET(MPTK_REFERENCE_PATH_INSTALL ${CMAKE_INSTALL_PREFIX}/mptk/reference)

##############################################################################
#Define and create directory for python install
##############################################################################
SET(MPTK_PYTHON_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/python" CACHE INTERNAL "Python installing path" )
FILE(MAKE_DIRECTORY ${MPTK_PYTHON_INSTALL_PATH})	


##############################################################################
#Define temporary directory
##############################################################################
IF(UNIX)
	SET(MPTK_TMP_PATH "${MPTK_SOURCE_DIR}/tmp")
ELSE(UNIX)
	IF(WIN32)
		SET(MPTK_TMP_PATH "${MPTK_SOURCE_DIR}/Temp")
	ENDIF(WIN32)
ENDIF(UNIX)
FILE(MAKE_DIRECTORY ${MPTK_TMP_PATH})	

##############################################################################
# Setting DEFAULT REFERENCE FILES
##############################################################################
#--------------------------
# Copy default other files
#--------------------------
SET(MPTK_DEFAULT_MIXER_FILENAME  mixer/mix_58_mixer.txt)

FOREACH(defaultOther MIXER )
	SET(MPTK_DEFAULT_${defaultOther}_FILE          ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_${defaultOther}_FILENAME})
	SET(MPTK_DEFAULT_${defaultOther}_FILE_INSTALL  ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_DEFAULT_${defaultOther}_FILENAME})
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_${defaultOther}_FILENAME} ${MPTK_DEFAULT_${defaultOther}_FILE} COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_${defaultOther}_FILENAME}  ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_DEFAULT_${defaultOther}_FILENAME} COPYONLY)
ENDFOREACH(defaultOther)

#--------------------------
# Example reference files
#--------------------------
SET(MPTK_EXAMPLE_BOOK_FILENAME book/exampleBook.bin)
SET(MPTK_EXAMPLE_SIGNAL_FILENAME signal/glockenspiel.wav)
SET(MPTK_EXAMPLE_STEREOSIGNAL_FILENAME signal/mix_58_stereo.wav)
SET(MPTK_EXAMPLE_BIRDSIGNAL_FILENAME signal/chiffchaff.wav)
# Useful for Matlab script test
SET(MPTK_EXAMPLE_BOOK_WRITE_FILENAME book/testBookWrite.bin)
SET(MPTK_EXAMPLE_SIGNAL_WRITE_FILENAME signal/testglockenspiel.wav)
SET(MPTK_EXAMPLE_SIGNAL_RECONST_FILENAME signal/testglockenspielReconst.wav)

FOREACH(example BOOK SIGNAL STEREOSIGNAL BIRDSIGNAL)
	SET(MPTK_EXAMPLE_${example}_FILE          ${MPTK_REFERENCE_PATH}/${MPTK_EXAMPLE_${example}_FILENAME})
	SET(MPTK_EXAMPLE_${example}_FILE_INSTALL  ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_EXAMPLE_${example}_FILENAME})
	# Copy in the build directory
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_EXAMPLE_${example}_FILENAME} ${MPTK_EXAMPLE_${example}_FILE} COPYONLY)
	# Copy in the pre-install directory
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_EXAMPLE_${example}_FILENAME}  ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_EXAMPLE_${example}_FILENAME} COPYONLY)
ENDFOREACH(example)

# ----------------------------
# Configure AnywaveTable files
# ----------------------------
SET(MPTK_DEFAULT_ANYWAVETABLE_FILENAME wavetable/defaultWaveTable.xml)
SET(MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME wavetable/defaultWaveTableData.bin)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME} ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME} COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME} ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME} COPYONLY)
SET(MPTK_DEFAULT_ANYWAVETABLE_DATA_FILE ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME})
SET(MPTK_DEFAULT_ANYWAVETABLE_DATA_FILE_INSTALL ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME})
SET(ADDINSTALL "")
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_ANYWAVETABLE_FILENAME}.in ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_ANYWAVETABLE_FILENAME})
SET(ADDINSTALL "_INSTALL")
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_ANYWAVETABLE_FILENAME}.in ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_DEFAULT_ANYWAVETABLE_FILENAME})
SET(ADDINSTALL "")
SET(MPTK_DEFAULT_ANYWAVETABLE_FILE ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_ANYWAVETABLE_FILENAME})
SET(MPTK_DEFAULT_ANYWAVETABLE_FILE_INSTALL ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_DEFAULT_ANYWAVETABLE_FILENAME})

#----------------------------------------------------
# Retrieving automatically the dictionaries and copy
#----------------------------------------------------
FILE(GLOB DICT_PATHS "${MPTK_SOURCE_DIR}/reference/dictionary/dic_*.xml*")
FOREACH(dictcount ${DICT_PATHS})
	GET_FILENAME_COMPONENT(DICT_FILE_NAME ${dictcount} NAME_WE)
	GET_FILENAME_COMPONENT(DICT_FILE_EXT ${dictcount} EXT)
	IF(${DICT_FILE_EXT} STREQUAL ".xml.in")
		SET(ADDINSTALL "")
		SET(MPTK_DEFAULT_ANYWAVEDICT_FILE ${MPTK_REFERENCE_PATH}/dictionary/${DICT_FILE_NAME}.xml)
		CONFIGURE_FILE(${dictcount} ${MPTK_REFERENCE_PATH}/dictionary/${DICT_FILE_NAME}.xml)
		SET(ADDINSTALL "_INSTALL")
		SET(MPTK_DEFAULT_ANYWAVEDICT_FILE_INSTALL ${MPTK_REFERENCE_PATH_INSTALL}/dictionary/${DICT_FILE_NAME}.xml)
		CONFIGURE_FILE(${dictcount} ${MPTK_BINARY_DIR}/install/mptk/reference/dictionary/${DICT_FILE_NAME}.xml)
		SET(ADDINSTALL "")
		SET(dictcountCompil ${dictcountCompil} ${MPTK_REFERENCE_PATH}/dictionary/${DICT_FILE_NAME}.xml)
		SET(dictcountInstall ${dictcountInstall} ${CMAKE_INSTALL_PREFIX}/mptk/reference/dictionary/${DICT_FILE_NAME}.xml)
	ELSE()
		CONFIGURE_FILE(${dictcount} ${MPTK_REFERENCE_PATH}/dictionary/${DICT_FILE_NAME}${DICT_FILE_EXT} COPYONLY)
		CONFIGURE_FILE(${dictcount} ${MPTK_BINARY_DIR}/install/mptk/reference/dictionary/${DICT_FILE_NAME}${DICT_FILE_EXT} COPYONLY)
		SET(dictcountCompil ${dictcountCompil} ${MPTK_REFERENCE_PATH}/dictionary/${DICT_FILE_NAME}${DICT_FILE_EXT})
		SET(dictcountInstall ${dictcountInstall} ${CMAKE_INSTALL_PREFIX}/mptk/reference/dictionary/${DICT_FILE_NAME}${DICT_FILE_EXT})
	ENDIF()
	IF(${dictcount} MATCHES "gabor")
		SET(MPTK_DEFAULT_GABORDICT_FILE ${MPTK_REFERENCE_PATH}/dictionary/${DICT_FILE_NAME}${DICT_FILE_EXT})
		SET(MPTK_DEFAULT_GABORDICT_FILE_INSTALL ${MPTK_REFERENCE_PATH_INSTALL}/dictionary/${DICT_FILE_NAME}${DICT_FILE_EXT})
		SET(MPTK_DEFAULT_GABORDICT_FILENAME ${DICT_FILE_NAME}${DICT_FILE_EXT})
	ENDIF(${dictcount} MATCHES "gabor")
ENDFOREACH(dictcount)

# --------------------------
# Configure DictList file
# --------------------------
SET(MPTK_DEFAULT_DICT_LIST_FILENAME  dictionary/dict-list.txt)
SET(ADDINSTALL "")
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_DICT_LIST_FILENAME}.in ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_DICT_LIST_FILENAME})
SET(ADDINSTALL "_INSTALL")
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_DICT_LIST_FILENAME}.in ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_DEFAULT_DICT_LIST_FILENAME})
SET(ADDINSTALL "")
SET(MPTK_DEFAULT_DICT_LIST_FILE ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_DICT_LIST_FILENAME})
SET(MPTK_DEFAULT_DICT_LIST_FILE_INSTALL ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_DEFAULT_DICT_LIST_FILENAME})

##############################################################################
# Depending on compiler+IDE (Make/Xcode/Visual), need to specify postfix for build type
##############################################################################
# Case of Visual Studio 
IF(WIN32)
    IF (BUILD_DEBUG)
		# Case of debug mode
		SET(MPTK_DEBUG_RELEASE "debug")
		SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/Debug)
    ELSE (BUILD_DEBUG)
    	# Case of release mode
		SET(MPTK_DEBUG_RELEASE "release")
		SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/Release)
    ENDIF (BUILD_DEBUG)
	SET(MPTK_LIB_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/mptk/plugins)
ELSE(WIN32)
	# Case of Xcode
	IF( CMAKE_GENERATOR STREQUAL "Xcode")
    	# We would like to select a single build mode, would we ?
	    # or to select it dynamically there is the CMAKE_CFG_INTDIR stuff
	    IF (BUILD_DEBUG)
    		SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/Debug)
	    ELSE (BUILD_DEBUG)
    		SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/Release)
	    ENDIF (BUILD_DEBUG)
	ELSE( CMAKE_GENERATOR STREQUAL "Xcode")
	    # Generic case
	    SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib)
	ENDIF(CMAKE_GENERATOR STREQUAL "Xcode") 
    SET(MPTK_LIB_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/mptk/plugins)
ENDIF(WIN32)

#------------------
# setting bin path
# -----------------
IF(EXECUTABLE_OUTPUT_PATH)
ELSE(EXECUTABLE_OUTPUT_PATH)
	SET(EXECUTABLE_OUTPUT_PATH "${MPTK_BINARY_DIR}/bin/" CACHE PATH "Single output directory for building all executables.")
ENDIF(EXECUTABLE_OUTPUT_PATH)

#-----------------
#setting lib path
#-----------------
IF(LIBRARY_OUTPUT_PATH)
ELSE(LIBRARY_OUTPUT_PATH)
	SET(LIBRARY_OUTPUT_PATH "${MPTK_BINARY_DIR}/lib/" CACHE PATH "Single output directory for building all libraries.")
ENDIF(LIBRARY_OUTPUT_PATH)


##############################################################################
# FINDING EXTERNAL LIBRARIES
##############################################################################
# Setting path to libraries and modules for apps
SET(LIBDSP_INCLUDE_DIR  ${MPTK_SOURCE_DIR}/src/utils/libdsp_windows)
SET(LIBTINYXML_INCLUDE_DIR ${MPTK_SOURCE_DIR}/src/utils/libtinyxml)   	# TO INCLUDE TinyXML code

SET(UTILS_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/utils )
SET(GUI_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/gui )

# Include file to find libraries
INCLUDE( ${MPTK_SOURCE_DIR}/CMake/FindExternalLibs.cmake )
#-----------------------------------------------------------------------------

#Set option for building and linking:
#Set directories for linker
IF (EXISTS "/usr")
   SET(POSSIBLE_LIBRARY_PATH_1 "/usr" CACHE INTERNAL "")
ENDIF(EXISTS "/usr")

IF (EXISTS "/usr/local")
   SET(POSSIBLE_LIBRARY_PATH_2 "/usr/local" CACHE INTERNAL "")
ENDIF (EXISTS "/usr/local")

IF (EXISTS "/sw")
   SET(POSSIBLE_LIBRARY_PATH_3 "/sw" CACHE INTERNAL "")
ENDIF (EXISTS "/sw")

IF (EXISTS "/opt/local")
   SET(POSSIBLE_LIBRARY_PATH_4 "/opt/local" CACHE INTERNAL "")
ENDIF (EXISTS "/opt/local")


LINK_DIRECTORIES( "${POSSIBLE_LIBRARY_PATH_1}/lib"
                  "${POSSIBLE_LIBRARY_PATH_2}/lib"
                  "${POSSIBLE_LIBRARY_PATH_3}/lib"
                  "${POSSIBLE_LIBRARY_PATH_4}/lib")
#set the include directories paths for building
INCLUDE_DIRECTORIES(/usr/include 
		${MPTK_SOURCE_DIR}               	          # TO INCLUDE mp_system.h
		${MPTK_BINARY_DIR}               	          # TO INCLUDE config.h
		${PTHREAD_INCLUDE_DIR}				              # AUTOMATICALLY CREATED IN FindPThread
		${MPTK_SOURCE_DIR}/src/libmptk
		${MPTK_BINARY_DIR}/src/libmptk/ 	          # TO INCLUDE regression_constant.h and mptk.h, automatically generated 
		${LIBDSP_INCLUDE_DIR}
		${LIBTINYXML_INCLUDE_DIR}                   # TO INCLUDE TinyXML code
		${FFTW3_INCLUDE_DIR} 
		${SNDFILE_INCLUDE_DIR} 
		${MPTK_SOURCE_DIR}/src/plugin/base/
		${MPTK_SOURCE_DIR}/src/plugin/contrib/lam/
   	${MPTK_SOURCE_DIR}/src/utils/libbase64   	    # TO INCLUDE base64 code
   	${MPTK_SOURCE_DIR}/src/utils/libmd5sum   	    # TO INCLUDE md5sum code
    )
##############################################################################
##Platform configuration tests.
##############################################################################
INCLUDE(${MPTK_SOURCE_DIR}/CMake/CMakeBackwardCompatibilityC.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityCXX.cmake)

#Configure file according to multithread option:
IF(BUILD_MULTITHREAD)
SET(MULTITHREAD 1 CACHE INTERNAL "Define MULTITHREAD to 1" )
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/dict.in.cpp ${MPTK_BINARY_DIR}/src/libmptk/dict.cpp) 
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/atom.in.cpp ${MPTK_BINARY_DIR}/src/libmptk/atom.cpp) 
ELSE(BUILD_MULTITHREAD)
SET(MULTITHREAD 0 CACHE INTERNAL "Define MULTITHREAD to 0" )
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/dict.in.cpp ${MPTK_BINARY_DIR}/src/libmptk/dict.cpp)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/atom.in.cpp ${MPTK_BINARY_DIR}/src/libmptk/atom.cpp)  
ENDIF(BUILD_MULTITHREAD)

##############################################################################
##header for configuration tests.
##############################################################################
INCLUDE(${MPTK_SOURCE_DIR}/CMake/CMakeGenerateMPTKHeader.cmake)


##############################################################################
## ADDING CONFIGURATION TO SUBFOLDERS
##############################################################################
# Go to compile mptk libraries
ADD_SUBDIRECTORY (src/libmptk)
# Go to compile matlab API in MEX Files
ADD_SUBDIRECTORY (src/matlab)
# Go to compile python Files
ADD_SUBDIRECTORY (src/python)
# Go to compile mptk plugin
ADD_SUBDIRECTORY (src/plugin/base)
ADD_SUBDIRECTORY (src/plugin/contrib)

# Compile the executable if requested by the end-user
IF(BUILD_EXECUTABLE)
	ADD_SUBDIRECTORY (src/utils)
ENDIF(BUILD_EXECUTABLE)

# Compile the documentation if requested by the end-user
IF(BUILD_DOCUMENTATION)
	ADD_SUBDIRECTORY(doc)
ENDIF(BUILD_DOCUMENTATION)

# Compile GUI if requested by the end-user
IF(BUILD_GUI)
	ADD_SUBDIRECTORY (src/gui) 
ENDIF(BUILD_GUI)

# Compile TEST if requested by the end-user
IF(BUILD_TESTING)
ADD_SUBDIRECTORY (src/tests) 
ENDIF(BUILD_TESTING)

INSTALL(DIRECTORY ${MPTK_BINARY_DIR}/install/mptk/reference DESTINATION mptk)
INSTALL(FILES ${MPTK_BINARY_DIR}/install/include/mptk.h DESTINATION include)
# --------------------------------
# setting path configuration file
# --------------------------------
SET(ADDINSTALL "")
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/path.xml.in ${MPTK_BINARY_DIR}/mptk/path.xml)
SET(ADDINSTALL "_INSTALL")
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/path.xml.in ${MPTK_BINARY_DIR}/install/path.xml)
SET(ADDINSTALL "")
INSTALL(FILES ${MPTK_BINARY_DIR}/install/path.xml DESTINATION mptk)

##############################################################################
# Packaging using CPack:
##############################################################################
# For Unix Makefiles / MinGW Makefiles: Generates standard UNIX makefiles:
# A hierarchy of UNIX makefiles is generated into the build tree.
# Any standard UNIX-style make program can build the project through the default make target.
# A "install" target is provided. For example "make install" or "mingw32-make install"
# Use "package" target for a binary distribution. For example "make package" or "mingw32-make package"
# Use "package_source" target for a source distribution. For example "make package_source" or "mingw32-make package_source"

# For Visual Studio and XCode project files
# A hierarchy of target is generated into the build tree and displayed in the IDE 
# Project provides "INSTALL" and "PACKAGE" target 

#In order to make a release:
#Set the release number in the file "version/buildversion.cmake"
#Make a tag with the number of the release
#Use Unix make to make a source release with command "make package_source"
#Upload the archive on the INRIA GForge MPTK project using the files admin page
#Add release in the corresponding categories MPTK with Add files page
#Filling the various required informations and validate the release
#Download the release and test if build correctly
#Make a binary relesase using Visual Studio using target "PACKAGE"
#Upload the archive on the INRIA GForge MPTK project using the files admin page
#Add release in the corresponding categories MPTK_win with Add files page
#Filling the various required informations and validate the release
#Download the release and test if build correctly

##############################################################################
# CPACK RULES
##############################################################################
# By default, do not warn when built on machines using only VS Express:
IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
	SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_SET_DESTDIR "ON")
SET(CPACK_PACKAGE_RELOCATABLE "FALSE")
SET(CPACK_CMAKE_GENERATOR ${CMAKE_GENERATOR})
SET(CPACK_PACKAGE_NAME "MPTK")
SET(CPACK_PACKAGE_VENDOR "PANAMA Project-team IRISA")
SET(CPACK_PACKAGE_VERSION ${BUILDVERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR "${BUILDVERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${BUILDVERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${BUILDVERSION_PATCH}")
SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_PROCESSOR}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "MPTK")

SET(CPACK_OUTPUT_CONFIG_FILE "${MPTK_BINARY_DIR}/CPackConfig.cmake")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${MPTK_SOURCE_DIR}/README.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Matching Pursuit Tool Kit")
SET(CPACK_RESOURCE_FILE_LICENSE "${MPTK_SOURCE_DIR}/COPYING.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "${MPTK_SOURCE_DIR}/README.txt")
INSTALL(FILES "${MPTK_SOURCE_DIR}/README.txt" DESTINATION mptk)
INSTALL(FILES "${MPTK_SOURCE_DIR}/COPYING.txt" DESTINATION mptk)

##############################################################################
# BINARY DISTRIBUTION:
##############################################################################
SET(CPACK_PACKAGE_CONTACT "matchingpursuite@irisa.fr")
SET(CPACK_INSTALL_CMAKE_PROJECTS "${MPTK_BINARY_DIR};MPTK;ALL;/")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Matching Pursuit Tool Kit")
SET(CPACK_IGNORE_FILES "/CMake/;/build/;/CMakeFiles/;/_CPack_Packages/;/src/;/bin/make_regression_constants;/install_manifest_/;.*~;/www/;/CVS//;/.svn/;/.git/;.cdtprojects;.project;/.settings/")
SET(CPACK_STRIP_FILES "")

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "MPTK-Source-${CPACK_PACKAGE_VERSION}")
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_IGNORE_FILES "/build/;/CMakeFiles/;/_CPack_Packages/;/bin/make_regression_constants;/install_manifest_/;.*~;/www/;/CVS//;/.svn/;/.git/;.cdtprojects;.project;/.settings/")

IF(WIN32)
	SET(CPACK_GENERATOR "NSIS")
	IF(CMAKE_CL_64)
		SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-x86_64-Windows")
    	SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-x86_64-Windows")
	ELSE(CMAKE_CL_64)
    	SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-i386-Windows")
    	SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-i386-Windows")
  	ENDIF(CMAKE_CL_64)
	SET(CPACK_SET_DESTDIR "OFF")
	# NSIS not used yet, cause of a bug with suitable ZIP programme
	# There is a bug in NSI that does not handle full unix paths properly. Make
	# sure there is at least one set of four (4) backlasshes.
	# SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
	SET(CPACK_NSIS_DISPLAY_NAME "MPTK")
	SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.irisa.fr/metiss/home_html-fr")
	SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\http://www.irisa.fr/metiss/home_html-fr")
	SET(CPACK_NSIS_CONTACT "matchingpursuite@irisa.fr")
	SET(CPACK_NSIS_MODIFY_PATH OFF)
ELSE(WIN32)
	IF(APPLE)
		SET(CPACK_GENERATOR "PackageMaker")
		IF(CMAKE_SIZEOF_VOID_P MATCHES "4")
			SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-i386-Mac")
			SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-i386-Mac")
		ELSE(CMAKE_SIZEOF_VOID_P MATCHES "4")
			SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-x86_64-Mac")
			SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-x86_64-Mac")
		ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "4")
	ELSE(APPLE)
		SET(CPACK_GENERATOR "DEB;RPM")
		SET(CPACK_DEBIAN_PACKAGE_DEPENDS "python, python-numpy, libfftw3-3, libsndfile1")
		SET(CPACK_DEBIAN_PACKAGE_SECTION "math")
		SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "The Matching Pursuit Tool Kit (MPTK) provides a fast implementation of the Matching Pursuit algorithm for the sparse decomposition of multichannel audio signals. It comprises a library, standalone utilities and wrappers for Matlab and Python enabling you to use MPTK directly and plot its results.")
		IF(CMAKE_SIZEOF_VOID_P MATCHES "4")
			SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-i386-Linux")
			SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-i386-Linux")
			SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
		ELSE(CMAKE_SIZEOF_VOID_P MATCHES "4")
			SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-x86_64-Linux")
			SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-x86_64-Linux")
			SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
		ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "4")
	ENDIF(APPLE)
ENDIF(WIN32)
INCLUDE(CPack)

##############################################################################
#Testing with CTest
##############################################################################
#CTest is a testing tool distributed as a part of CMake.
#To allow test the CMake variable BUILD_TESTING has to be set to ON
#Results are by default displayed on the INRIA dash server:
#http://cdash.inria.fr/CDash/index.php?project=MPTK

IF(BUILD_TESTING)
	ENABLE_TESTING()
	INCLUDE(CTest)
	INCLUDE(CTestConfig.cmake)

	#-------------------------------
	# Help test
	#-------------------------------
	ADD_TEST(HELP_MPD, ${MPTK_BINARY_DIR}/bin/mpd)
	ADD_TEST(HELP_GPD, ${MPTK_BINARY_DIR}/bin/gpd)
	ADD_TEST(HELP_MPR, ${MPTK_BINARY_DIR}/bin/mpr)
	ADD_TEST(HELP_MPF, ${MPTK_BINARY_DIR}/bin/mpf)
	ADD_TEST(HELP_MPCAT, ${MPTK_BINARY_DIR}/bin/mpcat)
	ADD_TEST(HELP_MPVIEW, ${MPTK_BINARY_DIR}/bin/mpview)
	ADD_TEST(HELP_MPDDEMIX, ${MPTK_BINARY_DIR}/bin/mpd_demix)
	
	#-------------------------------
	# Scratch test
	#-------------------------------
	ADD_TEST(SCRATCH_MPD_RELATIVE, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -d ${MPTK_DEFAULT_GABORDICT_FILENAME} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav -)
	ADD_TEST(SCRATCH_MPD_REPORT, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${MPTK_DEFAULT_GABORDICT_FILE} -n 100 -R 50 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav -)
	ADD_TEST(SCRATCH_WINDOWLEN, ${MPTK_BINARY_DIR}/bin/test_windowLen ${MPTK_BINARY_DIR}/mptk/path.xml ${MPTK_DEFAULT_GABORDICT_FILE} ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav)
	ADD_TEST(SCRATCH_ANYWAVE, ${MPTK_BINARY_DIR}/bin/test_anywave ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_DEFAULT_ANYWAVETABLE_FILE} ${MPTK_DEFAULT_ANYWAVEDICT_FILE})
	ADD_TEST(SCRATCH_BEHAVIOUR_IN_NOISE, ${MPTK_BINARY_DIR}/bin/test_behaviour_in_noise  ${MPTK_BINARY_DIR}/mptk/path.xml)
	ADD_TEST(SCRATCH_CONVOLUTION, ${MPTK_BINARY_DIR}/bin/test_convolution ${MPTK_BINARY_DIR}/mptk/path.xml)
	ADD_TEST(SCRATCH_FFT, ${MPTK_BINARY_DIR}/bin/test_fft ${MPTK_BINARY_DIR}/mptk/path.xml)
	ADD_TEST(SCRATCH_MESSAGE, ${MPTK_BINARY_DIR}/bin/test_messaging)
	ADD_TEST(SCRATCH_PARABOLIC_REGRESSION, ${MPTK_BINARY_DIR}/bin/test_parabolic_regression)
	ADD_TEST(SCRATCH_TEST_MP, ${MPTK_BINARY_DIR}/bin/test_mp  ${MPTK_BINARY_DIR}/mptk/path.xml)
	ADD_TEST(SCRATCH_STDIN, ${MPTK_BINARY_DIR}/bin/script/TestStdin${BATCHEXT})

	#-------------------------------
	# Compilation test
	#-------------------------------
	FOREACH(defaultDict ${dictcountCompil})
		GET_FILENAME_COMPONENT(DICT_FILE_NAME ${defaultDict} NAME_WE)
		ADD_TEST(COMPIL_MPD_BIN_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${defaultDict} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_mpd_bin_${DICT_FILE_NAME}.bin)
		ADD_TEST(COMPIL_MPD_TXT_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${defaultDict} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_mpd_txt_${DICT_FILE_NAME}.xml)
		ADD_TEST(COMPIL_GPD_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/gpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${defaultDict} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_gpd${DICT_FILE_NAME}.bin)
		ADD_TEST(COMPIL_MPR_BIN_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/mpr -C ${MPTK_BINARY_DIR}/mptk/path.xml  ${MPTK_TMP_PATH}/test_mpd_bin_${DICT_FILE_NAME}.bin ${MPTK_TMP_PATH}/test_mpr_bin_${DICT_FILE_NAME}.wav)
		ADD_TEST(COMPIL_MPR_TXT_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/mpr -C ${MPTK_BINARY_DIR}/mptk/path.xml  ${MPTK_TMP_PATH}/test_mpd_txt_${DICT_FILE_NAME}.xml ${MPTK_TMP_PATH}/test_mpr_txt_${DICT_FILE_NAME}.wav)
		SET(bookCompilBin ${bookCompilBin} ${MPTK_TMP_PATH}/test_mpd_bin_${DICT_FILE_NAME}.bin)
		SET(bookCompilTxt ${bookCompilTxt} ${MPTK_TMP_PATH}/test_mpd_txt_${DICT_FILE_NAME}.xml)
	ENDFOREACH(defaultDict)
	ADD_TEST(COMPIL_MPD_DEMIX, ${MPTK_BINARY_DIR}/bin/mpd_demix -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${MPTK_DEFAULT_DICT_LIST_FILE} -M ${MPTK_DEFAULT_MIXER_FILE} -n 10 ${MPTK_EXAMPLE_STEREOSIGNAL_FILE} ${MPTK_TMP_PATH}/test_demix.bin)	
	ADD_TEST(COMPIL_MPF, ${MPTK_BINARY_DIR}/bin/mpf -C ${MPTK_BINARY_DIR}/mptk/path.xml --Freq=[0:110] --len=[0:256] ${MPTK_EXAMPLE_BOOK_FILE} ${MPTK_TMP_PATH}/test_mpf_bookYes.bin ${MPTK_TMP_PATH}/test_mpf_bookNo.bin)
	ADD_TEST(COMPIL_MPVIEW, ${MPTK_BINARY_DIR}/bin/mpview -C ${MPTK_BINARY_DIR}/mptk/path.xml ${MPTK_EXAMPLE_BOOK_FILE} ${MPTK_TMP_PATH}/test_mpview.flt)
	ADD_TEST(COMPIL_SCRIPT_EXEC, ${MPTK_BINARY_DIR}/bin/script/TestExecCycle${BATCHEXT})
	ADD_TEST(COMPIL_MPCAT_BIN, ${MPTK_BINARY_DIR}/bin/mpcat -f -C ${MPTK_BINARY_DIR}/mptk/path.xml ${bookCompilBin} ${MPTK_TMP_PATH}/test_bin_concatene.bin)
	ADD_TEST(COMPIL_MPCAT_TXT, ${MPTK_BINARY_DIR}/bin/mpcat -f -C ${MPTK_BINARY_DIR}/mptk/path.xml ${bookCompilTxt} ${MPTK_TMP_PATH}/test_txt_concatene.xml)

	#-------------------------------
	# Installation test
	#-------------------------------
	FOREACH(defaultDict ${dictcountInstall})
		GET_FILENAME_COMPONENT(DICT_FILE_NAME ${defaultDict} NAME_WE)
		#ADD_TEST(INSTALL_MPD_${DICT_FILE_NAME}, ${CMAKE_INSTALL_PREFIX}/bin/mpd -C ${CMAKE_INSTALL_PREFIX}/mptk/path.xml -D ${defaultDict} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_mpd${DICT_FILE_NAME}.bin)
		ADD_TEST(INSTALL_MPD_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${defaultDict} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_mpd${DICT_FILE_NAME}.bin)
		#ADD_TEST(INSTALL_GPD_${DICT_FILE_NAME}, ${CMAKE_INSTALL_PREFIX}/bin/gpd -C ${CMAKE_INSTALL_PREFIX}/mptk/path.xml -D ${defaultDict} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_gpd${DICT_FILE_NAME}.bin)
		ADD_TEST(INSTALL_GPD_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/gpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${defaultDict} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_gpd${DICT_FILE_NAME}.bin)
		#ADD_TEST(INSTALL_MPR_${DICT_FILE_NAME}, ${CMAKE_INSTALL_PREFIX}/bin/mpr -C ${CMAKE_INSTALL_PREFIX}/mptk/path.xml  ${MPTK_TMP_PATH}/test_mpd${DICT_FILE_NAME}.bin ${MPTK_TMP_PATH}/test_mpr${DICT_FILE_NAME}.wav)
		ADD_TEST(INSTALL_MPR_${DICT_FILE_NAME}, ${MPTK_BINARY_DIR}/bin/mpr -C ${MPTK_BINARY_DIR}/mptk/path.xml  ${MPTK_TMP_PATH}/test_mpd${DICT_FILE_NAME}.bin ${MPTK_TMP_PATH}/test_mpr${DICT_FILE_NAME}.wav)
	ENDFOREACH(defaultDict)
	ADD_TEST(INSTALL_MPD_DEMIX, ${CMAKE_INSTALL_PREFIX}/bin/mpd_demix -C ${CMAKE_INSTALL_PREFIX}/mptk/path.xml -D ${MPTK_DEFAULT_DICT_LIST_FILE} -M ${MPTK_DEFAULT_MIXER_FILE} -n 10 ${MPTK_EXAMPLE_STEREOSIGNAL_FILE} ${MPTK_TMP_PATH}/test_demix.bin)	
	ADD_TEST(INSTALL_MPF, ${CMAKE_INSTALL_PREFIX}/bin/mpf -C ${CMAKE_INSTALL_PREFIX}/mptk/path.xml --Freq=[0:110] --len=[0:256] ${MPTK_EXAMPLE_BOOK_FILE} ${MPTK_TMP_PATH}/test_mpf_bookYes.bin ${MPTK_TMP_PATH}/test_mpf_bookNo.bin)
	ADD_TEST(INSTALL_MPVIEW, ${CMAKE_INSTALL_PREFIX}/bin/mpview -C ${CMAKE_INSTALL_PREFIX}/mptk/path.xml ${MPTK_EXAMPLE_BOOK_FILE} ${MPTK_TMP_PATH}/test_mpview.flt)

	#-------------------------------
	# Matlab test
	#-------------------------------
	IF(NOT PIPOL_IMAGE)
		ADD_TEST(MATLABGETMPTKINFO,  ${MPTK_BINARY_DIR}/bin/script/TestMatlab${BATCHEXT})
	ENDIF(NOT PIPOL_IMAGE)
ENDIF(BUILD_TESTING)

