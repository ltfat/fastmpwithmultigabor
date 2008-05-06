## Main CMakeLists.txt of the space Research of METISS Team
PROJECT(MPTK)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
## SETTING COMPILATION OPTION
#############################
# Compilation of modules:
OPTION(BUILD_GUI "Build Graphical user interface" OFF) 
# Enable testing:
OPTION(BUILD_TESTING "Build Tests" OFF) 
# Allows shard compiling:
OPTION(BUILD_SHARED_LIBS "Build shared libraries." ON)
#Allows generating the doc:
OPTION(BUILD_DOCUMENTATION "Generating the doxygen documentation." OFF)
#Allows generating the doc for GUI:
OPTION(BUILD_DOCUMENTATION_GUI "Generating the doxygen documentation for GUI." OFF)
# Allows building executable:
OPTION(BUILD_EXECUTABLE "Build executable." ON)
# Allows multithreading:
OPTION(BUILD_MULTITHREAD "Enable multithread." OFF)
# Allows verbose:
OPTION(BUILD_VERBOSE "Enable verbose option when compile (-v)." ON)
# Allows MPTK Debug:
OPTION(BUILD_DEBUG "Enable MPTK Debug mode." OFF)
# Allows Matlab MEX files :
OPTION(BUILD_MATLAB_MEX_FILES "Enable building Matlab MEX files." OFF)
# Allows build python extension :
OPTION(BUILD_PYTHON_FILES "Enable building python files." OFF)
# Allows build experimental feature :
OPTION(BUILD_EXPERIMENTAL "Enable building experimental files." OFF)
# Allows build experimental feature :
OPTION(BUILD_MATLAB_MEX_FILES_EXPERIMENTAL "Enable building Matlab experimental MEX files." OFF)
# Allows build RPM experimental feature :
SET(BUILD_RPM OFF)
#OPTION(BUILD_RPM "Allows build RPM experimental feature." OFF)
#You can reference the special $ENV{} variable to get environment values
#like any other CMake variable.
#-----------------------------------------------------------------------------
## SETTING GLOBAL VARIABLES
###########################
# setting globlal compilation definitions
#-----------------------------------------------------------------------------
# Choose a default CMAKE_INSTALL_PREFIX on Windows.  CMake 2.2 and
# above already choose a good default, so force a change only if the
# current value looks like the CMake 2.0 default.

IF(UNIX)
  IF(APPLE)
SET(MPTK_USER_LOCAL_PATH $ENV{HOME}/local/bin)
SET(MPTK_USER_INCLUDE_PATH $ENV{HOME}/local/include)
SET(MPTK_USER_LIB_PATH $ENV{HOME}/local/lib)
  ELSE(APPLE)
SET(MPTK_USER_LOCAL_PATH $ENV{HOME}/local/bin)
SET(MPTK_USER_INCLUDE_PATH $ENV{HOME}/local/include)
SET(MPTK_USER_LIB_PATH $ENV{HOME}/local/lib)
  ENDIF(APPLE)
ELSE(UNIX)
IF(WIN32)
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
ENDIF(WIN32)
ENDIF(UNIX)

# With Visual Studio 2005, Microsoft deprecates the standard C library, for
# example fopen() and sprintf(), to non-portable functions fopen_s() and
# sprintf_s(). These functions are considered by Microsoft more secure. This is
# a worthwhile exercise ! The use of these deprecated functions causes a lot of
# warnings. To suppress it, we add the _CRT_SECURE_NO_DEPRECATE preprocessor
# definition
IF(WIN32)
  IF(MSVC80 OR MSVC90)
    ADD_DEFINITIONS("-D_CRT_SECURE_NO_DEPRECATE")
  ENDIF(MSVC80 OR MSVC90)
ENDIF(WIN32)

#Define compilation tag
#Select the ADD_DEFINITIONS according to selected options
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
IF(BUILD_DEBUG)
IF (BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe -v)
ELSE(BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe)
ENDIF(BUILD_VERBOSE)
ELSE(BUILD_DEBUG)
IF (BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe -DNDEBUG)
ELSE(BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe -DNDEBUG)
ENDIF(BUILD_VERBOSE)
ENDIF(BUILD_DEBUG)
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")

