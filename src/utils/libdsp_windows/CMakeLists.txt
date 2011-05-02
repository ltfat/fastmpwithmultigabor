#------------------------------------------------
# Build libdsp_windows library
#------------------------------------------------
# Macro in order to get the hole bunch of files necessary
SET(DSP_WINDOWS_LIB_CPPSOURCE dsp_windows.c dsp_windows.h)
ADD_LIBRARY(dsp_windows STATIC ${DSP_WINDOWS_LIB_CPPSOURCE})
SET_TARGET_PROPERTIES(dsp_windows PROPERTIES COMPILE_FLAGS "${EXECUTABLE_C_COMPILE_FLAGS}" OUTPUT_NAME "libdsp_windows" )

	
#------------------------------------------------
# Define install target:
#------------------------------------------------
IF(BUILD_SHARED_LIBS)
	INSTALL(TARGETS dsp_windows 
	   LIBRARY DESTINATION lib
	   ARCHIVE DESTINATION lib)
ENDIF(BUILD_SHARED_LIBS)
