# CMakeLists.txt file for MPTK plugins
# This is where you should add instructions to compile your news plugins (blocks/atoms) if needed
#

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# 1 : Retrieve all the contrib subfloder except templates
FILE(GLOB CONTRIB_PATHS "[^.]*[^*.txt$]")

# 2 : Loop to include the subfolders CMakeLists except "template"
FOREACH(COUNT ${CONTRIB_PATHS})
	IF(NOT(${COUNT} MATCHES "template"))
		ADD_SUBDIRECTORY (${COUNT})
	ENDIF(NOT(${COUNT} MATCHES "template"))
ENDFOREACH(COUNT ${CONTRIB_PATHS})