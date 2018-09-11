#------------------------------------------------
# Build libbase64 library
#------------------------------------------------
# Macro in order to get the hole bunch of files necessary
SET(BASE64_LIB_CPPSOURCE base64.cpp base64.h)
ADD_LIBRARY(base64 STATIC ${BASE64_LIB_CPPSOURCE})
SET_TARGET_PROPERTIES(base64 PROPERTIES COMPILE_FLAGS "${EXECUTABLE_C_COMPILE_FLAGS}" OUTPUT_NAME "libbase64" )

	
#------------------------------------------------
# Define install target:
#------------------------------------------------
IF(BUILD_SHARED_LIBS)
	INSTALL(TARGETS base64 
	   LIBRARY DESTINATION lib
	   ARCHIVE DESTINATION lib)
ENDIF(BUILD_SHARED_LIBS)
