
# Include src folder to compiling proccess

# Go to compile mptk libraries
SUBDIRS (libmptk)

# Go to compile libdsp_windows
SUBDIRS (libdsp_windows)

# Go to compile mptk utils
SUBDIRS (utils)

# Go to compile GUI
IF(BUILD_GUI)
  SUBDIRS (gui)
ENDIF(BUILD_GUI)

