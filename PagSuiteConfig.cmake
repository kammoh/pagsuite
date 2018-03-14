macro( configure )
	IF( NOT PAGSUITE_CONFIGURED )
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
		SET(PAGSUITE_CONFIGURED true)

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
	IF( NOT COPA_INCLUDED )
		include_directories( ${PAGSUITE_ROOT_DIR}/libs/copa/include )
		IF( EXISTS ${PAGSUITE_ROOT_DIR}/libs/copa/lib/${SYSTEM_NAME} )
			link_directories( ${PAGSUITE_ROOT_DIR}/libs/copa/lib/${SYSTEM_NAME} )
		ELSE( EXISTS ${PAGSUITE_ROOT_DIR}/libs/copa/lib/${SYSTEM_NAME} )
			message( FATAL_ERROR "There is no copa binary for your system. (${PAGSUITE_ROOT_DIR}/libs/copa/lib/${SYSTEM_NAME})" )
		ENDIF( EXISTS ${PAGSUITE_ROOT_DIR}/libs/copa/lib/${SYSTEM_NAME} )
		SET(COPA_INCLUDED true)
	ENDIF( NOT COPA_INCLUDED )
endmacro( include_copa )

macro( include_paglib PAGSUITE_ROOT_DIR )
	IF( NOT TARGET pag_lib )
		include_copa( ${PAGSUITE_ROOT_DIR} )
		add_subdirectory( ${PAGSUITE_ROOT_DIR}/pag_lib/paglib_copa ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/paglib_copa)
		INCLUDE_DIRECTORIES( ${PAGSUITE_ROOT_DIR}/pag_lib/paglib_copa/inc )
	ENDIF( NOT TARGET pag_lib )
endmacro( include_paglib )
