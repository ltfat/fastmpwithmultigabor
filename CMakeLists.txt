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
OPTION(BUILD_PYTHON_FILES "Enable building python files." OFF)
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
      SET(CMAKE_INSTALL_PREFIX
        "${MPTK_GENERIC_PROGRAM_FILES}/${PROJECT_NAME}"
        CACHE PATH "Install path prefix, prepended onto install directories."
        FORCE)
      SET(MPTK_GENERIC_SYSTEM_DRIVE)
    ENDIF(CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
    MARK_AS_ADVANCED(CLEAR CMAKE_INSTALL_PREFIX)
    # Recherche des librairies : Suffixes ".dll" et prefixes : "lib"
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    SET(CMAKE_FIND_LIBRARY_PREFIXES "lib;")
  ENDIF(WIN32)
ENDIF(UNIX)

##############################################################################
# Setting the compilation flags in dynamic format:
##############################################################################
IF(WIN32)
	IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
		SET(CMAKE_CXX_COMPILER "${CMAKE_CXX_FLAGS} -fPIC ")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
	ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
ELSE(WIN32)
	IF(APPLE)
		IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -single_module -headerpad_max_install_names") 
			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
		ELSE(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -single_module -headerpad_max_install_names") 
		ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
	ELSE(APPLE)
		IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wl,--export-dynamic")
		ELSE(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
			SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--export-dynamic")
		ENDIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "ia64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL  "x86_64")
	ENDIF(APPLE)
ENDIF(WIN32)

##############################################################################
# Dash server testing :
##############################################################################
set(BUILD_CXX ${CMAKE_CXX_COMPILER})
STRING(REGEX REPLACE "/" "_" BUILD_CXX "${BUILD_CXX}")

IF(BUILD_TESTING AND DASH_TESTING)
	INCLUDE(${MPTK_SOURCE_DIR}/CMake/Pipol.cmake)
	IF(PIPOL_IMAGE)
		SET(BUILDNAME "CMAKE-MptkBuild-${CMAKE_GENERATOR}-${BUILD_CXX}-${PIPOL_IMAGE_NAME}")
		SET(SITE "${PIPOL_SITE}")
	ELSE(PIPOL_IMAGE)
		SET(BUILDNAME "CMAKE-MptkBuild-${CMAKE_GENERATOR}-${BUILD_CXX}-${CMAKE_SYSTEM_NAME}")
	ENDIF(PIPOL_IMAGE)
ENDIF(BUILD_TESTING AND DASH_TESTING)

##############################################################################
# Setting compilation options :
##############################################################################
#Select the ADD_DEFINITIONS according to selected options
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
	# With Visual Studio 2005, Microsoft deprecates the standard C library, for
	# example fopen() and sprintf(), to non-portable functions fopen_s() and
	# sprintf_s(). These functions are considered by Microsoft more secure. This is
	# a worthwhile exercise ! The use of these deprecated functions causes a lot of
	# warnings. To suppress it, we add the _CRT_SECURE_NO_DEPRECATE preprocessor
	# definition
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
	# Case of other compiler/environment : tags depend on build mode (debug/release
	IF(BUILD_DEBUG) 
		# Case of debug mode
		IF (BUILD_VERBOSE)
			ADD_DEFINITIONS(-g -O3 -Wall -pipe -v)
		ELSE(BUILD_VERBOSE)
			ADD_DEFINITIONS(-g -O3 -quiet -pipe)
		ENDIF(BUILD_VERBOSE)
		# Add debug mode for mex files
		SET(MATLAB_COMPILATION "-v -g")		
	ELSE(BUILD_DEBUG)
		# Case of release mode
		IF (BUILD_VERBOSE)
			ADD_DEFINITIONS( -O3 -Wall -pipe -v -DNDEBUG)
		ELSE(BUILD_VERBOSE)
			ADD_DEFINITIONS( -O3 -quiet -pipe -DNDEBUG)
		ENDIF(BUILD_VERBOSE)
		# Add debug mode for mex files
		SET(MATLAB_COMPILATION "-v")		
	ENDIF(BUILD_DEBUG)
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")

#Set  BUILDVERSION, PACKAGENAME, PACKAGENAMEFULL and AUTHORS according to text files in /version

INCLUDE(${MPTK_SOURCE_DIR}/version/buildversion.cmake)
INCLUDE(${MPTK_SOURCE_DIR}/CMake/CommonConfig.cmake)
FILE(READ ${MPTK_SOURCE_DIR}/version/AUTHORS AUTHORS)
SET(BUILDVERSION_GUILLEMETS \"${BUILDVERSION_MAJOR}.${BUILDVERSION_MINOR}.${BUILDVERSION_PATCH}\")
SET(BUILDVERSION "${BUILDVERSION_MAJOR}.${BUILDVERSION_MINOR}.${BUILDVERSION_PATCH}")

#Define the target for install

SET(MPTK_FFTW_WISDOM_FILE ${MPTK_BINARY_DIR}/mptk/default_fftw_wisdom_file)
SET(MPTK_FFTW_WISDOM_FILE_INSTALL ${CMAKE_INSTALL_PREFIX}/mptk/default_fftw_wisdom_file)
SET(MPTK_REFERENCE_PATH ${MPTK_BINARY_DIR}/mptk/reference)
SET(MPTK_REFERENCE_PATH_INSTALL ${CMAKE_INSTALL_PREFIX}/mptk/reference)

IF(UNIX)
SET(MPTK_TMP_PATH /tmp)
ELSE(UNIX)
IF(WIN32)
SET(MPTK_TMP_PATH C:/Temp)
ENDIF(WIN32)
ENDIF(UNIX)

# --------------------------
# DEFAULT REFERENCE FILES
# --------------------------
SET(MPTK_DEFAULT_DICT_FILENAME dictionary/dic_gabor_two_scales.xml)
SET(MPTK_DEFAULT_MDCTDICT_FILENAME dictionary/dic_mdct_two_scales.xml)
SET(MPTK_DEFAULT_MIXER_FILENAME  mixer/mix_58_mixer.txt)
SET(MPTK_DEFAULT_ANYWAVETABLE_DATA_FILENAME wavetable/defaultWaveTableData.bin)
SET(MPTK_DEFAULT_DICT_LIST_INSTALL_PATH dictionary)
SET(MPTK_DEFAULT_ANYWAVETABLE_INSTALL_PATH wavetable)
SET(MPTK_DEFAULT_ANYWAVEDICT_INSTALL_PATH dictionary)

# Copied default files
FOREACH(default DICT MDCTDICT MIXER ANYWAVETABLE_DATA)
	SET(MPTK_DEFAULT_${default}_FILE          ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_${default}_FILENAME})
	SET(MPTK_DEFAULT_${default}_FILE_INSTALL  ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_DEFAULT_${default}_FILENAME})
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_${default}_FILENAME} ${MPTK_DEFAULT_${default}_FILE} COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_${default}_FILENAME}  ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_DEFAULT_${default}_FILENAME} COPYONLY)
ENDFOREACH(default)

