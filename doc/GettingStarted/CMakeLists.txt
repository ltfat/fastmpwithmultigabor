INCLUDE(FindLATEX)

IF(LATEX_COMPILER)
	# Copying tex files
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/GettingStarted.tex ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.tex COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/GettingStartedContent.tex ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStartedContent.tex COPYONLY)
	# Copying images
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/Images/EnvironmentConfiguration.eps ${MPTK_BINARY_DIR}/doc/GettingStarted/Images/EnvironmentConfiguration.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/Images/PathSelection.eps ${MPTK_BINARY_DIR}/doc/GettingStarted/Images/PathSelection.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/Images/WindowsLicence.eps ${MPTK_BINARY_DIR}/doc/GettingStarted/Images/WindowsLicence.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/Images/CommandPrompt.eps ${MPTK_BINARY_DIR}/doc/GettingStarted/Images/CommandPrompt.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/Images/MacPathSelection.eps ${MPTK_BINARY_DIR}/doc/GettingStarted/Images/MacPathSelection.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/GettingStarted/latex_src/Images/MacLicence.eps ${MPTK_BINARY_DIR}/doc/GettingStarted/Images/MacLicence.eps COPYONLY)


	ADD_CUSTOM_COMMAND(
		OUTPUT    ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.aux
		DEPENDS   ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.tex
		COMMAND   ${LATEX_COMPILER}
		ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted
		COMMENT   "Latex (first pass)"
	)
	ADD_CUSTOM_COMMAND(
		OUTPUT    ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.dvi
		DEPENDS   ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.aux
		COMMAND   ${LATEX_COMPILER}
		ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted
		COMMENT   "Latex (second pass)"
	)
	ADD_CUSTOM_COMMAND(
		OUTPUT    ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.log
		DEPENDS   ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.dvi
		COMMAND   ${LATEX_COMPILER}
		ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted
		COMMENT   "Latex (third pass)"
	)

	IF(DVIPS_CONVERTER)
		ADD_CUSTOM_COMMAND( 
			OUTPUT    ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.ps
			COMMAND   ${DVIPS_CONVERTER}
			 ARGS     ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.dvi
			          -o ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.ps
			DEPENDS   ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.dvi
			COMMENT   "dvi2ps"
		)

		IF(PS2PDF_CONVERTER)
			ADD_CUSTOM_COMMAND( 
				OUTPUT    ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.pdf
				COMMAND   ${PS2PDF_CONVERTER}
				ARGS      ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.ps
				DEPENDS   ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.ps
				COMMENT   "ps2pdf"
			)

			ADD_CUSTOM_TARGET(GettingStarted ALL echo
				DEPENDS   ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.pdf
			)

			INSTALL(FILES ${MPTK_BINARY_DIR}/doc/GettingStarted/GettingStarted.pdf DESTINATION doc)   
		ENDIF(PS2PDF_CONVERTER)
	ENDIF(DVIPS_CONVERTER)
ENDIF(LATEX_COMPILER)
