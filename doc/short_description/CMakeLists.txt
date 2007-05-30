INCLUDE(FindLATEX)
IF(LATEX_COMPILER)
  ADD_CUSTOM_COMMAND( 
    OUTPUT    ${MPTK_BINARY_DIR}/doc/short_description/short_description.dvi
    COMMAND   ${LATEX_COMPILER}
    ARGS      ${MPTK_SOURCE_DIR}/doc/short_description/latex_src/short_description.tex
    DEPENDS   ${MPTK_SOURCE_DIR}/doc/short_description/latex_src/short_description.tex
    COMMENT   "Tex2dvi"
  )

  IF(DVIPS_CONVERTER)
    ADD_CUSTOM_COMMAND( 
      OUTPUT    ${MPTK_BINARY_DIR}/doc/short_description/short_description.ps
      COMMAND   ${DVIPS_CONVERTER}
      ARGS      ${MPTK_BINARY_DIR}/doc/short_description/short_description.dvi
                -o ${MPTK_BINARY_DIR}/doc/short_description/short_description.ps
      DEPENDS   ${MPTK_BINARY_DIR}/doc/short_description/short_description.dvi
      COMMENT   "dvi2ps"
   )

 IF(PS2PDF_CONVERTER)
    ADD_CUSTOM_COMMAND( 
      OUTPUT    ${MPTK_BINARY_DIR}/doc/short_description/short_description.pdf
      COMMAND   ${PS2PDF_CONVERTER}
      ARGS      ${MPTK_BINARY_DIR}/doc/short_description/short_description.ps
      DEPENDS   ${MPTK_BINARY_DIR}/doc/short_description/short_description.ps
      COMMENT   "ps2pdf"
    )

    ADD_CUSTOM_TARGET(short_description_LaTeXDocument ALL echo
      DEPENDS   ${MPTK_BINARY_DIR}/doc/short_description/short_description.pdf
    )
    ENDIF(PS2PDF_CONVERTER)
  ENDIF(DVIPS_CONVERTER)
ENDIF(LATEX_COMPILER)
