#------------------------------------------------
# Build lib_dsp_windows library
#------------------------------------------------
# Macro in order to get the hole bunch of files necessary
MACRO(GET_DSP_WINDOWS_CSOURCE out)
	SET(${out}
	${CMAKE_SOURCE_DIR}/src/libdsp_windows/dsp_windows.h
	${CMAKE_SOURCE_DIR}/src/libdsp_windows/dsp_windows.c)
ENDMACRO(GET_DSP_WINDOWS_CSOURCE)

#Begining : Call the macro
GET_DSP_WINDOWS_CSOURCE(DSP_WINDOWS_CSOURCE)

ADD_CUSTOM_TARGET(dsp_windows-lib-source DEPENDS ${DSP_WINDOWS_CSOURCE})
SET_SOURCE_FILES_PROPERTIES(${DSP_WINDOWS_CSOURCE} PROPERTIES GENERATED true)
ADD_LIBRARY(dsp_windows STATIC ${DSP_WINDOWS_CSOURCE})
#Compile using the Flags previously defined
SET_TARGET_PROPERTIES(dsp_windows PROPERTIES COMPILE_FLAGS "-DC_BUILDING_LIBDSP_WINDOWS ${CMAKE_C_FLAGS}" OUTPUT_NAME "dsp_windows")

# Add the dependencies
ADD_DEPENDENCIES(dsp_windows dsp_windows-lib-source)

#------------------------------------------------
# Define install target:
#------------------------------------------------
IF(BUILD_SHARED_LIBS)
	INSTALL(TARGETS dsp_windows 
	   LIBRARY DESTINATION lib
	   ARCHIVE DESTINATION lib)
ENDIF(BUILD_SHARED_LIBS)
