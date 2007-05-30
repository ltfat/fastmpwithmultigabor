INCLUDE(FindLATEX)
IF(LATEX_COMPILER)
    ADD_CUSTOM_TARGET(usermam_LaTeXDocument ALL echo
      DEPENDS    ${MPTK_BINARY_DIR}/doc/userman/userman.pdf
    )
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/biblio_MP.bib ${MPTK_BINARY_DIR}/doc/userman/biblio_MP.bib
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/atom.ps ${MPTK_BINARY_DIR}/doc/userman/atom.ps
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/plonk.ps ${MPTK_BINARY_DIR}/doc/userman/plonk.ps
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/tree.eps ${MPTK_BINARY_DIR}/doc/userman/tree.eps
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/tree.fig ${MPTK_BINARY_DIR}/doc/userman/tree.fig
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/mpcycle.eps ${MPTK_BINARY_DIR}/doc/userman/mpcycle.eps
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/mpcycle.fig ${MPTK_BINARY_DIR}/doc/userman/mpcycle.fig
                 COPYONLY)
CONFIGURE_FILE(${MPTK_SOURCE_DIR}/doc/userman/latex_src/userman.tex ${MPTK_BINARY_DIR}/doc/userman/userman.tex
                 COPYONLY)
ADD_CUSTOM_COMMAND(
  OUTPUT    ${MPTK_BINARY_DIR}/doc/userman/userman.tex
  DEPENDS   ${MPTK_BINARY_DIR}/doc/userman/userman.tex
  COMMAND   ${LATEX_COMPILER}
  ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.tex
      COMMENT   "Latex (first pass)"
  )

ADD_CUSTOM_COMMAND(
TARGET usermam_LaTeXDocument
  COMMAND   ${LATEX_COMPILER}
  ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.tex
      COMMENT   "Latex (second pass)"
  )
IF(BIBTEX_COMPILER) 
ADD_CUSTOM_COMMAND(
TARGET usermam_LaTeXDocument
  COMMAND   ${BIBTEX_COMPILER}
  ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman
  COMMENT   "Bibtex"
  )
ENDIF(BIBTEX_COMPILER) 

ADD_CUSTOM_COMMAND(
  TARGET usermam_LaTeXDocument
  COMMAND   ${LATEX_COMPILER}
  ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman 
  COMMENT   "Latex (third pass)"
  )
  
ADD_CUSTOM_COMMAND(
  TARGET usermam_LaTeXDocument
  COMMAND   ${LATEX_COMPILER}
  ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman 
  COMMENT   "Latex (third pass)"
  )
ADD_CUSTOM_COMMAND(
  TARGET usermam_LaTeXDocument
            ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
  COMMAND   ${LATEX_COMPILER}
  ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.bbl  
  COMMENT   "Latex (fourth pass)"
  )
ADD_CUSTOM_COMMAND(
  TARGET usermam_LaTeXDocument
            ${MPTK_BINARY_DIR}/doc/userman/userman.dvi
  COMMAND   ${LATEX_COMPILER}
  ARGS      ${MPTK_BINARY_DIR}/doc/userman/userman.bbl  
  COMMENT   "Latex (fourth pass)"
  )
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
    ENDIF(PS2PDF_CONVERTER)
  ENDIF(DVIPS_CONVERTER)
ENDIF(LATEX_COMPILER)