# EXAMPLE REFERENCE FILES
SET(MPTK_EXAMPLE_BOOK_FILENAME book/exampleBook.bin)
SET(MPTK_EXAMPLE_SIGNAL_FILENAME signal/glockenspiel.wav)
SET(MPTK_EXAMPLE_STEREOSIGNAL_FILENAME signal/mix_58_stereo.wav)

FOREACH(example BOOK SIGNAL STEREOSIGNAL)
	SET(MPTK_EXAMPLE_${example}_FILE          ${MPTK_REFERENCE_PATH}/${MPTK_EXAMPLE_${example}_FILENAME})
	SET(MPTK_EXAMPLE_${example}_FILE_INSTALL  ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_EXAMPLE_${example}_FILENAME})
	# Copy in the build directory
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_EXAMPLE_${example}_FILENAME} ${MPTK_EXAMPLE_${example}_FILE} COPYONLY)
	# Copy in the pre-install directory
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_EXAMPLE_${example}_FILENAME}  ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_EXAMPLE_${example}_FILENAME} COPYONLY)
ENDFOREACH(example)

# --------------------------
# GENERATED DEFAULT FILES
# --------------------------
SET(MPTK_DEFAULT_DICT_LIST_FILENAME  dictionary/dict-list.txt)
SET(MPTK_DEFAULT_ANYWAVETABLE_FILENAME wavetable/defaultWaveTable.xml)
SET(MPTK_DEFAULT_ANYWAVEDICT_FILENAME dictionary/dic_anywave.xml)

