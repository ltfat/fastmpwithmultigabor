# CMakeLists.txt file for MPTK plugins

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#-------------------------------
#            MCLT
#-------------------------------
SET(MCLT_ATOM_PLUGIN_SOURCES
	mclt_atom_plugin.h
	mclt_atom_plugin.cpp)
 
SET(MCLT_BLOCK_PLUGIN_SOURCES
	mclt_abstract_block_plugin.h
	mclt_abstract_block_plugin.cpp
	mclt_block_plugin.h
	mclt_block_plugin.cpp) 

#-------------------------------
#             MDCT
#-------------------------------
SET(MDCT_ATOM_PLUGIN_SOURCES
	mdct_atom_plugin.h
	mdct_atom_plugin.cpp) 

SET(MDCT_BLOCK_PLUGIN_SOURCES
	mclt_abstract_block_plugin.h
	mclt_abstract_block_plugin.cpp
	mdct_block_plugin.h
	mdct_block_plugin.cpp) 

#-------------------------------
#             MDST
#-------------------------------
SET(MDST_ATOM_PLUGIN_SOURCES
	mdst_atom_plugin.h
	mdst_atom_plugin.cpp)
 
SET(MDST_BLOCK_PLUGIN_SOURCES
	mclt_abstract_block_plugin.h
	mclt_abstract_block_plugin.cpp
	mdst_block_plugin.h
	mdst_block_plugin.cpp) 


#########################################
# Loop to generate the plugins
#########################################
FOREACH(target MDCT MDST MCLT)
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