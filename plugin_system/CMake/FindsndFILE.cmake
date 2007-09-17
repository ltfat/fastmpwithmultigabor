# Locate sndFile library and include paths

IF(UNIX)
  IF(APPLE)
    FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.h
      /usr/local/include
       ${MPTK_USER_INCLUDE_PATH}
      /usr/include/ 
      )
     FIND_LIBRARY(SNDFILE_LIBRARY_FILE sndfile
       /usr/local/lib
      $ENV{LD_LIBRARY_PATH}
      ${MPTK_USER_LIB_PATH}
      /usr/lib
      )
  ELSE(APPLE)
    FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.h
      /usr/local/include
       ${MPTK_USER_INCLUDE_PATH}
      /usr/include/ 
      )

    FIND_LIBRARY(SNDFILE_LIBRARY_FILE sndfile
      /usr/local/lib
      $ENV{LD_LIBRARY_PATH}
      ${MPTK_USER_LIB_PATH}
      /usr/lib
      )
  ENDIF(APPLE)
ELSE(UNIX)
  
  IF(WIN32)
    FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.h
    $ENV{INCLUDE}
      )
    FIND_LIBRARY(SNDFILE_LIBRARY_FILE libsndfile-1
    $ENV{SystemDrive}/WINDOWS/system32
    $ENV{LIB}
      )
  ENDIF(WIN32)
ENDIF(UNIX)

SET (SNDFILE_INCLUDE_FOUND 0)
IF(SNDFILE_INCLUDE_DIR)
  SET(SNDFILE_INCLUDE_FOUND 1 )
  SET(HAVE_SNDFILE 1)
  MESSAGE(STATUS "SndFile.h found !!")
ELSE(SNDFILE_INCLUDE_DIR)
  MESSAGE(STATUS "SndFile.h not found !!")
ENDIF(SNDFILE_INCLUDE_DIR)

SET (SNDFILE_LIB_FOUND 0)
IF(SNDFILE_LIBRARY_FILE)
  SET(SNDFILE_LIB_FOUND 1 )
  MESSAGE(STATUS "SndFile library found !!")
ELSE(SNDFILE_LIBRARY_FILE)
  MESSAGE(STATUS "SndFile library not found !!")
ENDIF(SNDFILE_LIBRARY_FILE)
