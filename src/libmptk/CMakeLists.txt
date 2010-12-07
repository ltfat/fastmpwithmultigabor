   if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 OLD)
    endif(COMMAND cmake_policy)
                 
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
#Set the list of header files
SET(HEADER_FILE 
    ${CMAKE_SOURCE_DIR}/src/libmptk/mp_types.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_messaging.h   
    ${CMAKE_SOURCE_DIR}/src/libmptk/dll.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/win_server.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/fft_interface.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/dct_interface.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/general.h           
	${CMAKE_SOURCE_DIR}/src/libmptk/mtrand.h            
	${CMAKE_SOURCE_DIR}/src/libmptk/regression.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_signal.h         
	${CMAKE_SOURCE_DIR}/src/libmptk/tfmap.h             
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_param.h
	${CMAKE_SOURCE_DIR}/src/libmptk/freq_atom_param.h
	${CMAKE_SOURCE_DIR}/src/libmptk/atom.h
	${CMAKE_SOURCE_DIR}/src/libmptk/block.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/mask.h              
	${CMAKE_SOURCE_DIR}/src/libmptk/book.h 
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_book.h
	${CMAKE_SOURCE_DIR}/src/libmptk/dict.h       
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_table.h
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_server.h    
	${CMAKE_SOURCE_DIR}/src/libmptk/convolution.h  
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_factory.h  
	${CMAKE_SOURCE_DIR}/src/libmptk/block_factory.h  
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_pthreads_barrier.h
	${CMAKE_SOURCE_DIR}/src/libmptk/mixer.h
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_core.h
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_core.h
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_demix_core.h
	${CMAKE_SOURCE_DIR}/src/libmptk/mptk_env.h
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_param_book.h
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_pos_book.h
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_block_book.h
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_pos_range_sub_book.h	
	${CMAKE_SOURCE_DIR}/src/libmptk/double_index_book.h	
	${CMAKE_SOURCE_DIR}/src/libmptk/gpd_core.h	
	)
#
#------------------------------------------------
#Find or generate the MPTK shared header (the one for end-user)
IF(NOT EXISTS ${MPTK_BINARY_DIR}/install/include/mptk.h)
  SET(MPTK_SHARED_HEADER ${MPTK_BINARY_DIR}/install/include/mptk.h)
  IF(BUILD_SHARED_LIBS)
    FILE(READ ${MPTK_SOURCE_DIR}/src/libmptk/mptk.h.shared.cmake INIT)
    FILE(WRITE ${MPTK_BINARY_DIR}/install/include/mptk.h "${INIT}")
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h "/* * * * * * * * * * * * * * * * * * * * * * *\n")
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h "${AUTHORS}")
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h "* * * * * * * * * * * * * * * * * * * * * * */\n")
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h "#ifndef __mptk_h_\n")
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h "#define __mptk_h_\n")
    FOREACH(HEADER ${HEADER_FILE})
      FILE(READ ${HEADER} BUFFER)
      FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h  "${BUFFER}" )
    ENDFOREACH(HEADER)
    FILE(READ ${CMAKE_SOURCE_DIR}/src/libmptk/mp_pthreads_barrier.h BUFFER)
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h  "${BUFFER}" )
    FILE(READ ${CMAKE_SOURCE_DIR}/mp_system.h BUFFER)
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h  "${BUFFER}" )
    FILE(APPEND ${MPTK_BINARY_DIR}/install/include/mptk.h "#endif /* __mptk_h_ */\n" )
    SET(MPTK_SHARED_HEADER TRUE INTERNAL)
  ENDIF(BUILD_SHARED_LIBS)
ENDIF(NOT EXISTS ${MPTK_BINARY_DIR}/install/include/mptk.h)

#Place this header file in the include directory of the build directory
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/mptk.h.cmake ${MPTK_BINARY_DIR}/src/libmptk/mptk.h)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/config.h.cmake ${MPTK_BINARY_DIR}/src/libmptk/config.h)
# 

