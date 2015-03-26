CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)

IF(WIN32)
	set(CTEST_CMAKE_GENERATOR "Visual Studio 9 2008")
	SET (CTEST_SOURCE_DIRECTORY "$ENV{USERPROFILE}/mptk_test")
	SET (CTEST_BINARY_DIRECTORY "$ENV{USERPROFILE}/mptk_test_build")
ELSE(WIN32)
	SET(CTEST_CMAKE_GENERATOR "Unix Makefiles")
	SET (CTEST_SOURCE_DIRECTORY "$ENV{HOME}/mptk_test")
	SET (CTEST_BINARY_DIRECTORY "$ENV{HOME}/mptk_test_build")
ENDIF(WIN32)

FIND_PROGRAM(CTEST_SVN_COMMAND NAMES svn)
SET (CTEST_SVN_CHECKOUT  "${CTEST_SVN_COMMAND} checkout --username anonsvn --password anonsvn https://scm.gforge.inria.fr/svn/mptk/trunk \"${CTEST_SOURCE_DIRECTORY}\"")
SET (CTEST_CHECKOUT_COMMAND "${CTEST_SVN_CHECKOUT}")


site_name(CTEST_SITE)
set(CTEST_BUILD_NAME "${CMAKE_SYSTEM}_${CMAKE_HOST_SYSTEM_PROCESSOR}")



SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)


CTEST_START("Nightly")
CTEST_UPDATE()
CTEST_CONFIGURE(OPTIONS "-DDASH_TESTING=ON -DBUILD_COVERAGE=ON -G \"${CTEST_CMAKE_GENERATOR}\"")
CTEST_BUILD()
CTEST_TEST()
if (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
  ctest_coverage()
endif (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
if (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
  ctest_memcheck()
endif (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND)
CTEST_SUBMIT()
