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
    ${CMAKE_SOURCE_DIR}/src/libmptk/dll.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/win_server.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/fft_interface.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/general.h           
	${CMAKE_SOURCE_DIR}/src/libmptk/mtrand.h            
	${CMAKE_SOURCE_DIR}/src/libmptk/regression.h        
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_signal.h         
	${CMAKE_SOURCE_DIR}/src/libmptk/tfmap.h             
	${CMAKE_SOURCE_DIR}/src/libmptk/atom.h
	${CMAKE_SOURCE_DIR}/src/libmptk/block.h     
	${CMAKE_SOURCE_DIR}/src/libmptk/mask.h              
	${CMAKE_SOURCE_DIR}/src/libmptk/book.h 
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
FILE(READ ${CMAKE_SOURCE_DIR}/src/libmptk/mp_pthreads_barrier.h BUFFER)
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h  "${BUFFER}" )
FILE(APPEND ${MPTK_BINARY_DIR}/include/mptk.h "#endif /* __mptk_h_ */\n" )
SET(MPTK_SHARED_HEADER TRUE INTERNAL)
ENDIF(NOT MPTK_SHARED_HEADER)
ENDIF(BUILD_SHARED_LIBS)
#Place this header file in the include directory of the build directory

CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/libmptk/mptk.h.cmake ${MPTK_BINARY_DIR}/src/libmptk/mptk.h)
INSTALL(FILES ${MPTK_BINARY_DIR}/include/mptk.h DESTINATION include)
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
	${CMAKE_SOURCE_DIR}/src/libmptk/mtrand.c          
	${CMAKE_SOURCE_DIR}/src/libmptk/regression.cpp    
	${CMAKE_SOURCE_DIR}/src/libmptk/mp_signal.cpp     
	${CMAKE_SOURCE_DIR}/src/libmptk/win_server.cpp    
	${CMAKE_SOURCE_DIR}/src/libmptk/fft_interface.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/block.cpp 
	${MPTK_BINARY_DIR}/src/libmptk/atom.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/anywave_table.cpp 
    ${CMAKE_SOURCE_DIR}/src/libmptk/anywave_server.cpp             
	${CMAKE_SOURCE_DIR}/src/libmptk/convolution.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/atom_factory.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/block_factory.cpp
	${MPTK_BINARY_DIR}/src/libmptk/dict.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mask.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/book.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/tfmap.cpp 
	${CMAKE_SOURCE_DIR}/src/libmptk/mixer.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_core.cpp
	${CMAKE_SOURCE_DIR}/src/libmptk/mpd_demix_core.cpp
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
#Set the source files properties:
SET_SOURCE_FILES_PROPERTIES(
  ${MPTK_LIB_CPPSOURCE}
  PROPERTIES GENERATED true
)
#
#------------------------------------------------
#Create the library in dynamic format:
IF(WIN32)
SET(CMAKE_SHARED_LINKER_FLAGS " -Wl,--export-all-symbols,--enable-auto-import,--enable-runtime-pseudo-reloc -Wno-deprecated")
ELSE(WIN32)
IF(APPLE)
SET(CMAKE_SHARED_LINKER_FLAGS "-single_module -Wno-deprecated") 
ELSE(APPLE)
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-dynamic -Wno-deprecated")
ENDIF(APPLE)
ENDIF(WIN32)

 

ADD_LIBRARY(mptk SHARED ${MPTK_LIB_CPPSOURCE})
#In case of 64 bits plateform we have to compil with -fPIC flag
#
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(mptk PROPERTIES
   COMPILE_FLAGS "-DC_BUILDING_MPTK ${SHARED_FLAGS} -Wno-deprecated -fPIC"
    OUTPUT_NAME "mptk"
    )
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  SET_TARGET_PROPERTIES(mptk PROPERTIES
   COMPILE_FLAGS "-DC_BUILDING_MPTK ${SHARED_FLAGS} -Wno-deprecated"
    OUTPUT_NAME "mptk"
    )
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  ADD_DEPENDENCIES(mptk mptk-lib-source)
  IF(WIN32)
  TARGET_LINK_LIBRARIES(mptk dsp_windows tinyxml ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
  ELSE(WIN32)
TARGET_LINK_LIBRARIES(mptk dsp_windows dl tinyxml ${PTHREAD_LIBRARY_FILE} ${SNDFILE_LIBRARY_FILE} ${FFTW3_LIBRARY_FILE})
ENDIF(WIN32)
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
 LIBRARY DESTINATION lib
 ARCHIVE DESTINATION lib
)
IF(WIN32)
INSTALL(FILES ${MPTK_BINARY_DIR}/lib/libmptk.dll DESTINATION bin)
ENDIF(WIN32)
#for mptk include file:
INSTALL(FILES ${MPTK_BINARY_DIR}/path.xml DESTINATION bin)
ENDIF(BUILD_SHARED_LIBS)
