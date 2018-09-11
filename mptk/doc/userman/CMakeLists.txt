INCLUDE(FindLATEX)

IF(BUILD_PYTHON_FILES)
	#TODO : make sure that the python command is set correctly and corresponds to an install where matplotlib is found correctly. This should already be done in FindPython.cmake
	# SET(PYTHON_EXECUTABLE /opt/local/bin/python2.7)
	# TODO this currently only works when pyMPTK is *installed* - we want to be able to run it even on first build
	FOREACH(OnePythonModule mptk mptkplot)
		EXECUTE_PROCESS(
			OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempPythonUsage.tex
			COMMAND 	${PYTHON_EXECUTABLE} -c "import ${OnePythonModule}; help(${OnePythonModule})"
			RESULT_VARIABLE rv
		)
#MESSAGE("python -c \"import ${OnePythonModule}; help(${OnePythonModule})\"")

		FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempPythonUsage.tex TEMPUSAGE)
		STRING(REPLACE "_" "\\_" TEMPUSAGE "${TEMPUSAGE}")
		FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/Python_${OnePythonModule}_Usage.tex "\\fontsize{8}{10}\\begin{alltt}")
		FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/Python_${OnePythonModule}_Usage.tex "${TEMPUSAGE}")
		FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/Python_${OnePythonModule}_Usage.tex "\\end{alltt}\\normalsize")
	ENDFOREACH()
ENDIF(BUILD_PYTHON_FILES)

FOREACH(loopTargets ${EXEC_TARGETS})
        # Get the version number. We need to get it only once, this will be with the MPD command
	IF(${loopTargets} STREQUAL "mpd")
		EXECUTE_PROCESS(
			OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempVersionUsage.tex
			COMMAND 		${loopTargets} -V
			RESULT_VARIABLE rv
		)

		FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempVersionUsage.tex VERSIONUSAGE)
		STRING(FIND ${VERSIONUSAGE} "version" VERSIONPOSITION)
		MATH(EXPR VERSIONFINALPOSITION ${VERSIONPOSITION}+8)
		STRING(LENGTH ${VERSIONUSAGE} VERSIONLENGTH)
		MATH(EXPR ENDVERSIONLENGTH ${VERSIONLENGTH}-${VERSIONFINALPOSITION}-1)
		STRING(SUBSTRING ${VERSIONUSAGE} ${VERSIONFINALPOSITION} ${ENDVERSIONLENGTH} VERSIONFINALUSAGE)
		FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/MPTKVersion.tex "${VERSIONFINALUSAGE}")
	ENDIF(${loopTargets} STREQUAL "mpd")
        # Get the usage for all command lines
	EXECUTE_PROCESS(
		OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex
		COMMAND 	${loopTargets}
		RESULT_VARIABLE rv
	)
	FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex TEMPUSAGE)
	FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "\\fontsize{8}{10}\\begin{alltt}")
	FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "${TEMPUSAGE}")
	FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${loopTargets}_Usage.tex "\\end{alltt}\\normalsize")
ENDFOREACH(loopTargets)