FOREACH(default DICT_LIST ANYWAVETABLE ANYWAVEDICT)
	SET(MPTK_DEFAULT_${default}_FILE          ${MPTK_REFERENCE_PATH}/${MPTK_DEFAULT_${default}_FILENAME})
	SET(MPTK_DEFAULT_${default}_FILE_INSTALL  ${MPTK_REFERENCE_PATH_INSTALL}/${MPTK_DEFAULT_${default}_FILENAME})
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_${default}_FILENAME}.in ${MPTK_DEFAULT_${default}_FILE} @ONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/reference/${MPTK_DEFAULT_${default}_FILENAME}.install ${MPTK_BINARY_DIR}/install/mptk/reference/${MPTK_DEFAULT_${default}_FILENAME} @ONLY)
ENDFOREACH(default)

# Depending on compiler+IDE (Make/Xcode/Visual), need to specify postfix for build type
#--------------------------------------------------------------------------------------
# Case of Visual Studio 
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    IF (BUILD_DEBUG)
      # Case of debug mode
      SET(MPTK_DEBUG_RELEASE "/debug")
      SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/debug)
      SET(MPTK_LIB_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/lib)
    ELSE (BUILD_DEBUG)
    	# Case of release mode
      SET(MPTK_DEBUG_RELEASE "/release")
    	SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/release)
    	SET(MPTK_LIB_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/lib)
    ENDIF (BUILD_DEBUG)
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
  # Case of Xcode
  IF( CMAKE_GENERATOR STREQUAL "Xcode")
    # We would like to select a single build mode, would we ?
    # or to select it dynamically there is the CMAKE_CFG_INTDIR stuff
    IF (BUILD_DEBUG)
    	SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/Debug)
    	SET(MPTK_LIB_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/lib)
    ELSE (BUILD_DEBUG)
    	SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib/Release)
    	SET(MPTK_LIB_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/lib)
    ENDIF (BUILD_DEBUG)
  ELSE( CMAKE_GENERATOR STREQUAL "Xcode")
    # Generic case
    SET(MPTK_LIB_DIR ${MPTK_BINARY_DIR}/lib)
    SET(MPTK_LIB_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/lib)
  ENDIF(CMAKE_GENERATOR STREQUAL "Xcode") 
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")

# ----------------------------------------------------------------------------
# setting path configuration file
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/path.xml.in ${MPTK_BINARY_DIR}/mptk/path.xml @ONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/path.xml.install ${MPTK_BINARY_DIR}/path.xml @ONLY)

#-----------------------------------------------------------------------------
# setting bin path
IF(EXECUTABLE_OUTPUT_PATH)
ELSE(EXECUTABLE_OUTPUT_PATH)
	SET(EXECUTABLE_OUTPUT_PATH "${MPTK_BINARY_DIR}/bin/${OS}" CACHE PATH "Single output directory for building all executables.")
ENDIF(EXECUTABLE_OUTPUT_PATH)

#-----------------------------------------------------------------------------
#setting lib path
IF(LIBRARY_OUTPUT_PATH)
ELSE(LIBRARY_OUTPUT_PATH)
	SET(LIBRARY_OUTPUT_PATH "${MPTK_BINARY_DIR}/lib/${OS}" CACHE PATH "Single output directory for building all libraries.")
ENDIF(LIBRARY_OUTPUT_PATH)

## FINDING EXTERNAL LIBRARIES
#############################
# Setting path to libraries and modules for apps
SET(LIBDSP_INCLUDE_DIR  ${MPTK_SOURCE_DIR}/src/libdsp_windows)
SET(UTILS_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/utils )
SET(GUI_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/gui )

# Include file to find libraries
INCLUDE( ${MPTK_SOURCE_DIR}/CMake/FindExternalLibs.cmake )
#-----------------------------------------------------------------------------

#Set option for building and linking:
#Set directories for linker
LINK_DIRECTORIES( ${PTHREAD_LIBRARY_PATH} 
                  ${SNDFILE_LIBRARY_PATH} 
                  ${FFTW3_LIBRARY_PATH} )
#set the include directories paths for building
INCLUDE_DIRECTORIES(/usr/include 
		${MPTK_SOURCE_DIR}               	# TO INCLUDE mp_system.h
   		${MPTK_SOURCE_DIR}/src/tinyxml   	# TO INCLUDE TinyXML code
		${MPTK_BINARY_DIR}               	# TO INCLUDE config.h
		${PTHREAD_INCLUDE_DIR}				# AUTOMATICALLY CREATED IN FindPThread
		${MPTK_SOURCE_DIR}/src/libmptk
		${MPTK_BINARY_DIR}/src/libmptk/ 	# TO INCLUDE regression_constant.h and mptk.h, automatically generated 
		${LIBDSP_INCLUDE_DIR} 
		${FFTW3_INCLUDE_DIR} 
		${SNDFILE_INCLUDE_DIR} 
		${MATLAB_INCLUDE_DIR} 
		)
