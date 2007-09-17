#------------------------------------------------
# Create TinyXML library
SET(TINYXML_LIB_CPPSOURCE tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp  )
ADD_CUSTOM_TARGET(tinyxml-lib-source DEPENDS ${TINYXML_LIB_CPPSOURCE})
ADD_LIBRARY(tinyxml STATIC ${TINYXML_LIB_CPPSOURCE})
ADD_DEPENDENCIES(tinyxml tinyxml-lib-source)
#------------------------------------------------
# Define install target:
#For mptk library:
IF(BUILD_SHARED_LIBS)
INSTALL(TARGETS
  tinyxml
 LIBRARY DESTINATION ${LIB_HOME}
 ARCHIVE DESTINATION ${LIB_HOME}
)
ENDIF(BUILD_SHARED_LIBS)

