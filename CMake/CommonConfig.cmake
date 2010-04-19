#------------------------------------------------------------------------------

if( NOT COMMON_CONFIG_HAS_BEEN_RUN_BEFORE)

	# Configure to build universal binaries.  Build 32-bit Intel/PPC on 10.4 and
	# 32/64-bit Intel/PPC on >= 10.5
	
	if( APPLE AND NOT NON_NATIVE_TARGET)
		if( NOT OSX_CONFIG_HAS_BEEN_RUN_BEFORE)
		
			# Make sure the version of CMake is compatible with this script
		
			if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 
				AND ${CMAKE_PATCH_VERSION} LESS 7)
				message( STATUS
							"Warning: A critical CMake bug exists in 2.4.6 and below.  "
							"Trying to build Universal Binaries will result in a compile "
							"error that seems unrelated.  Either avoid building Universal "
							"Binaries by changing the CMAKE_OSX_ARCHITECTURES field to list "
							"only your architecture, or upgrade to a newer version of CMake.")
			endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 
				AND ${CMAKE_PATCH_VERSION} LESS 7)
				
			# Determine the correct SDK
				
			if("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
				if( EXISTS /Developer/SDKs/MacOSX10.6.sdk OR EXISTS /Developer/SDKs/MacOSX10.5.sdk)
					if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
						set( CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for OSX" FORCE)
					else( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
						set( CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
					endif( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
				else( EXISTS /Developer/SDKs/MacOSX10.6.sdk OR EXISTS /Developer/SDKs/MacOSX10.5.sdk)
					if( EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
						set( CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING "Build architectures for OSX" FORCE)
					else( EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
						# No Universal Binary support
					endif( EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
				endif( EXISTS /Developer/SDKs/MacOSX10.6.sdk OR EXISTS /Developer/SDKs/MacOSX10.5.sdk)
			endif("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
			set( OSX_CONFIG_HAS_BEEN_RUN_BEFORE TRUE)
		endif( NOT OSX_CONFIG_HAS_BEEN_RUN_BEFORE)
	endif( APPLE AND NOT NON_NATIVE_TARGET)
endif( NOT COMMON_CONFIG_HAS_BEEN_RUN_BEFORE)