#Set  BUILDVERSION, BUILDVERSION, BUILDVERSION and AUTHORS according to text files in /version
FILE(READ ${MPTK_SOURCE_DIR}/version/buildversion.cmake BUILDVERSION)
FILE(READ ${MPTK_SOURCE_DIR}/version/packagename.cmake PACKAGENAME)
FILE(READ ${MPTK_SOURCE_DIR}/version/packagenamefull.cmake PACKAGENAMEFULL)
FILE(READ ${MPTK_SOURCE_DIR}/version/AUTHORS AUTHORS)

#Define the target for install
SET(BIN_HOME     ${CMAKE_INSTALL_PREFIX}/bin)
SET(INCLUDE_HOME ${CMAKE_INSTALL_PREFIX}/include)
SET(LIB_HOME     ${CMAKE_INSTALL_PREFIX}/lib)

SET(MPTK_PLUGIN_DIR ${MPTK_BINARY_DIR}/lib)
SET(MPTK_FTTW_WISDOM_FILE ${MPTK_BINARY_DIR}/bin/default_fftw_wisdom_file)
SET(MPTK_FTTW_WISDOM_FILE_INSTALL ${CMAKE_INSTALL_PREFIX}/bin/default_fftw_wisdom_file)
SET(MPTK_REFERENCE_PATH_INSTALL ${CMAKE_INSTALL_PREFIX}/bin/reference)
SET(MPTK_REFERENCE_PATH ${MPTK_BINARY_DIR}/bin/reference)

IF( CMAKE_CXX_COMPILER STREQUAL "cl")
SET(MPTK_PLUGIN_DIR_INSTALL ${MPTK_BINARY_DIR}/lib/release)
SET(LIB_HOME_INSTALL ${CMAKE_INSTALL_PREFIX}/lib/release)
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
SET(MPTK_PLUGIN_DIR_INSTALL ${MPTK_BINARY_DIR}/lib)
SET(LIB_HOME_INSTALL ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")

#-----------------------------------------------------------------------------
# setting bin path
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/path.xml.in ${MPTK_BINARY_DIR}/bin/path.xml @ONLY)
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

#-----------------------------------------------------------------------------
#setting tmp path
#IF(NOT WIN32)
#   EXEC_PROGRAM(whoami OUTPUT_VARIABLE USERNAME)
#   SET(TMP "/tmp/tmpMPTK/${USERNAME}")
#   MAKE_DIRECTORY(${TMP})
#ELSE(NOT WIN32)
#   SET(TMP "c:/tmp/tmpMPTK/")
#   MAKE_DIRECTORY(${TMP})
#ENDIF(NOT WIN32)
#-----------------------------------------------------------------------------

## FINDING EXTERNAL LIBRARIES
#############################
# Setting path to libraries and modules for apps
SET(LIBMPTK_INCLUDE_DIR  ${MPTK_SOURCE_DIR}/src/libmptk )
SET(PORTAUDIO_INCLUDE_DIR  ${MPTK_SOURCE_DIR}/src/gui/portaudio_v18_1/pa_common
			               ${MPTK_SOURCE_DIR}/src/gui/portaudio_v18_1/pablio/
			               ${MPTK_SOURCE_DIR}/src/gui/portaudio_v18_1/pa_win_ds
			               )
SET(LIBDSP_INCLUDE_DIR  ${MPTK_SOURCE_DIR}/src/libdsp_windows)
SET(UTILS_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/utils )
SET(GUI_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/gui )

# Include file to find libraries
INCLUDE( ${MPTK_SOURCE_DIR}/CMake/FindExternalLibs.cmake )
#-----------------------------------------------------------------------------

#Set option for building and linking:
#Set directories for linker
LINK_DIRECTORIES( ${MPTK_SOURCE_DIR}/src/libmptk 
                  ${PTHREAD_LIBRARY_FILE} 
                  ${SNDFILE_LIBRARY_FILE} 
                  ${FFTW3_LIBRARY_FILE} 
                  ${wxWidgets_LIBRARY_DIRS} )
