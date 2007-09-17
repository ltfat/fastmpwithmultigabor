#------------------------------------------------
# Build lib_dsp_windows library
MACRO(GET_DSP_WINDOWS_CSOURCE out)
  SET(${out}
  ${CMAKE_SOURCE_DIR}/src/libdsp_windows/dsp_windows.h
  ${CMAKE_SOURCE_DIR}/src/libdsp_windows/dsp_windows.c
  )
ENDMACRO(GET_DSP_WINDOWS_CSOURCE)
GET_DSP_WINDOWS_CSOURCE(DSP_WINDOWS_CSOURCE)
ADD_CUSTOM_TARGET(dsp_windows-lib-source DEPENDS ${DSP_WINDOWS_CSOURCE})
SET_SOURCE_FILES_PROPERTIES(
  ${DSP_WINDOWS_CSOURCE}
  PROPERTIES GENERATED true
)
ADD_LIBRARY(dsp_windows STATIC ${DSP_WINDOWS_CSOURCE})
  SET_TARGET_PROPERTIES(dsp_windows PROPERTIES
    COMPILE_FLAGS "-DC_BUILDING_LIBDSP_WINDOWS ${STATIC_FLAGS}"
    OUTPUT_NAME "dsp_windows")
  ADD_DEPENDENCIES(dsp_windows dsp_windows-lib-source)
#------------------------------------------------
# Define install target:
#

IF(BUILD_SHARED_LIBS)
INSTALL(TARGETS
  dsp_windows
 LIBRARY DESTINATION ${LIB_HOME}
 ARCHIVE DESTINATION ${LIB_HOME}
)
ENDIF(BUILD_SHARED_LIBS)
