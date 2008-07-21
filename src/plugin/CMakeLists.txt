# CMakeLists.txt file for MPTK plugins
# This is where you should add instructions to compile your news plugins (blocks/atoms) if needed
#

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#In case of 64 bits plateform we have to compil with -fPIC flag
# THIS SHOULD BE INHERITED FROM CALLING CMakeLists.txt !!!!

IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(PLUGIN_COMPILE_FLAGS " -fPIC")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(PLUGIN_COMPILE_FLAGS " -Wno-deprecated -fPIC")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ELSE( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 
IF( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(PLUGIN_COMPILE_FLAGS "")
ELSE( CMAKE_CXX_COMPILER STREQUAL "cl")
    SET(PLUGIN_COMPILE_FLAGS " -Wno-deprecated")
ENDIF( CMAKE_CXX_COMPILER STREQUAL "cl")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" ) 

#########################################
# GABOR 
#Create the Gabor plugins in DLL
#Including Harmonic Atom

SET(GABOR_ATOM_PLUGIN_SOURCES
base/gabor_atom_plugin.h
base/gabor_atom_plugin.cpp
base/harmonic_atom_plugin.h
base/harmonic_atom_plugin.cpp
  ) 
SET(GABOR_BLOCK_PLUGIN_SOURCES
base/gabor_block_plugin.h
base/gabor_block_plugin.cpp
base/harmonic_block_plugin.h
base/harmonic_block_plugin.cpp
base/chirp_block_plugin.h
base/chirp_block_plugin.cpp
  ) 
#########################################
# CONSTANT
#Create the Constant plugins in DLL

SET(CONSTANT_ATOM_PLUGIN_SOURCES
base/constant_atom_plugin.h
base/constant_atom_plugin.cpp
  ) 
SET(CONSTANT_BLOCK_PLUGIN_SOURCES
base/constant_block_plugin.h
base/constant_block_plugin.cpp
  ) 

#########################################
# DIRAC
#Create the Dirac plugins in DLL

SET(DIRAC_ATOM_PLUGIN_SOURCES
base/dirac_atom_plugin.h
base/dirac_atom_plugin.cpp
) 
SET(DIRAC_BLOCK_PLUGIN_SOURCES
base/dirac_block_plugin.h
base/dirac_block_plugin.cpp
  ) 
#########################################
# NYQUIST
#Create the Nyquist plugins in DLL

SET(NYQUIST_ATOM_PLUGIN_SOURCES
base/nyquist_atom_plugin.h
base/nyquist_atom_plugin.cpp
  ) 
SET(NYQUIST_BLOCK_PLUGIN_SOURCES
base/nyquist_block_plugin.h
base/nyquist_block_plugin.cpp
  ) 

#########################################
# ANYWAVE
#Create the Anywave plugin in DLL
#Including Anywave Hilbert
SET(ANYWAVE_ATOM_PLUGIN_SOURCES
base/anywave_atom_plugin.h
base/anywave_atom_plugin.cpp
base/anywave_hilbert_atom_plugin.h
base/anywave_hilbert_atom_plugin.cpp
  ) 
SET(ANYWAVE_BLOCK_PLUGIN_SOURCES
base/anywave_block_plugin.h
base/anywave_block_plugin.cpp
base/anywave_hilbert_block_plugin.h
base/anywave_hilbert_block_plugin.cpp
  ) 

#########################################
# MCLT
#Create the Mclt plugin in DLL

SET(MCLT_ATOM_PLUGIN_SOURCES
contrib/lam/mclt_atom_plugin.h
contrib/lam/mclt_atom_plugin.cpp
  ) 
SET(MCLT_BLOCK_PLUGIN_SOURCES
contrib/lam/mclt_abstract_block_plugin.h
contrib/lam/mclt_abstract_block_plugin.cpp
contrib/lam/mclt_block_plugin.h
contrib/lam/mclt_block_plugin.cpp
  ) 
#########################################
# MDCT
#Create the Mdct plugin in DLL

SET(MDCT_ATOM_PLUGIN_SOURCES
contrib/lam/mdct_atom_plugin.h
contrib/lam/mdct_atom_plugin.cpp
  ) 
SET(MDCT_BLOCK_PLUGIN_SOURCES
contrib/lam/mclt_abstract_block_plugin.h
contrib/lam/mclt_abstract_block_plugin.cpp
contrib/lam/mdct_block_plugin.h
contrib/lam/mdct_block_plugin.cpp
  ) 

#########################################
# MDST
#Create the Mdst  plugin in DLL

SET(MDST_ATOM_PLUGIN_SOURCES
contrib/lam/mdst_atom_plugin.h
contrib/lam/mdst_atom_plugin.cpp
  ) 
SET(MDST_BLOCK_PLUGIN_SOURCES
contrib/lam/mclt_abstract_block_plugin.h
contrib/lam/mclt_abstract_block_plugin.cpp
contrib/lam/mdst_block_plugin.h
contrib/lam/mdst_block_plugin.cpp
  ) 

################################################
#
# TEMPLATE : TO BUILD YOUR OWN ATOMS AND BLOCKS:
# 1) REPLACE "TEMPLATE" BY THE NAME OF YOUR 
#    BLOCKS/ATOMS AND UNCOMMENT 
# 2) ADD THE NAME OF YOUR BLOCKS/ATOMS IN THE 
#    FOREACH LOOP
#########################################
#Create the <Template> plugin in DLL
#SET(TEMPLATE_ATOM_PLUGIN_SOURCES
#contrib/template/template_atom_plugin.h
#contrib/template/template_atom_plugin.cpp) 
#SET(TEMPLATE_BLOCK_PLUGIN_SOURCES
#contrib/template/template_block_plugin.h
#contrib/template/template_block_plugin.cpp) 

# LOOP TO GENERATE ALL PLUGINS

FOREACH(target GABOR CONSTANT DIRAC NYQUIST ANYWAVE MCLT MDCT MDST)
   FOREACH(type ATOM BLOCK)
      ADD_LIBRARY(${target}_${type} SHARED ${${target}_${type}_PLUGIN_SOURCES})
      SET_TARGET_PROPERTIES(${target}_${type} PROPERTIES
			    COMPILE_FLAGS " -DC_BUILDING_${target}_${type} ${SHARED_FLAGS} ${PLUGIN_COMPILE_FLAGS}"
			    PREFIX ""
    			    OUTPUT_NAME "${target}_${type}")
      TARGET_LINK_LIBRARIES(${target}_${type}  mptk)
      ADD_DEPENDENCIES(${target}_${type}  mptk) 
      IF(BUILD_SHARED_LIBS)
		INSTALL(TARGETS ${target}_${type}
    			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
       			)
      ENDIF(BUILD_SHARED_LIBS)
   ENDFOREACH(type)
ENDFOREACH(target)

# COPY THE PLUGIN INFO. TODO : generate plugin info dynamically
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/src/plugin/plugin-info.txt ${MPTK_BINARY_DIR}/lib/plugin-info.txt COPYONLY)



