INCLUDE(FindLATEX)

FOREACH(loopTargets ${EXEC_TARGETS})
	EXECUTE_PROCESS(
		OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex
		COMMAND 	${loopTargets}
		RESULT_VARIABLE rv
	)
	FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex TEMPUSAGE)
	FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "\\tiny\\begin{verbatim}")
	FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "${TEMPUSAGE}")
	FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "\\end{verbatim}\\normalsize")
ENDFOREACH(loopTargets)

IF(MATLAB_ROOT)
	FOREACH(MatLoopTargets ${MAT_EXEC_TARGETS})
		EXECUTE_PROCESS(
			OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex
			COMMAND 	matlab -nosplash -nojvm -nodesktop -r "help ${MPTK_SOURCE_DIR}/src/matlab/${MatLoopTargets}.m,exit"
			RESULT_VARIABLE rv
		)
		# Getting the "Usage" offset
		FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex TEMPUSAGE)
		STRING( FIND ${TEMPUSAGE} "Usage" POSITION)
		STRING(LENGTH ${TEMPUSAGE} LENGTH)
		MATH(EXPR ENDLENGTH ${LENGTH}-${POSITION}-8)
		STRING(SUBSTRING ${TEMPUSAGE} ${POSITION} ${ENDLENGTH} LOLUSAGE)
		IF(NOT ${POSITION} STREQUAL "-1")
			FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex "\\tiny\\begin{verbatim}\n")
			FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex "${LOLUSAGE}")
			FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex "\\end{verbatim}\\normalsize")
		ELSE(NOT ${POSITION} STREQUAL "-1")
			MESSAGE("<<<<<<<<<<<<<<< Error, no \"Usage\" tag has been found in file ${MPTK_SOURCE_DIR}/src/matlab/${MatLoopTargets}.m >>>>>>>>>>>>")
		ENDIF(NOT ${POSITION} STREQUAL "-1")
	ENDFOREACH(MatLoopTargets)

	FOREACH(MatScriptTargets ${MATSCRIPT_EXEC})
		EXECUTE_PROCESS(
			OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex
			COMMAND 	matlab -nosplash -nojvm -nodesktop -r "help ${MPTK_SOURCE_DIR}/src/matlab/scripts/${MatScriptTargets}.m,exit"
			RESULT_VARIABLE rv
		)
		# Getting the "Usage" offset
		FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex TEMPUSAGE)
		STRING( FIND ${TEMPUSAGE} "Usage" POSITION)
		STRING(LENGTH ${TEMPUSAGE} LENGTH)
		MATH(EXPR ENDLENGTH ${LENGTH}-${POSITION}-8)
		STRING(SUBSTRING ${TEMPUSAGE} ${POSITION} ${ENDLENGTH} LOLUSAGE)
		IF(NOT ${POSITION} STREQUAL "-1")
			FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex "\\tiny\\begin{verbatim}\n")
			FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex "${LOLUSAGE}")
			FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex "\\end{verbatim}\\normalsize")
		ELSE(NOT ${POSITION} STREQUAL "-1")
			MESSAGE("<<<<<<<<<<<<<<< Error, no \"Usage\" tag has been found in file ${MPTK_SOURCE_DIR}/src/matlab/scripts/${MatScriptTargets}.m >>>>>>>>>>>>")
		ENDIF(NOT ${POSITION} STREQUAL "-1")
	ENDFOREACH(MatScriptTargets)
ENDIF(MATLAB_ROOT)

IF(LATEX_COMPILER)
	# Copying images
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/WindowsCmake.eps ${MPTK_BINARY_DIR}/doc/userman/WindowsCmake.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/tree.eps ${MPTK_BINARY_DIR}/doc/userman/tree.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/mpcycle.eps ${MPTK_BINARY_DIR}/doc/userman/mpcycle.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/VisualStudioCmake.eps ${MPTK_BINARY_DIR}/doc/userman/VisualStudioCmake.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/WindowsCmake2.eps ${MPTK_BINARY_DIR}/doc/userman/WindowsCmake2.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/FailedCmake.eps ${MPTK_BINARY_DIR}/doc/userman/FailedCmake.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/bookeditPage.eps ${MPTK_BINARY_DIR}/doc/userman/bookeditPage.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/bookeditToolbar.eps ${MPTK_BINARY_DIR}/doc/userman/bookeditToolbar.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/bookeditFileToolbar.eps ${MPTK_BINARY_DIR}/doc/userman/bookeditFileToolbar.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/bookeditEditToolbar.eps ${MPTK_BINARY_DIR}/doc/userman/bookeditEditToolbar.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/bookeditTransformToolbar.eps ${MPTK_BINARY_DIR}/doc/userman/bookeditTransformToolbar.eps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/cTestReport.eps ${MPTK_BINARY_DIR}/doc/userman/cTestReport.eps COPYONLY)

	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/biblio_MP.bib ${MPTK_BINARY_DIR}/doc/userman/biblio_MP.bib COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/atom.ps ${MPTK_BINARY_DIR}/doc/userman/atom.ps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/plonk.ps ${MPTK_BINARY_DIR}/doc/userman/plonk.ps COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/tree.fig ${MPTK_BINARY_DIR}/doc/userman/tree.fig COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/mpcycle.fig ${MPTK_BINARY_DIR}/doc/userman/mpcycle.fig COPYONLY)
	CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/userman.tex ${MPTK_BINARY_DIR}/doc/userman/userman.tex COPYONLY)

	IF(BIBTEX_COMPILER) 
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.aux
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.tex
			COMMAND   ${LATEX_COMPILER}
			ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Latex (first pass)"
		)
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.bbl
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.aux
			COMMAND   ${BIBTEX_COMPILER}
			ARGS      -terse ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Bibtex"
		)
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.bbl
			COMMAND   ${LATEX_COMPILER}
			ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Latex (second pass)"
		)
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.log
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.bbl
			          ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
			COMMAND   ${LATEX_COMPILER}
			ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Latex (third pass)"
		)

		# Eventually trigger the whole process
		ADD_CUSTOM_TARGET(userman ALL echo
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.log
		)
	ENDIF(BIBTEX_COMPILER) 

	IF(DVIPS_CONVERTER)
		ADD_CUSTOM_COMMAND( 
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.ps
			COMMAND   ${DVIPS_CONVERTER}
			ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.dvi -o ${MPTK_BINARY_DIR}/doc/userman/userman.ps
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
			COMMENT   "dvi2ps"
   		)
		IF(PS2PDF_CONVERTER)
			ADD_CUSTOM_COMMAND( 
				OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.pdf
				COMMAND   ${PS2PDF_CONVERTER}
				ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.ps
				DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.ps
				COMMENT   "ps2pdf"
			)
    
			ADD_CUSTOM_TARGET(userman ALL echo
				DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.pdf
    			)
			INSTALL(FILES ${MPTK_BINARY_DIR}/doc/userman/userman.pdf DESTINATION doc)
		ENDIF(PS2PDF_CONVERTER)
	ENDIF(DVIPS_CONVERTER)
ENDIF(LATEX_COMPILER)