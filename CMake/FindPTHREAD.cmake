# Locate PTHREAD library and include paths

IF(UNIX)
  IF(APPLE)
    FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
      /usr/local/include
      ${MPTK_USER_INCLUDE_PATH}
       /usr/include
      )
    FIND_LIBRARY(PTHREAD_LIBRARY_FILE pthread
      /usr/local/lib
      /usr/lib
      $ENV{LD_LIBRARY_PATH}
      )
  ELSE(APPLE)
    FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
      /usr/local/include
      ${MPTK_USER_INCLUDE_PATH}
      /usr/include
      )
    FIND_LIBRARY(PTHREAD_LIBRARY_FILE pthread
      /usr/local/lib
       $ENV{LD_LIBRARY_PATH}
      /usr/lib
      )
  ENDIF(APPLE)
ELSE(UNIX)
  
  IF(WIN32)
    FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
    "${CMAKE_SOURCE_DIR}/extras/windows/include/"
      $ENV{INCLUDE}
      C:/
      )
    FIND_LIBRARY(PTHREAD_LIBRARY_FILE pthreadVC2
    "${CMAKE_SOURCE_DIR}/extras/windows/lib/"
      $ENV{LIB}
      C:/
      )
  ENDIF(WIN32)
ENDIF(UNIX)

SET (PTHREAD_FOUND 0)
IF(PTHREAD_INCLUDE_DIR)
  SET(PTHREAD_FOUND 1 )
   SET(HAVE_PTHREAD_H 1)
  MESSAGE(STATUS "pthread library found !!")
ELSE(PTHREAD_INCLUDE_DIR)
  MESSAGE(STATUS "pthread library not found !!")
ENDIF(PTHREAD_INCLUDE_DIR)