#------------------------------------------------
# Set macro to find the sources of MPTK lib:
#
MACRO(GET_MPTK_LIB_CPPSOURCE out)
  SET(${out}
	${CMAKE_SOURCE_DIR}/src/libmptk/header.h 
	${MPTK_BINARY_DIR}/src/libmptk/mptk.h
	${MPTK_BINARY_DIR}/src/libmptk/regression_constants.h
	${HEADER_FILE}
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_messaging.cpp  
	${CMAKE_SOURCE_DIR}/src/libmptk/dll_elf.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/dll_win32.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/general.cpp       
	${CMAKE_SOURCE_DIR}/src/libmptk/mtrand.cpp          
	${CMAKE_SOURCE_DIR}/src/libmptk/regression.cpp    
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_signal.cpp     
	${CMAKE_SOURCE_DIR}/src/libmptk/win_server.cpp    
	${CMAKE_SOURCE_DIR}/src/libmptk/fft_interface.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/dct_interface.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/block.cpp 
	${MPTK_BINARY_DIR}/src/libmptk/atom.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_table.cpp 
   	 ${CMAKE_SOURCE_DIR}/src/libmptk/anywave_server.cpp             
	${CMAKE_SOURCE_DIR}/src/libmptk/convolution.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_factory.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/block_factory.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_param.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_book.cpp
	${MPTK_BINARY_DIR}/src/libmptk/dict.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mask.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/book.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/tfmap.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/mixer.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_core.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_demix_core.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mptk_env.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_param_book.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_pos_book.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_block_book.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/gp_pos_range_sub_book.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/double_index_book.cpp
  	${CMAKE_SOURCE_DIR}/src/libmptk/gpd_core.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/freq_atom_param.cpp
	)

ENDMACRO(GET_MPTK_LIB_CPPSOURCE)

#------------------------------------------------
# Using the macro to find the sources of MPTK lib:
#
GET_MPTK_LIB_CPPSOURCE(MPTK_LIB_CPPSOURCE)
#
#Set the source files properties:
SET_SOURCE_FILES_PROPERTIES(
  ${MPTK_LIB_CPPSOURCE}
  PROPERTIES GENERATED true
)
#

IF(BUILD_SHARED_LIBS)
	ADD_LIBRARY(mptk SHARED ${MPTK_LIB_CPPSOURCE})
ELSE(BUILD_SHARED_LIBS)
	ADD_LIBRARY(mptk STATIC ${MPTK_LIB_CPPSOURCE})
ENDIF(BUILD_SHARED_LIBS)

#In case of 64 bits plateform we have to compil with -fPIC flag
#
SET_TARGET_PROPERTIES(mptk PROPERTIES COMPILE_FLAGS "-DC_BUILDING_MPTK ${CMAKE_CXX_FLAGS}" OUTPUT_NAME "mptk")

  IF(WIN32)
  TARGET_LINK_LIBRARIES(mptk dsp_windows tinyxml ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
  ELSE(WIN32)
  TARGET_LINK_LIBRARIES(mptk dsp_windows dl tinyxml ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ENDIF(WIN32)
#
#------------------------------------------------
# Define install target:
#For mptk library:
IF(BUILD_SHARED_LIBS)
	INSTALL(TARGETS mptk 
 		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION lib)
ENDIF(BUILD_SHARED_LIBS)

IF(WIN32)
	IF(CMAKE_CXX_COMPILER STREQUAL "cl")
		IF(BUILD_DEBUG)
			INSTALL(FILES ${MPTK_BINARY_DIR}/lib/debug/mptk.dll DESTINATION bin)
		ELSE(BUILD_DEBUG)
			INSTALL(FILES ${MPTK_BINARY_DIR}/lib/release/mptk.dll DESTINATION bin)
		ENDIF(BUILD_DEBUG)
	ELSE(CMAKE_CXX_COMPILER STREQUAL "cl")
		INSTALL(FILES ${MPTK_BINARY_DIR}/lib/libmptk.dll DESTINATION bin)
	ENDIF(CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF(WIN32)

