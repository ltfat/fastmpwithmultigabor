#------------------------------------------------
# Build getopt library
#------------------------------------------------
# Macro in order to get the hole bunch of files necessary
SET(GETOPT_LIB_CPPSOURCE getopt.c getopt1.c getopt.h)
ADD_LIBRARY(getopt STATIC ${GETOPT_LIB_CPPSOURCE})
SET_TARGET_PROPERTIES(getopt PROPERTIES COMPILE_FLAGS "${EXECUTABLE_C_COMPILE_FLAGS}" OUTPUT_NAME "libgetopt" )

	
#------------------------------------------------
# Define install target:
#------------------------------------------------
IF(BUILD_SHARED_LIBS)
	INSTALL(TARGETS getopt 
	   LIBRARY DESTINATION lib
	   ARCHIVE DESTINATION lib)
ENDIF(BUILD_SHARED_LIBS)
