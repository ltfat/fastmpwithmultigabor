# Locate Doxygen and dot:
IF(UNIX)
  IF(APPLE)
    # - this module looks for Doxygen and the path to Graphiz's dot
# With the OS X GUI version, it likes to be installed to /Applications and
# it contains the doxygen executable in the bundle. In the versions I've 
# seen, it is located in Resources, but in general, more often binaries are 
# located in MacOS.
FIND_PROGRAM(DOXYGEN
  doxygen
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\doxygen_is1;Inno Setup: App Path]/bin"
  /Applications/Doxygen.app/Contents/Resources
  /Applications/Doxygen.app/Contents/MacOS
)

# In the older versions of OS X Doxygen, dot was included with the 
# Doxygen bundle. But the new versions place make you download Graphviz.app
# which contains dot in its bundle.
FIND_PROGRAM(DOT
  dot
  "$ENV{ProgramFiles}/ATT/Graphviz/bin"
  "C:/Program Files/ATT/Graphviz/bin"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin
  /Applications/Graphviz.app/Contents/MacOS
  /Applications/Doxygen.app/Contents/Resources
  /Applications/Doxygen.app/Contents/MacOS
)

# The Doxyfile wants the path to Dot, not the entire path and executable
# so for convenience, I'll add another search for DOT_PATH.
FIND_PATH(DOT_PATH
  dot
  "C:/Program Files/ATT/Graphviz/bin"
  [HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin
  /Applications/Graphviz.app/Contents/MacOS
  /Applications/Doxygen.app/Contents/Resources
  /Applications/Doxygen.app/Contents/MacOS
)

MARK_AS_ADVANCED(
  DOT
  DOT_PATH
  DOXYGEN
)
  ELSE(APPLE)
    FIND_PROGRAM(DOXYGEN doxygen
      /usr/local/bin
      /usr/bin
$ENV{HOME}      
${MPTK_USER_LOCAL_PATH})

    IF(DOXYGEN)
      SET(DOXYGEN_FOUND TRUE)
   ENDIF(DOXYGEN)
    FIND_PROGRAM(DOT dot
      /usr/local/bin
      /usr/bin
$ENV{HOME}
      ${MPTK_USER_LOCAL_PATH})
IF(DOT)
      SET(DOT_FOUND TRUE)
   ENDIF(DOT)
FIND_PATH(DOT_PATH dot 
      /usr/local/bin
      /usr/bin
$ENV{HOME}
      ${MPTK_USER_LOCAL_PATH}
      )
IF(DOT_PATH)
      SET(DOT_PATH_FOUND TRUE)
   ENDIF(DOT_PATH)
MARK_AS_ADVANCED(
  DOT
  DOT_PATH
  DOXYGEN
)

  ENDIF(APPLE)
ELSE(UNIX)
  
  IF(WIN32)
  
FIND_PROGRAM(DOXYGEN
  doxygen
  $ENV{DOXYGEN_HOME}
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\doxygen_is1;Inno Setup: App Path]/bin"
  "$ENV{ProgramFiles}/doxygen/bin"
  "$ENV{ProgramFiles}/doxygen/bin"
)

# In the older versions of OS X Doxygen, dot was included with the 
# Doxygen bundle. But the new versions place make you download Graphviz.app
# which contains dot in its bundle.
FIND_PROGRAM(DOT
  dot
  $ENV{DOT_HOME}
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin"
  "$ENV{ProgramFiles}/ATT/Graphviz/bin"
  "$ENV{ProgramFiles}/Graphviz/bin"
)

# The Doxyfile wants the path to Dot, not the entire path and executable
# so for convenience, I'll add another search for DOT_PATH.
FIND_PATH(DOT_PATH
  dot
  $ENV{DOT_HOME}
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\ATT\\Graphviz;InstallPath]/bin"
  "$ENV{ProgramFiles}/ATT/Graphviz/bin"
  "$ENV{ProgramFiles}/Graphviz/bin"
)

  ENDIF(WIN32)
ENDIF(UNIX)

IF(DOXYGEN_FOUND)
  MESSAGE(STATUS "Doxygen found !!")
ELSE(DOXYGEN_FOUND)
  MESSAGE(STATUS "Doxygen not found !!")
ENDIF(DOXYGEN_FOUND)
IF(DOT_FOUND)
  MESSAGE(STATUS "Dot found !!")
ELSE(DOT_FOUND)
  MESSAGE(STATUS "Dot not found !!")
ENDIF(DOT_FOUND)


