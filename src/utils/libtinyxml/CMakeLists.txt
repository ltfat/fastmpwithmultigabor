#------------------------------------------------
# Create TinyXML library
#------------------------------------------------
SET(TINYXML_LIB_CPPSOURCE tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp tinystr.h tinyxml.h)
ADD_LIBRARY(tinyxml STATIC ${TINYXML_LIB_CPPSOURCE})

#------------------------------------------------
# Define install target:
#------------------------------------------------
#For mptk library:
IF(BUILD_SHARED_LIBS)
INSTALL(TARGETS tinyxml
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
ENDIF(BUILD_SHARED_LIBS)
