#------------------------------------------------
# Generating regression_constants.h using make_regression_constants.cpp
#------------------------------------------------
ADD_EXECUTABLE(make_regression_constants ${MPTK_SOURCE_DIR}/src/libmptk/make_regression_constants.cpp)
GET_TARGET_PROPERTY(MY_GENERATOR_EXE make_regression_constants LOCATION)
#Add the custom command that will generate the file
ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/libmptk/regression_constants.h
   COMMAND ${MY_GENERATOR_EXE} ${EXECUTABLE_OUTPUT_PATH} 
   DEPENDS make_regression_constants
)
SET_SOURCE_FILES_PROPERTIES(${MPTK_BINARY_DIR}/src/libmptk/regression_constants.h GENERATED)
