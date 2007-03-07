#------------------------------------------------
# Generating file using flex:anywave_table_scanner.cpp
#
IF(FLEX_FOUND)
IF(UNIX)
IF(APPLE)
ADD_CUSTOM_COMMAND(OUTPUT ${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp
COMMAND /bin/sh  ${MPTK_SOURCE_DIR}/CMake/ylwrap ${MPTK_SOURCE_DIR}/src/libmptk/anywave_table_scanner.lpp lex.yy.c ${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp -- flex 
DEPENDS ${MPTK_SOURCE_DIR}/src/libmptk/anywave_table_scanner.lpp)
SET_SOURCE_FILES_PROPERTIES(${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp GENERATED)
ELSE(APPLE)
ADD_CUSTOM_COMMAND(OUTPUT ${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp
COMMAND /bin/sh  ${MPTK_SOURCE_DIR}/CMake/ylwrap ${MPTK_SOURCE_DIR}/src/libmptk/anywave_table_scanner.lpp lex.yy.c ${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp -- flex 
DEPENDS ${MPTK_SOURCE_DIR}/src/libmptk/anywave_table_scanner.lpp)
SET_SOURCE_FILES_PROPERTIES(${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp GENERATED)
ENDIF(APPLE)
ENDIF(UNIX)
ELSE(FLEX_FOUND)
IF(UNIX)
IF(APPLE)
MESSAGE("Flex not found:anywave_table_scanner.cpp will not be generated, need flex")
ELSE(APPLE)
MESSAGE("Flex not found:anywave_table_scanner.cpp will not be generated, need flex")
ENDIF(APPLE)
ELSE(UNIX)
IF(WIN32)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/extras/windows/src/anywave_table_scanner.cpp ${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp)
ENDIF(WIN32)
ENDIF(UNIX)
ENDIF(FLEX_FOUND)
#------------------------------------------------
# Generating file using flex:dict_scanner.cpp
# 
IF(FLEX_FOUND)
IF(UNIX)
IF(APPLE)
ADD_CUSTOM_COMMAND(OUTPUT ${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp
COMMAND /bin/sh  ${MPTK_SOURCE_DIR}/CMake/ylwrap ${MPTK_SOURCE_DIR}/src/libmptk/atom_classes/dict_scanner.lpp lex.yy.c ${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp -- flex 
DEPENDS ${MPTK_SOURCE_DIR}/src/libmptk/atom_classes/dict_scanner.lpp)
SET_SOURCE_FILES_PROPERTIES(${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp GENERATED)
ELSE(APPLE)
ADD_CUSTOM_COMMAND(OUTPUT ${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp
COMMAND /bin/sh  ${MPTK_SOURCE_DIR}/CMake/ylwrap ${MPTK_SOURCE_DIR}/src/libmptk/atom_classes/dict_scanner.lpp lex.yy.c ${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp -- flex 
DEPENDS ${MPTK_SOURCE_DIR}/src/libmptk/atom_classes/dict_scanner.lpp)
SET_SOURCE_FILES_PROPERTIES(${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp GENERATED)
ENDIF(APPLE)
ENDIF(UNIX)
ELSE(FLEX_FOUND)
IF(UNIX)
IF(APPLE)
MESSAGE("Flex not found:anywave_table_scanner.cpp will not be generated, need flex")
ELSE(APPLE)
MESSAGE("Flex not found:anywave_table_scanner.cpp will not be generated, need flex")
ENDIF(APPLE)
ELSE(UNIX)
IF(WIN32)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/extras/windows/src/dict_scanner.cpp ${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp)
ENDIF(WIN32)
ENDIF(UNIX)
ENDIF(FLEX_FOUND)
#------------------------------------------------
# Generating regression_constants.h using make_regression_constants.cpp
#
ADD_EXECUTABLE(make_regression_constants make_regression_constants.cpp)
GET_TARGET_PROPERTY(MY_GENERATOR_EXE make_regression_constants LOCATION)
#Add the custom command that will generate the file
ADD_CUSTOM_COMMAND(
   OUTPUT ${MPTK_BINARY_DIR}/src/libmptk/regression_constants.h
   COMMAND ${MY_GENERATOR_EXE} ${EXECUTABLE_OUTPUT_PATH} 
   DEPENDS make_regression_constants
)

#------------------------------------------------
#Set the source file properties as generated to include it
SET_SOURCE_FILES_PROPERTIES(${MPTK_BINARY_DIR}/src/libmptk/regression_constants.h GENERATED)
#
#------------------------------------------------
#Generate the mptk.h used to build:
INCLUDE(${MPTK_SOURCE_DIR}/CMake/CMakeGenerateMPTKHeader.cmake)
#
#------------------------------------------------
#Set the list of header files
SET(HEADER_FILE 
${CMAKE_SOURCE_DIR}/src/libmptk/mp_types.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_messaging.h      
	${CMAKE_SOURCE_DIR}/src/libmptk/win_server.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/fft_interface.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/general.h           
	${CMAKE_SOURCE_DIR}/src/libmptk/mtrand.h            
	${CMAKE_SOURCE_DIR}/src/libmptk/regression.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_signal.h         
	${CMAKE_SOURCE_DIR}/src/libmptk/tfmap.h             
	${CMAKE_SOURCE_DIR}/src/libmptk/atom.h  ${CMAKE_SOURCE_DIR}/src/libmptk/block.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/mask.h              
	${CMAKE_SOURCE_DIR}/src/libmptk/book.h ${CMAKE_SOURCE_DIR}/src/libmptk/dict.h       
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_table.h ${CMAKE_SOURCE_DIR}/src/libmptk/anywave_server.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_table_io_interface.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/convolution.h                    
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/dirac_atom.h 
    ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/dirac_block.h    
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/gabor_atom.h   
    ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/gabor_block.h    
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/harmonic_atom.h  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/harmonic_block.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/chirp_block.h    
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_atom.h         ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_block.h         
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_hilbert_atom.h ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_hilbert_block.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/constant_atom.h        ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/constant_block.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/nyquist_atom.h         ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/nyquist_block.h         
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mclt_abstract_block.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdct_block.h  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdct_atom.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mclt_block.h  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mclt_atom.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdst_block.h  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdst_atom.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/block_io_interface.h  
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/atom_classes.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_core.h
	${CMAKE_SOURCE_DIR}/src/libmptk/mptk_env.h
	)
#
#------------------------------------------------
#Find or generate the MPTK shared header (the one for end-user)
 FIND_FILE(MPTK_SHARED_HEADER mptk.h
             PATHS ${MPTK_BINARY_DIR}/include/
             NO_CMAKE_ENVIRONMENT_PATH
             NO_CMAKE_PATH
             NO_SYSTEM_ENVIRONMENT_PATH
             NO_CMAKE_SYSTEM_PATH)
IF(BUILD_SHARED_LIBS)
IF(NOT MPTK_SHARED_HEADER)
FILE(READ ${MPTK_SOURCE_DIR}/src/libmptk/mptk.h.shared.cmake INIT)
FILE(WRITE ${MPTK_BINARY_DIR}/include/mptk.h "${INIT}")
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h "/* * * * * * * * * * * * * * * * * * * * * * *\n")
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h "${AUTHORS}")
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h "* * * * * * * * * * * * * * * * * * * * * * */\n")
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h "#ifndef __mptk_h_\n")
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h "#define __mptk_h_\n")
FOREACH(HEADER ${HEADER_FILE})
FILE(READ ${HEADER} BUFFER)
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h  "${BUFFER}" )
ENDFOREACH(HEADER)
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h "#endif /* __mptk_h_ */\n" )
SET(MPTK_SHARED_HEADER TRUE INTERNAL)
ENDIF(NOT MPTK_SHARED_HEADER)
ENDIF(BUILD_SHARED_LIBS)
#Place this header file in the include directory of the build directory
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/mptk.h.cmake ${MPTK_BINARY_DIR}/src/libmptk/mptk.h)
# 

#------------------------------------------------
# Set macro to find the sources of MPTK lib:
#
MACRO(GET_MPTK_LIB_CPPSOURCE out)
  SET(${out}
    ${CMAKE_SOURCE_DIR}/src/libmptk/mp_pthreads_barrier.h
	${CMAKE_SOURCE_DIR}/src/libmptk/header.h 
	${MPTK_BINARY_DIR}/src/libmptk/mptk.h
	${MPTK_BINARY_DIR}/src/libmptk/regression_constants.h
	${HEADER_FILE}
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_messaging.cpp  
	${CMAKE_SOURCE_DIR}/src/libmptk/general.cpp       
	${CMAKE_SOURCE_DIR}/src/libmptk/mtrand.c          
	${CMAKE_SOURCE_DIR}/src/libmptk/regression.cpp    
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_signal.cpp     
	${CMAKE_SOURCE_DIR}/src/libmptk/win_server.cpp    
	${CMAKE_SOURCE_DIR}/src/libmptk/fft_interface.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/block.cpp 
	${MPTK_BINARY_DIR}/src/libmptk/atom.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/atom_classes.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_table.cpp 
    ${CMAKE_SOURCE_DIR}/src/libmptk/anywave_server.cpp             
	${MPTK_BINARY_DIR}/src/libmptk/anywave_table_scanner.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_table_io_interface.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/convolution.cpp   
    ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/dirac_atom.cpp     ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/dirac_block.cpp    
    ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/gabor_atom.cpp     ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/gabor_block.cpp    
    ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/harmonic_atom.cpp  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/harmonic_block.cpp 
    ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/chirp_block.cpp    
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_atom.cpp         ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_block.cpp         
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_hilbert_atom.cpp ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/anywave_hilbert_block.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/constant_atom.cpp        ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/constant_block.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/nyquist_atom.cpp         ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/base/nyquist_block.cpp  
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mclt_abstract_block.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdct_block.cpp  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdct_atom.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mclt_block.cpp  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mclt_atom.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdst_block.cpp  ${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/contrib/lam/mdst_atom.cpp 
	${MPTK_BINARY_DIR}/src/libmptk/dict_scanner.cpp        
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_classes/block_io_interface.cpp  
	${MPTK_BINARY_DIR}/src/libmptk/dict.cpp ${CMAKE_SOURCE_DIR}/src/libmptk/mask.cpp ${CMAKE_SOURCE_DIR}/src/libmptk/book.cpp ${CMAKE_SOURCE_DIR}/src/libmptk/tfmap.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_core.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mptk_env.cpp
  )
ENDMACRO(GET_MPTK_LIB_CPPSOURCE)

#------------------------------------------------
# Using the macro to find the sources of MPTK lib:
#
GET_MPTK_LIB_CPPSOURCE(MPTK_LIB_CPPSOURCE)
#
#------------------------------------------------
# Creating target to build the mptk lib with mptk source files and set dependencies of this files
ADD_CUSTOM_TARGET(mptk-lib-source DEPENDS ${MPTK_LIB_CPPSOURCE})
ADD_DEPENDENCIES(mptk-lib-source dict_scanner)
ADD_DEPENDENCIES(mptk-lib-source anywave_table_scanner)
IF(GENERATE_FFTW_WISDOM)
ADD_DEPENDENCIES(mptk-lib-source fftw_plan)
ENDIF(GENERATE_FFTW_WISDOM)
#Set the source files properties:
SET_SOURCE_FILES_PROPERTIES(
  ${MPTK_LIB_CPPSOURCE}
  PROPERTIES GENERATED true
)
#
#------------------------------------------------
#Create the library in static format:
ADD_LIBRARY(mptk STATIC ${MPTK_LIB_CPPSOURCE})
  SET_TARGET_PROPERTIES(mptk PROPERTIES
    COMPILE_FLAGS " -DC_BUILDING_LIBMPTK ${STATIC_FLAGS} -lpthread"
    OUTPUT_NAME "mptk")
  ADD_DEPENDENCIES(mptk mptk-lib-source)
#
#------------------------------------------------  
#Build the executable if requested by the end-user
IF(BUILD_EXECUTABLE)
INCLUDE( ${MPTK_SOURCE_DIR}/CMake/buildEXECUTABLE.cmake )
ENDIF(BUILD_EXECUTABLE)
#
#------------------------------------------------
# Define install target:
#For mptk library:
IF(BUILD_SHARED_LIBS)
INSTALL(TARGETS
  mptk
 LIBRARY DESTINATION ${LIB_HOME}
 ARCHIVE DESTINATION ${LIB_HOME}
)
#for mptk include file:
INSTALL(FILES ${MPTK_BINARY_DIR}/include/mptk.h DESTINATION ${INCLUDE_HOME})
ENDIF(BUILD_SHARED_LIBS)
#