#set the include directories paths for building
INCLUDE_DIRECTORIES(/usr/include 
					${MPTK_SOURCE_DIR} 
					${MPTK_SOURCE_DIR}/src/tinyxml 
					${MPTK_BINARY_DIR} 
					${PTHREAD_INCLUDE_DIR} 
					${LIBMPTK_INCLUDE_DIR}  
					${PORTAUDIO_INCLUDE_DIR} 
					${MPTK_BINARY_DIR}/src/libmptk/ 
					${LIBDSP_INCLUDE_DIR} 
					${FFTW3_INCLUDE_DIR} 
					${SNDFILE_INCLUDE_DIR} 
					${wxWidgets_INCLUDE_DIRS} 
					${MPTK_BINARY_DIR}/src/gui/Qt 
					${QT_INCLUDE_DIR} 
					${QT_INCLUDE_PATH}
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
ADD_SUBDIRECTORY(${MPTK_SOURCE_DIR}/src/libmptk)
ADD_SUBDIRECTORY(${MPTK_SOURCE_DIR}/src/libdsp_windows)
IF(BUILD_GUI)
ADD_SUBDIRECTORY(${MPTK_SOURCE_DIR}/src/gui)
ENDIF(BUILD_GUI)
IF(BUILD_DOCUMENTATION)
ADD_SUBDIRECTORY(${MPTK_SOURCE_DIR}/doc)
ENDIF(BUILD_DOCUMENTATION)
ADD_SUBDIRECTORY( src)

#-----------------------------------------------------------------------------
## CPACK RULES
INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_NAME "MPTK")
SET(CPACK_PACKAGE_VENDOR "METISS Project IRISA")
SET(CPACK_CMAKE_GENERATOR ${CMAKE_GENERATOR})
SET(CPACK_PACKAGE_VERSION "0.5.4")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "5")
SET(CPACK_PACKAGE_VERSION_PATCH "4")
IF(LINUX)
IF(CMAKE_OSX_ARCHITECTURES MATCHES i386)
SET(CPACK_SYSTEM_NAME "MAC-PPC")
SET(CPACK_TOPLEVEL_TAG "MAC-PPC")
ELSE(CMAKE_OSX_ARCHITECTURES MATCHES i386)
SET(CPACK_SYSTEM_NAME "MAC")
SET(CPACK_TOPLEVEL_TAG "MAC")
ENDIF(CMAKE_OSX_ARCHITECTURES MATCHES i386)
ELSE(APPLE)
SET(CPACK_SYSTEM_NAME "Linux-i686")
SET(CPACK_TOPLEVEL_TAG "Linux-i686")
ENDIF(APPLE)
SET(CPACK_PACKAGE_FILE_NAME "MPTK-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-${CPACK_PACKAGE_VERSION}")
SET(CPACK_NSIS_DISPLAY_NAME "MPTK-${CPACK_PACKAGE_VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "MPTK-${CPACK_PACKAGE_VERSION}")
ELSE(LINUX)
IF(WIN32)
SET(CPACK_SYSTEM_NAME "WIN-32")
SET(CPACK_TOPLEVEL_TAG "WIN-32")
SET(CPACK_PACKAGE_FILE_NAME "MPTK-${CPACK_PACKAGE_VERSION}")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "MPTK-${CPACK_PACKAGE_VERSION}")
SET(CPACK_NSIS_DISPLAY_NAME "MPTK-${CPACK_PACKAGE_VERSION}-BIN-${CPACK_SYSTEM_NAME}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "MPTK")
ENDIF(WIN32)
ENDIF(LINUX)
SET(CPACK_OUTPUT_CONFIG_FILE "${MPTK_BINARY_DIR}/CPackConfig.cmake")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${MPTK_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Matching Pursuit Tool Kit")
SET(CPACK_RESOURCE_FILE_LICENSE "${MPTK_SOURCE_DIR}/COPYING")
SET(CPACK_RESOURCE_FILE_README "${MPTK_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_WELCOME "${MPTK_SOURCE_DIR}/INSTALL")


#BINARY DISTRIBUTION:

SET(CPACK_INSTALL_CMAKE_PROJECTS "${MPTK_BINARY_DIR};MPTK;ALL;/")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Matching Pursuit Tool Kit")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${MPTK-CPACK_PACKAGE_VERSION}")
SET(CPACK_IGNORE_FILES "/CMake/;/CMakeFiles/;/_CPack_Packages/;/src/;/bin/make_regression_constants;/install_manifest_/;.*~;/www/;/CVS//;/.svn/;.cdtprojects;.project;/.settings/")

IF(WIN32 AND NOT LINUX)
SET(CPACK_GENERATOR "TGZ")
  # NSIS not used yet, cause of a bug with suitable ZIP programme
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_DISPLAY_NAME "MPTK")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.irisa.fr/metiss/home_html-fr")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\http://www.irisa.fr/metiss/home_html-fr")
  SET(CPACK_NSIS_CONTACT "matchingpursuite@irisa.fr")