IF(MATLAB_ROOT)
# TODO: check this works for Windows as well as 
SET(MATLAB_CMD ${MATLAB_ROOT}/bin/matlab)
        # Get usage for each Matlab executable target
	FOREACH(MatLoopTargets ${MAT_EXEC_TARGETS})
		IF(NOT EXISTS ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex)
			EXECUTE_PROCESS(
				OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempMatlabCmdUsage.tex
				COMMAND 	${MATLAB_CMD} -nosplash -nojvm -nodesktop -r "help ${MPTK_SOURCE_DIR}/src/matlab/${MatLoopTargets}.m,exit" > ${MPTK_BINARY_DIR}/doc/userman/TempMatlabCmdUsage.tex
				RESULT_VARIABLE rv
			)
			# Getting the "Usage" offset
			FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempMatlabCmdUsage.tex TEMPUSAGE)
			STRING( FIND ${TEMPUSAGE} "Usage" POSITION)
			STRING(LENGTH ${TEMPUSAGE} LENGTH)
			MATH(EXPR ENDLENGTH ${LENGTH}-${POSITION}-8)
			STRING(SUBSTRING ${TEMPUSAGE} ${POSITION} ${ENDLENGTH} LOLUSAGE)
			IF(NOT ${POSITION} STREQUAL "-1")
				FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex "\\fontsize{8}{10}\\begin{alltt}\n")
				FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex "${LOLUSAGE}")
				FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex "\\end{alltt}\\normalsize")
			ELSE(NOT ${POSITION} STREQUAL "-1")
				MESSAGE("<<<<<<<<<<<<<<< Error, no \"Usage\" tag has been found in file ${MPTK_SOURCE_DIR}/src/matlab/${MatLoopTargets}.m >>>>>>>>>>>>")
			ENDIF(NOT ${POSITION} STREQUAL "-1")
		ENDIF(NOT EXISTS ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatLoopTargets}_Usage.tex)
	ENDFOREACH(MatLoopTargets)

        # Get usage for each Matlab script
	FOREACH(MatScriptTargets ${MATSCRIPT_EXEC})
		IF(NOT EXISTS ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex)
			EXECUTE_PROCESS(
				OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex
				COMMAND 	${MATLAB_CMD} -nosplash -nojvm -nodesktop -r "help ${MPTK_SOURCE_DIR}/src/matlab/scripts/${MatScriptTargets}.m,exit"
				RESULT_VARIABLE rv
			)
			# Getting the "Usage" offset
			FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempCmdUsage.tex TEMPUSAGE)
			STRING( FIND ${TEMPUSAGE} "Usage" POSITION)
			STRING(LENGTH ${TEMPUSAGE} LENGTH)
			MATH(EXPR ENDLENGTH ${LENGTH}-${POSITION}-8)
			STRING(SUBSTRING ${TEMPUSAGE} ${POSITION} ${ENDLENGTH} LOLUSAGE)
			IF(NOT ${POSITION} STREQUAL "-1")
				FILE(WRITE ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex "\\fontsize{8}{10}\\begin{alltt}\n")
				FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex "${LOLUSAGE}")
				FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex "\\end{alltt}\\normalsize")
			ELSE(NOT ${POSITION} STREQUAL "-1")
				MESSAGE("<<<<<<<<<<<<<<< Error, no \"Usage\" tag has been found in file ${MPTK_SOURCE_DIR}/src/matlab/scripts/${MatScriptTargets}.m >>>>>>>>>>>>")
			ENDIF(NOT ${POSITION} STREQUAL "-1")
		ENDIF(NOT EXISTS ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/${MatScriptTargets}_Usage.tex)
	ENDFOREACH(MatScriptTargets)

SET(MatScriptTargets latexdoc_block_xmltags)
		     IF(NOT EXISTS ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/DictXMLUsage.tex)
# REPLACE ENVIRONMENT VARIABLES IN script (TODO: make sure this works even when first compiling MPTK. For the moment it may rely on the fact that MPTK has been installed)
			 CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/${MatScriptTargets}.m.in ${MPTK_BINARY_DIR}/doc/userman/${MatScriptTargets}.m @ONLY)
