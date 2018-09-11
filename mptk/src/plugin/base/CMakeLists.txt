# CMakeLists.txt file for MPTK plugins
# Compiling the bases plugins (blocks/atoms)


if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#-----------------------------------------
#                 GABOR 
#-----------------------------------------
SET(GABOR_ATOM_PLUGIN_SOURCES
	gabor_atom_plugin.h
	gabor_atom_plugin.cpp
	harmonic_atom_plugin.h
	harmonic_atom_plugin.cpp) 

SET(GABOR_BLOCK_PLUGIN_SOURCES
	gabor_block_plugin.h
	gabor_block_plugin.cpp
	harmonic_block_plugin.h
	harmonic_block_plugin.cpp
	chirp_block_plugin.h
	chirp_block_plugin.cpp) 

#-----------------------------------------
#              CONSTANT
#-----------------------------------------
SET(CONSTANT_ATOM_PLUGIN_SOURCES
	constant_atom_plugin.h
	constant_atom_plugin.cpp) 

SET(CONSTANT_BLOCK_PLUGIN_SOURCES
	constant_block_plugin.h
	constant_block_plugin.cpp) 

#-----------------------------------------
#                DIRAC
#-----------------------------------------
SET(DIRAC_ATOM_PLUGIN_SOURCES
	dirac_atom_plugin.h
	dirac_atom_plugin.cpp)
 
SET(DIRAC_BLOCK_PLUGIN_SOURCES
	dirac_block_plugin.h
	dirac_block_plugin.cpp) 

#-----------------------------------------
#                 NYQUIST
#-----------------------------------------
SET(NYQUIST_ATOM_PLUGIN_SOURCES
	nyquist_atom_plugin.h
	nyquist_atom_plugin.cpp) 

SET(NYQUIST_BLOCK_PLUGIN_SOURCES
	nyquist_block_plugin.h
	nyquist_block_plugin.cpp) 

#-----------------------------------------
#                 ANYWAVE
#-----------------------------------------
SET(ANYWAVE_ATOM_PLUGIN_SOURCES
	anywave_atom_plugin.h
	anywave_atom_plugin.cpp
	anywave_hilbert_atom_plugin.h
	anywave_hilbert_atom_plugin.cpp) 

SET(ANYWAVE_BLOCK_PLUGIN_SOURCES
	anywave_block_plugin.h
	anywave_block_plugin.cpp
	anywave_hilbert_block_plugin.h
	anywave_hilbert_block_plugin.cpp) 


#########################################
# Loop to generate the plugins
#########################################
FOREACH(target GABOR CONSTANT DIRAC NYQUIST ANYWAVE)
	FOREACH(type ATOM BLOCK)
		ADD_LIBRARY(${target}_${type} SHARED ${${target}_${type}_PLUGIN_SOURCES})
		SET_TARGET_PROPERTIES(${target}_${type} PROPERTIES
			COMPILE_FLAGS " -DC_BUILDING_${target}_${type} ${CMAKE_CXX_FLAGS}"
			PREFIX ""
    			OUTPUT_NAME "${target}_${type}")
		TARGET_LINK_LIBRARIES(${target}_${type}  mptk)
		ADD_DEPENDENCIES(${target}_${type}  mptk) 
		IF(BUILD_SHARED_LIBS)
			INSTALL(TARGETS ${target}_${type}
    				LIBRARY DESTINATION mptk/plugins
				ARCHIVE DESTINATION mptk/plugins
				RUNTIME DESTINATION mptk/plugins)
		ENDIF(BUILD_SHARED_LIBS)
	ENDFOREACH(type)
ENDFOREACH(target)