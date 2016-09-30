cmake_minimum_required(VERSION 2.8)
project(paglib_copa)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()
include_copa( ${PAGSUITE_ROOT_DIR} )

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -fPIC")

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/inc )

SET(SRC_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/paglib_copa
    ${CMAKE_CURRENT_SOURCE_DIR}/src/paglib_scopes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adder_graph
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vector_util
)

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/paglib_copa
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/paglib_scopes
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adder_graph
)

SET(HEADERS_EXPORT
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adder_graph.h
)
IF ( NOT STATIC_ONLY )
	ADD_LIBRARY( pag_lib 	    SHARED ${SRC_FILES} ${HEADERS})
	set_target_properties(pag_lib PROPERTIES LINKER_LANGUAGE CXX)
	target_link_libraries( pag_lib ${COPA} )
	target_include_directories(pag_lib PUBLIC inc)
	SET_TARGET_PROPERTIES(
	pag_lib
	PROPERTIES
		SOVERSION 0
		VERSION 2.1.0
	)
	INSTALL(TARGETS pag_lib
	  RUNTIME DESTINATION paglib/lib
	  LIBRARY DESTINATION paglib/lib
	  ARCHIVE DESTINATION paglib/lib
	)
ENDIF ( NOT STATIC_ONLY )

ADD_LIBRARY( pag_lib_static STATIC ${SRC_FILES} ${HEADERS})
set_target_properties(pag_lib_static PROPERTIES OUTPUT_NAME pag_lib)
target_link_libraries( pag_lib_static ${COPA_STATIC} )
target_include_directories(pag_lib_static PUBLIC inc)

INSTALL(FILES ${HEADERS_EXPORT}
    DESTINATION paglib/include
)

INSTALL(TARGETS pag_lib_static
  RUNTIME DESTINATION paglib/lib
  LIBRARY DESTINATION paglib/lib
  ARCHIVE DESTINATION paglib/lib
)