# RUN MATLAB TO APPLY SCRIPT AND GET DICT XML TAGS DOCUMENTATION
			EXECUTE_PROCESS(
				OUTPUT_FILE 	${MPTK_BINARY_DIR}/doc/userman/TempDictXMLUsage.tex
				COMMAND 	${MATLAB_CMD} -nosplash -nojvm -nodesktop -r "cd('${MPTK_BINARY_DIR}/doc/userman');${MatScriptTargets},exit"
				RESULT_VARIABLE rv
			)
			# Getting the "BEGINCONTENT" offset
			FILE(READ ${MPTK_BINARY_DIR}/doc/userman/TempDictXMLUsage.tex TEMPUSAGE)
			STRING( FIND ${TEMPUSAGE} "BEGINCONTENT" TMPPOSITION)
                        MATH(EXPR POSITION ${TMPPOSITION}+12)
			STRING(LENGTH ${TEMPUSAGE} LENGTH)
			MATH(EXPR ENDLENGTH ${LENGTH}-${POSITION}-8)
			STRING(SUBSTRING ${TEMPUSAGE} ${POSITION} ${ENDLENGTH} LOLUSAGE)
			IF(NOT ${POSITION} STREQUAL "-1")
				FILE(APPEND ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/DictXMLUsage.tex "${LOLUSAGE}")
			ELSE(NOT ${POSITION} STREQUAL "-1")
				MESSAGE("<<<<<<<<<<<<<<< Error, no \"\begin{itemize}\" tag has been found >>>>>>>>>>>>")
			ENDIF(NOT ${POSITION} STREQUAL "-1")
		ENDIF(NOT EXISTS ${MPTK_BINARY_DIR}/doc/userman/CommandUsage/DictXMLUsage.tex)
ENDIF(MATLAB_ROOT)

IF(LATEX_COMPILER)
	# Copying images
	SET(IMAGES CommandPrompt.eps EnvironmentConfiguration.eps FailedCmake.eps MacLicence.eps MacPathSelection.eps MatlabPathSelect.eps PathSelection.eps UnixCMakeGUI.eps VisualStudioCmake.eps WindowsCMakeGUI.eps WindowsCmake.eps WindowsCmake2.eps WindowsLicence.eps bookeditEditToolbar.eps bookeditFileToolbar.eps bookeditPage.eps bookeditToolbar.eps bookeditTransformToolbar.eps cTestReport.eps logo.eps mpcycle.eps tree.eps)
	FOREACH(file ${IMAGES})
		CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/Images/${file} ${MPTK_BINARY_DIR}/doc/userman/Images/${file} COPYONLY)
	ENDFOREACH(file)

	# Copying tex
	SET(texdocs shortdescription.tex MPTK4Windows.tex MPTK4Linux.tex MPTK4OSX.tex MPTK4Matlab.tex MPTK4Python.tex MPTK4CmdLine.tex MPTKSourceInstall.tex MPTKHelp.tex userman.tex usermanIntro.tex biblio_MP.bib atom.ps plonk.ps)
	FOREACH(file ${texdocs})
		CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/${file} ${MPTK_BINARY_DIR}/doc/userman/${file} COPYONLY)
	ENDFOREACH(file)
	IF(BIBTEX_COMPILER) 
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.aux
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.tex
			COMMAND   ${LATEX_COMPILER}
			ARGS      -interaction=batchmode ${MPTK_BINARY_DIR}/doc/userman/userman
			COMMENT   "Latex (first pass)"
		)
		ADD_CUSTOM_COMMAND(
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.bbl ${MPTK_BINARY_DIR}/doc/userman/userman.blg
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
		ADD_CUSTOM_TARGET(userman1 ALL echo
			DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.log
		)
	ENDIF(BIBTEX_COMPILER) 

	IF(DVIPS_CONVERTER)
		ADD_CUSTOM_COMMAND( 
			OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.ps
			COMMAND   ${DVIPS_CONVERTER}
			ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.dvi 
					  -o ${MPTK_BINARY_DIR}/doc/userman/userman.ps
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
    
			ADD_CUSTOM_TARGET(userman2 ALL echo
				DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.pdf
    			)
			
			INSTALL(FILES ${MPTK_BINARY_DIR}/doc/userman/userman.pdf DESTINATION doc)
		ENDIF(PS2PDF_CONVERTER)
	ENDIF(DVIPS_CONVERTER)
ENDIF(LATEX_COMPILER)