SET(CPACK_NSIS_MODIFY_PATH ON)
INSTALL(FILES "${MPTK_SOURCE_DIR}/README" DESTINATION /)
INSTALL(FILES  "${MPTK_SOURCE_DIR}/COPYING" DESTINATION /)
ELSE(WIN32 AND NOT LINUX)
SET(CPACK_GENERATOR "TGZ")
ENDIF(WIN32 AND NOT LINUX)
SET(CPACK_STRIP_FILES "")

# SOURCE DISTRIBUTION:
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "${MPTK_BINARY_DIR}/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "MPTK-Source-${CPACK_PACKAGE_VERSION}")
SET(CPACK_SOURCE_IGNORE_FILES "/www/;/CVS//;.*~;/.svn/;.cdtprojects;.project;/.settings/")
SET(CPACK_SOURCE_STRIP_FILES "")
INCLUDE(CPack)
# EXPERIMENTAL RPM BUILD
#IF(BUILD_RPM)
#INCLUDE(${MPTK_SOURCE_DIR}/CMake/UseRPMTools.cmake)
#IF(RPMTools_FOUND)
#  RPMTools_ADD_RPM_TARGETS(${PROJECT_NAME})
#ENDIF(RPMTools_FOUND)
#ENDIF(BUILD_RPM)

# TEST:
IF(BUILD_TESTING)
ENABLE_TESTING()
INCLUDE(CTest)
INCLUDE(CTestConfig.cmake)
SET(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage  ${CMAKE_CXX_FLAGS}") 
IF(BUILD_MULTITHREAD)
ADD_TEST(MPD, ${MPTK_BINARY_DIR}/bin/mpd_multithread -C ${MPTK_BINARY_DIR}/bin/path.xml -D ${MPTK_BINARY_DIR}/bin/reference/dictionary/dic_gabor_two_scales.xml -n 10 -q ${MPTK_BINARY_DIR}/bin/reference/signal/glockenspiel.wav ${MPTK_BINARY_DIR}/bin/reference/signal/test.bin)
ELSE(BUILD_MULTITHREAD)
ADD_TEST(MPD, ${MPTK_BINARY_DIR}/bin/mpd -C ${MPTK_BINARY_DIR}/bin/path.xml -D ${MPTK_BINARY_DIR}/bin/reference/dictionary/dic_gabor_two_scales.xml -n 10 -q ${MPTK_BINARY_DIR}/bin/reference/signal/glockenspiel.wav ${MPTK_BINARY_DIR}/bin/reference/signal/test.bin)
ENDIF(BUILD_MULTITHREAD)
ADD_TEST(MPR, ${MPTK_BINARY_DIR}/bin/mpr -C ${MPTK_BINARY_DIR}/bin/path.xml ${MPTK_BINARY_DIR}/bin/reference/signal/test.bin ${MPTK_BINARY_DIR}/bin/reference/signal/test.wav)
# Add other test
#ADD_TEST(Test1, ${MPTK_DIR}/bin/Testmptk)
ENDIF(BUILD_TESTING)
