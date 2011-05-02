#------------------------------------------------
# Build libmd5sum library
#------------------------------------------------
# Macro in order to get the hole bunch of files necessary
SET(MD5SUM_LIB_CPPSOURCE md5sum.cpp md5sum.h)
ADD_LIBRARY(md5sum STATIC ${MD5SUM_LIB_CPPSOURCE})
SET_TARGET_PROPERTIES(md5sum PROPERTIES COMPILE_FLAGS "${EXECUTABLE_C_COMPILE_FLAGS}" OUTPUT_NAME "libmd5sum" )

	
#------------------------------------------------
# Define install target:
#------------------------------------------------
IF(BUILD_SHARED_LIBS)
	INSTALL(TARGETS md5sum
	   LIBRARY DESTINATION lib
	   ARCHIVE DESTINATION lib)
ENDIF(BUILD_SHARED_LIBS)
