macro( configure )
	IF( NOT PAGSUITE_CONFIGURED )
		cmake_minimum_required(VERSION 2.6)
		if(COMMAND cmake_policy)
			cmake_policy(SET CMP0005 NEW)
			cmake_policy(SET CMP0003 NEW)
		endif(COMMAND cmake_policy)

		if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		    SET( BITNESS 64 )
		else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		    SET( BITNESS 32 )
		endif()

		IF( UNIX )
			 IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
				SET( SYSTEM_PREFIX "mac" )
				ADD_DEFINITIONS(-DOS_MAC)
			 ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
				SET( SYSTEM_PREFIX "lin" )
				ADD_DEFINITIONS(-DOS_LINUX)
			 ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		elseif( MINGW )
		    SET( SYSTEM_PREFIX "win" )
			ADD_DEFINITIONS(-DOS_WIN)
		else()
		    message( FATAL_ERROR "No supported system found, CMake will exit." )
		endif()

		SET( SYSTEM_NAME ${SYSTEM_PREFIX}${BITNESS} )
		message( "Building for " ${SYSTEM_NAME} )
		SET(PAGSUITE_CONFIGURED true CACHE INTERNAL "pagsuite_conf")

		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	ENDIF( NOT PAGSUITE_CONFIGURED )	

	STRING( TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
	IF (BUILD_TYPE STREQUAL "debug")
		message( "Now building ${PROJECT_NAME} in debug mode")
	ELSE (BUILD_TYPE STREQUAL "debug")
		message( "Now building ${PROJECT_NAME} in release mode")
		SET ( BUILD_TYPE "release" )
	ENDIF (BUILD_TYPE STREQUAL "debug")
endmacro( configure )

macro( include_copa PAGSUITE_ROOT_DIR )
	IF( NOT COPA_FOUND )
		find_library( COPA
			NAMES copa libcopa.so
			REQUIRED
			)
		
		find_library( COPA_STATIC
			NAMES libcopa.a
			REQUIRED
			)

		find_path( COPA_INCLUDE_DIRS NAMES copa/copa.h )
		include_directories( ${COPA_INCLUDE_DIRS} )
		IF ( NOT COPA )
			Message( FATAL_ERROR "Copa not found." )
		ELSE ( NOT COPA )
			Message( "Copa found at: ${COPA}")
			Message( "Copa static found at: ${COPA_STATIC}")
			Message( "Copa inc path found at: ${COPA_INCLUDE_DIRS}")
		ENDIF ( NOT COPA )

		SET(COPA_FOUND true CACHE INTERNAL "copa_found")
	ENDIF( NOT COPA_FOUND )
endmacro( include_copa )

macro( include_paglib PAGSUITE_ROOT_DIR )
	IF( NOT TARGET pag_lib )
		add_subdirectory( ${PAGSUITE_ROOT_DIR}/pag_lib/paglib_copa ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/paglib_copa)
	ENDIF( NOT TARGET pag_lib )
endmacro( include_paglib )
