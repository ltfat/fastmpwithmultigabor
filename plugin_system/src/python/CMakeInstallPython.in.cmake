# Convert the prefix to a windows path if necessary.  The python
# distutils implementation seems sensitive to the slash direction.
IF(WIN32)
  IF(NOT CYGWIN)
    STRING(REGEX REPLACE "/" "\\\\" CMAKE_INSTALL_PREFIX
      "${CMAKE_INSTALL_PREFIX}")
  ENDIF(NOT CYGWIN)
ENDIF(WIN32)

# Run python on setup.py to install the python modules.
EXEC_PROGRAM( "@PYTHON_EXECUTABLE@" "@MPTK_BINARY_DIR@/src/python/" ARGS
  "setup.py" "install" )

