INCLUDE(FindLATEX)

FOREACH(loopTargets ${EXEC_TARGETS})
	EXECUTE_PROCESS(
		OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex
		COMMAND 	${loopTargets}
		RESULT_VARIABLE rv
	)
	FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex TEMPUSAGE)
	FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "\\tiny\\begin{verbatim}")
	FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "${TEMPUSAGE}")
	FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "\\end{verbatim}\\normalsize")
ENDFOREACH(loopTargets)


IF(LATEX_COMPILER)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/biblio_MP.bib ${MPTK_BINARY_DIR}/doc/userman/biblio_MP.bib COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/atom.ps ${MPTK_BINARY_DIR}/doc/userman/atom.ps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/plonk.ps ${MPTK_BINARY_DIR}/doc/userman/plonk.ps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/tree.eps ${MPTK_BINARY_DIR}/doc/userman/tree.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/tree.fig ${MPTK_BINARY_DIR}/doc/userman/tree.fig COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/mpcycle.eps ${MPTK_BINARY_DIR}/doc/userman/mpcycle.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/mpcycle.fig ${MPTK_BINARY_DIR}/doc/userman/mpcycle.fig COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/userman.tex ${MPTK_BINARY_DIR}/doc/userman/userman.tex COPYONLY)

	IF(BIBTEX_COMPILER) 
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.aux
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.tex
				  ${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex
			COMMAND   ${LATEX_COMPILER}
			ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Latex (first pass)"
		)
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.bbl
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.aux
				  ${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex
			COMMAND   ${BIBTEX_COMPILER}
			ARGS      -terse ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Bibtex"
		)
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.bbl
				  ${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex
			COMMAND   ${LATEX_COMPILER}
			ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Latex (second pass)"
		)
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.log
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.bbl
				  ${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex
			          ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
			COMMAND   ${LATEX_COMPILER}
			ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Latex (third pass)"
		)
		# Eventually trigger the whole process
		ADD_CUSTOM_TARGET(LaTeXDocument ALL echo
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.log
		)
	ENDIF(BIBTEX_COMPILER) 

	IF(DVIPS_CONVERTER)
		ADD_CUSTOM_COMMAND( 
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.ps
			COMMAND   ${DVIPS_CONVERTER}
			ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.dvi -o ${MPTK_BINARY_DIR}/doc/userman/userman.ps
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
				  ${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex
			COMMENT   "dvi2ps"
   		)
		IF(PS2PDF_CONVERTER)
			ADD_CUSTOM_COMMAND( 
				OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.pdf
				COMMAND   ${PS2PDF_CONVERTER}
				ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.ps
				DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.ps
				  	  ${MPTK_BINARY_DIR}/doc/userman/TempUsage.tex
				COMMENT   "ps2pdf"
			)
    
			ADD_CUSTOM_TARGET(userman_LaTeXDocument ALL echo
				DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.pdf
    			)
			INSTALL(FILES ${MPTK_BINARY_DIR}/doc/userman/userman.pdf DESTINATION doc)
		ENDIF(PS2PDF_CONVERTER)
	ENDIF(DVIPS_CONVERTER)
ENDIF(LATEX_COMPILER)
