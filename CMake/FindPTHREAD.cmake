# Locate PTHREAD library and include paths

IF(UNIX)
  IF(APPLE)
    FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
      /usr/include
      /usr/local/include
      /sw/include
      /opt/local/include
      )
    FIND_LIBRARY(PTHREAD_LIBRARY_FILE pthread
      /usr/lib
      /usr/local/lib
      /sw/lib
      /opt/local/lib
      )
  ELSE(APPLE)
    FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h
      /usr/include
      /usr/local/include
      )
    FIND_LIBRARY(PTHREAD_LIBRARY_FILE pthread
      /usr/lib
      /usr/local/lib
      )
  ENDIF(APPLE)
ELSE(UNIX)
	IF(WIN32)
		IF(CMAKE_CL_64)
			SET(RELATIVE_INCLUDE_PATH extras/windows/win64/include)
			SET(RELATIVE_LIBRARY_PATH extras/windows/win64/lib)
			FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h ${MPTK_SOURCE_DIR}/${RELATIVE_INCLUDE_PATH} NO_DEFAULT_PATH)
			FIND_LIBRARY(PTHREAD_LIBRARY_FILE pthreadVC2 ${MPTK_SOURCE_DIR}/${RELATIVE_LIBRARY_PATH} NO_DEFAULT_PATH)
		ELSE(CMAKE_CL_64)
			SET(RELATIVE_INCLUDE_PATH extras/windows/win32/include)
			SET(RELATIVE_LIBRARY_PATH extras/windows/win32/lib)
			FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h ${MPTK_SOURCE_DIR}/${RELATIVE_INCLUDE_PATH} NO_DEFAULT_PATH)
			FIND_LIBRARY(PTHREAD_LIBRARY_FILE pthreadVC2 ${MPTK_SOURCE_DIR}/${RELATIVE_LIBRARY_PATH} NO_DEFAULT_PATH)
		ENDIF(CMAKE_CL_64)
	ENDIF(WIN32)
ENDIF(UNIX)

GET_FILENAME_COMPONENT(PTHREAD_LIBRARY_PATH ${PTHREAD_LIBRARY_FILE} PATH CACHE)

SET (PTHREAD_FOUND 0)
IF(PTHREAD_INCLUDE_DIR)
  SET(PTHREAD_FOUND 1 )
   SET(HAVE_PTHREAD_H 1)
  MESSAGE(STATUS "pthread library found !!")
ELSE(PTHREAD_INCLUDE_DIR)
  MESSAGE(STATUS "pthread library not found !!")
ENDIF(PTHREAD_INCLUDE_DIR)
