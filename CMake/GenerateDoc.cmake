##Generation of the documentation using doxygen

IF (DOXYGEN_FOUND)
    IF (NOT DOXY_IN_FILE_UPDATED)
		IF(DOXYGEN_DOT_FOUND)
    		IF(BUILD_DOCUMENTATION_GUI)
				CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configGUIWITHDOT.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in @ONLY) 
			ELSE(BUILD_DOCUMENTATION_GUI)
				CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configSTDWITHDOT.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in @ONLY)    
			ENDIF(BUILD_DOCUMENTATION_GUI)
		ELSE(DOXYGEN_DOT_FOUND)
			IF(BUILD_DOCUMENTATION_GUI)
				CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configGUI.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in @ONLY) 
			ELSE(BUILD_DOCUMENTATION_GUI)
				CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configSTD.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in @ONLY) 
			ENDIF(BUILD_DOCUMENTATION_GUI)
		ENDIF(DOXYGEN_DOT_FOUND)
		SET(DOXY_IN_FILE_UPDATED 1 INTERNAL) 
	ENDIF (NOT DOXY_IN_FILE_UPDATED)
	ADD_CUSTOM_TARGET(refman_doxygen ALL echo DEPENDS mptk)
	ADD_CUSTOM_COMMAND(
  		TARGET refman_doxygen
		COMMAND   ${DOXYGEN}
		ARGS "${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in"
		COMMENT   "Doxygen generate the documentation")
ELSE (DOXYGEN_FOUND)
	MESSAGE(STATUS "Doxygen not found, cannot generate the documentation")
ENDIF (DOXYGEN_FOUND)