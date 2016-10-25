cmake_minimum_required(VERSION 2.8)
PROJECT( rpaglib CXX )

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()
include_paglib( ${PAGSUITE_ROOT_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc )

FILE( GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE( GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h)

SET( HEADERS_EXPORT 
	${HEADERS}
)

IF (BUILD_TYPE STREQUAL "debug")
	add_definitions( -DRPAG_DEBUG )
ENDIF (BUILD_TYPE STREQUAL "debug")

set(RPAG_VERSION_MAJOR 1)
set(RPAG_VERSION_MINOR 5)
set(RPAG_VERSION_PATCH 0)
set(RPAG_VERSION ${RPAG_VERSION_MAJOR}.${RPAG_VERSION_MINOR}.${RPAG_VERSION_PATCH})
add_definitions( -DRPAG_VERSION="\\"${RPAG_VERSION}\\"" )

IF ( NOT STATIC_ONLY )
	add_library( rpag        SHARED ${SOURCES} ${HEADERS})
	target_link_libraries( rpag pag_lib )
	
	target_include_directories(rpag PUBLIC inc)
	set_target_properties(rpag PROPERTIES 
	VERSION ${RPAG_VERSION}
	SOVERSION ${RPAG_VERSION_MAJOR}
	)
	INSTALL(TARGETS rpag
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	)
ENDIF ( NOT STATIC_ONLY )

add_library( rpag_static STATIC ${SOURCES} ${HEADERS})
target_link_libraries( rpag_static pag_lib_static)
target_include_directories(rpag_static PUBLIC inc)
set_target_properties(rpag_static PROPERTIES OUTPUT_NAME rpag)

INSTALL(FILES ${HEADERS_EXPORT}
    DESTINATION include/rpag
)

INSTALL(TARGETS rpag_static
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
