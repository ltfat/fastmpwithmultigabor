# This is a template CmakeList.txt file for MPTK plugins. It won't work unless the directory name is different from "template"
#   1) Copy all the files contained in the template directory in a new folder under "contrib->NewDirectory"
#   2) Change the names of the files template_(atom/block)_plugin.(h/cpp) with your own choice (for example newMethod_(atom/block)_plugin.(h/cpp))
#   3) Change all the the "template" & "TEMPLATE" parts above with the correct strings (for example "newMethod" & "NEWMETHOD")

#-------------------------------
#            TEMPLATE
#-------------------------------
SET(TEMPLATE_ATOM_PLUGIN_SOURCES
	template_atom_plugin.h
	template_atom_plugin.cpp)
 
SET(TEMPLATE_BLOCK_PLUGIN_SOURCES
	template_block_plugin.h
	template_block_plugin.cpp) 

#########################################
# Loop to generate the plugins
#########################################
FOREACH(target TEMPLATE)
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