set(CTEST_PROJECT_NAME "MPTK")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "cdash.inria.fr")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=MPTK")
set(CTEST_DROP_SITE_CDASH TRUE)

#--------------------------------------------------------------------
# BUILNAME variable construction
# This variable will be used to set the build name which will appear 
# on the MPTK dashboard http://cdash.irisa.fr/CDash/
#--------------------------------------------------------------------
# Start with the short system name, e.g. "Linux", "FreeBSD" or "Windows"
#IF(BUILDNAME)
#  SET(BUILDNAME "${BUILDNAME}-${CMAKE_SYSTEM_NAME}")
#ELSE(BUILDNAME)
  # To suppress the first space if BUILDNAME is not set
#  SET(BUILDNAME "${CMAKE_SYSTEM_NAME}")
#ENDIF(BUILDNAME)

# Add the compiler name, e.g. "g++, ..."
#SET(BUILDNAME "${BUILDNAME}-${CMAKE_BASE_NAME}")

# Add the type of library generation, e.g. "Dynamic or Static"
#IF(BUILD_SHARED_LIBS)
#  SET(BUILDNAME "${BUILDNAME}-Dynamic")
#ELSE(BUILD_SHARED_LIBS)
#  SET(BUILDNAME "${BUILDNAME}-Static")
#ENDIF(BUILD_SHARED_LIBS)

# Add the build type, e.g. "Debug, Release..."
#IF(CMAKE_BUILD_TYPE)
#  SET(BUILDNAME "${BUILDNAME}-${CMAKE_BUILD_TYPE}")
#ENDIF(CMAKE_BUILD_TYPE)
