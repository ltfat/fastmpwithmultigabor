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
SET(CMAKE_INSTALL_PREFIX /usr)
IF(NOT UNIX)
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
ENDIF(NOT UNIX)

#Select the ADD_DEFINITIONS according to selected options
IF(BUILD_MULTITHREAD)
IF (BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe -DNDEBUG -lpthread -v)
ELSE(BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe -DNDEBUG -lpthread)
ENDIF(BUILD_VERBOSE)
ELSE(BUILD_MULTITHREAD)
IF (BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe -DNDEBUG  -v)
ELSE(BUILD_VERBOSE)
ADD_DEFINITIONS(-g -O3 -Wall -pipe -DNDEBUG )
ENDIF(BUILD_VERBOSE)
ENDIF(BUILD_MULTITHREAD)
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
ENDIF(UNIX)

#Set  BUILDVERSION, BUILDVERSION, BUILDVERSION and AUTHORS according to text files in /version
FILE(READ ${MPTK_SOURCE_DIR}/version/buildversion.cmake BUILDVERSION)
FILE(READ ${MPTK_SOURCE_DIR}/version/packagename.cmake PACKAGENAME)
FILE(READ ${MPTK_SOURCE_DIR}/version/packagenamefull.cmake PACKAGENAMEFULL)
FILE(READ ${MPTK_SOURCE_DIR}/version/AUTHORS AUTHORS)

#Define the target for install
SET(BIN_HOME     ${CMAKE_INSTALL_PREFIX}/bin)
SET(INCLUDE_HOME ${CMAKE_INSTALL_PREFIX}/include)
SET(LIB_HOME     ${CMAKE_INSTALL_PREFIX}/lib)



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
)
SET(LIBDSP_INCLUDE_DIR  ${MPTK_SOURCE_DIR}/src/libdsp_windows)
SET(UTILS_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/utils )
SET(GUI_SOURCE_DIR ${MPTK_SOURCE_DIR}/src/gui )

# Include file to find libraries
INCLUDE( ${MPTK_SOURCE_DIR}/CMake/FindExternalLibs.cmake )
#-----------------------------------------------------------------------------

#Set option for building and linking:
#Set directories for linker
LINK_DIRECTORIES( ${MPTK_SOURCE_DIR}/libmptk ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE} ${wxWidgets_LIBRARY_DIRS} )
#set the include directories paths for building
INCLUDE_DIRECTORIES( /usr/include ${MPTK_SOURCE_DIR} ${MPTK_BINARY_DIR} ${PTHREAD_INCLUDE_DIR} ${LIBMPTK_INCLUDE_DIR}  ${PORTAUDIO_INCLUDE_DIR} ${MPTK_BINARY_DIR}/src/libmptk/ ${LIBDSP_INCLUDE_DIR} ${FFTW3_INCLUDE_DIR} ${SNDFILE_INCLUDE_DIR} ${wxWidgets_INCLUDE_DIRS} )

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

# setting dart config
#####################
#IF(BUILD_TESTING)
  # profile flag options
#  SET(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage  ${CMAKE_CXX_FLAGS}") 
 # ENABLE_TESTING()
  #INCLUDE(${VISAGES_SOURCE_DIR}/DartConfig.cmake)
 # INCLUDE(Dart)
#ENDIF(BUILD_TESTING)

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
SUBDIRS (src)














#Tests
#ENABLE_TESTING()
#ADD_TEST(Test1, ${MPTK_DIR}/bin/TEST)
#ADD_TEST(Test1, ${MPTK_DIR}/bin/Testmptk)
