##Generation of the documentation using doxygen
FIND_PATH(DOXYGEN_DIR doxygen
/usr/local/bin
/usr/bin
 )
    IF (DOXYGEN_DIR)
    IF (NOT DOXY_IN_FILE_UPDATED)
IF(DOT)
    IF(BUILD_DOCUMENTATION_GUI)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configGUIWITHDOT.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in
@ONLY) 
ELSE(BUILD_DOCUMENTATION_GUI)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configSTDWITHDOT.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in
                  @ONLY)    
ENDIF(BUILD_DOCUMENTATION_GUI)
ELSE(DOT)
IF(BUILD_DOCUMENTATION_GUI)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configGUI.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in
@ONLY) 
ELSE(BUILD_DOCUMENTATION_GUI)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/refman/doxygen_configSTD.in ${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in
                  @ONLY) 
ENDIF(BUILD_DOCUMENTATION_GUI)
ENDIF(DOT)
SET(DOXY_IN_FILE_UPDATED 1 INTERNAL) 
ENDIF (NOT DOXY_IN_FILE_UPDATED)
ADD_CUSTOM_TARGET(refman_doxygen ALL echo
      DEPENDS mptk
    )
ADD_CUSTOM_COMMAND(
  TARGET refman_doxygen
  COMMAND   ${DOXYGEN_DIR}/doxygen
  ARGS "${MPTK_BINARY_DIR}/doc/refman/doxygen_config.in"
  COMMENT   "Doxygen generate the documentation"
  )
    ELSE (DOXYGEN_DIR)
    MESSAGE(STATUS "Doxygen not found, cannot generate the documentation")
    ENDIF (DOXYGEN_DIR)