#-----------------------------------------------------------------------------
##Platform configuration tests.
#############################
INCLUDE(${MPTK_SOURCE_DIR}/CMake/CMakeBackwardCompatibilityC.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestForANSIStreamHeaders.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestForSTDNamespace.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestForANSIForScope.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
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

# Simulate old CMakeBackwardCompatibilityCXX test.
# In CMake 2.4 and up this could be just
# INCLUDE(${CMAKE_ROOT}/Modules/TestForSSTREAM.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake)
CHECK_INCLUDE_FILE_CXX("sstream" CMAKE_HAS_ANSI_STRING_STREAM)
IF(NOT CMAKE_HAS_ANSI_STRING_STREAM)
  SET(CMAKE_NO_ANSI_STRING_STREAM 1 CACHE INTERNAL
    "Does the compiler support sstream or stringstream.")
ENDIF(NOT CMAKE_HAS_ANSI_STRING_STREAM)
#-----------------------------------------------------------------------------
##header for configuration tests.
#############################
INCLUDE(${MPTK_SOURCE_DIR}/CMake/CMakeGenerateMPTKHeader.cmake)


#-----------------------------------------------------------------------------
## STARTING WITH SUBFOLDERS
# Include src folder to compiling proccess
#
# Commented includes might be included twice with recursive include of src
#
#
IF(BUILD_DOCUMENTATION)
ADD_SUBDIRECTORY(${MPTK_SOURCE_DIR}/doc)
ENDIF(BUILD_DOCUMENTATION)
ADD_SUBDIRECTORY( src)

INSTALL(DIRECTORY ${MPTK_BINARY_DIR}/install/mptk/reference DESTINATION mptk)
INSTALL(FILES ${MPTK_BINARY_DIR}/install/include/mptk.h DESTINATION include)

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
#Set the release number in this file
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

# CPACK RULES
#-----------------------------------------------------------------------------
INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_SET_DESTDIR "ON")
SET(CPACK_PACKAGE_RELOCATABLE "FALSE")
SET(CPACK_CMAKE_GENERATOR ${CMAKE_GENERATOR})
SET(CPACK_PACKAGE_NAME "MPTK")
SET(CPACK_PACKAGE_VENDOR "METISS Project IRISA")
SET(CPACK_PACKAGE_VERSION ${BUILDVERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR "${BUILDVERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${BUILDVERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${BUILDVERSION_PATCH}")
SET(CPACK_SYSTEM_NAME "${CMAKE_OSX_ARCHITECTURES}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "MPTK")

SET(CPACK_OUTPUT_CONFIG_FILE "${MPTK_BINARY_DIR}/CPackConfig.cmake")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${MPTK_SOURCE_DIR}/README.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Matching Pursuit Tool Kit")
SET(CPACK_RESOURCE_FILE_LICENSE "${MPTK_SOURCE_DIR}/COPYING.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "${MPTK_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_README "${MPTK_SOURCE_DIR}/src/utils/readme.txt")
INSTALL(FILES "${MPTK_SOURCE_DIR}/README.txt" DESTINATION mptk)
INSTALL(FILES "${MPTK_SOURCE_DIR}/COPYING.txt" DESTINATION mptk)

# BINARY DISTRIBUTION:
#-----------------------------------------------------------------------------
SET(CPACK_INSTALL_CMAKE_PROJECTS "${MPTK_BINARY_DIR};MPTK;ALL;/")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Matching Pursuit Tool Kit")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${MPTK-CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CMake/;/CMakeFiles/;/_CPack_Packages/;/src/;/bin/make_regression_constants;/install_manifest_/;.*~;/www/;/CVS//;/.svn/;.cdtprojects;.project;/.settings/")
SET(CPACK_STRIP_FILES "")

IF(WIN32)
	SET(CPACK_GENERATOR "NSIS")
	SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-Win32")
	SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-Win32")
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
		SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-Mac")
		SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-Mac")
	ELSE(APPLE)
		SET(CPACK_GENERATOR "DEB;RPM")
		SET(CPACK_PACKAGE_CONTACT "matchingpursuite@irisa.fr")
		SET(CPACK_PACKAGE_FILE_NAME "MPTK-binary-${CPACK_PACKAGE_VERSION}-Linux")
		SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-binary-${CPACK_PACKAGE_VERSION}-Linux")
	ENDIF(APPLE)
ENDIF(WIN32)
INCLUDE(CPack)

##############################################################################
#Testing with CTest
##############################################################################
#CTest is a testing tool distributed as a part of CMake.
#To allow test the CMake variable BUILD_TESTING has to be set to ON
#A simple way to submit Experimental dashboard is:
#ctest -D Experimental
#To show what targets are available, run:
#ctest -D Help
#
#Results are by default displayed on the INRIA dash server:
#http://cdash.inria.fr/CDash/index.php?project=MPTK
#The "CTestConfig.cmake" file in the toplevel source directory set the dashboard preferences.

IF(BUILD_TESTING)
	ENABLE_TESTING()
	INCLUDE(CTest)
	INCLUDE(CTestConfig.cmake)
	# TEST with CTest:
	SET(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage  ${CMAKE_CXX_FLAGS}") 
	ADD_TEST(MESSAGE, ${MPTK_BINARY_DIR}/bin/test_messaging)
	ADD_TEST(FFT, ${MPTK_BINARY_DIR}/bin/test_fft ${MPTK_BINARY_DIR}/mptk/path.xml)
	ADD_TEST(PARABOLIC_REGRESSION, ${MPTK_BINARY_DIR}/bin/test_parabolic_regression)
	ADD_TEST(BEHAVIOUR_IN_NOISE, ${MPTK_BINARY_DIR}/bin/test_behaviour_in_noise  ${MPTK_BINARY_DIR}/mptk/path.xml)
	ADD_TEST(MPD, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${MPTK_DEFAULT_DICT_FILE} -n 10 ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_mpd.bin)
	ADD_TEST(MPDANYWAVE, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${MPTK_DEFAULT_ANYWAVEDICT_FILE} -n 10 ${MPTK_REFERENCE_PATH}/signal/anywave_signal.wav ${MPTK_TMP_PATH}/test_mpdanywave.bin)
	ADD_TEST(MPR, ${MPTK_BINARY_DIR}/bin/mpr -C ${MPTK_BINARY_DIR}/mptk/path.xml ${MPTK_EXAMPLE_BOOK_FILE} ${MPTK_TMP_PATH}/test_mpr.wav)
	ADD_TEST(MPF, ${MPTK_BINARY_DIR}/bin/mpf -C ${MPTK_BINARY_DIR}/mptk/path.xml --Freq=[0:110] --len=[0:256] ${MPTK_EXAMPLE_BOOK_FILE} ${MPTK_TMP_PATH}/test_mpf_bookYes.bin ${MPTK_TMP_PATH}/test_mpf_bookNo.bin)
	ADD_TEST(TEST_MP, ${MPTK_BINARY_DIR}/bin/test_mp  ${MPTK_BINARY_DIR}/mptk/path.xml)
	ADD_TEST(ANYWAVE, ${MPTK_BINARY_DIR}/bin/test_anywave ${MPTK_REFERENCE_PATH}/signal/anywave_signal.wav ${MPTK_DEFAULT_ANYWAVETABLE_FILE} ${MPTK_DEFAULT_ANYWAVEDICT_FILE})
	ADD_TEST(MPD_DEMIX, ${MPTK_BINARY_DIR}/bin/mpd_demix -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${MPTK_DEFAULT_DICT_LIST_FILE} -M ${MPTK_DEFAULT_MIXER_FILE} -n 10 ${MPTK_EXAMPLE_STEREOSIGNAL_FILE} ${MPTK_TMP_PATH}/test_demix.bin)
	ADD_TEST(SCRIPT1, ${MPTK_BINARY_DIR}/bin/script/test_mpcycle${BATCHEXT})
	ADD_TEST(GPD, ${MPTK_BINARY_DIR}/bin/gpd -C ${MPTK_BINARY_DIR}/mptk/path.xml -D ${MPTK_DEFAULT_MDCTDICT_FILE} -n 10 -q ${MPTK_REFERENCE_PATH}/signal/glockenspiel.wav ${MPTK_TMP_PATH}/test_gpd.bin)
ENDIF(BUILD_TESTING)

