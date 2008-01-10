# Build python files
IF(BUILD_PYTHON_FILES)
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
 ADD_DEFINITIONS(-fPIC)
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
INCLUDE(${CMAKE_ROOT}/Modules/FindPythonInterp.cmake)
GET_TARGET_PROPERTY(LIB_MPTK_OUTPUT_NAME mptk OUTPUT_NAME)
IF(PYTHON_EXECUTABLE)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/python/pyMPTK.cpp ${MPTK_BINARY_DIR}/src/python/pyMPTK.cpp COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/python/setup.py ${MPTK_BINARY_DIR}/src/python/setup.py @ONLY)  
      ADD_CUSTOM_TARGET(python_file ALL echo
      DEPENDS   ${MPTK_BINARY_DIR}/src/python/pyMPTK.cpp
    )
  ADD_CUSTOM_COMMAND( 
    TARGET    python_file 
    COMMAND   ${PYTHON_EXECUTABLE} ${MPTK_BINARY_DIR}/src/python/setup.py build
    COMMENT   "python"
  )
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/python/CMakeInstallPython.in.cmake ${MPTK_BINARY_DIR}/src/python/CMakeInstallPython.cmake @ONLY)  
INSTALL(SCRIPT ${MPTK_BINARY_DIR}/src/python/CMakeInstallPython.cmake)
ELSE(PYTHON_EXECUTABLE)
  MESSAGE(FATAL_ERROR "You requested building for python, but python was not found")
ENDIF(PYTHON_EXECUTABLE)
ENDIF(BUILD_PYTHON_FILES)

