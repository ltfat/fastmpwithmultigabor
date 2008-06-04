# Include src folder to compiling proccess

# Go to compile tinyxml library
ADD_SUBDIRECTORY (tinyxml)

# Go to compile mptk libraries
ADD_SUBDIRECTORY (libmptk)

# Go to compile libdsp_windows
ADD_SUBDIRECTORY (libdsp_windows)

# Go to compile mptk utils
#ADD_SUBDIRECTORY (utils)

# Go to compile matlab API in MEX Files
ADD_SUBDIRECTORY (matlab)

# Go to compile python Files
ADD_SUBDIRECTORY (python)

# Go to compile mptk plugin
ADD_SUBDIRECTORY (plugin)

# Go to compile GUI
IF(BUILD_GUI)
 ADD_SUBDIRECTORY (gui) 
ENDIF(BUILD_GUI)

IF(BUILD_TESTING)
ADD_SUBDIRECTORY (tests) 
ENDIF(BUILD_TESTING)
