cmake_minimum_required(VERSION 2.8)

if (CMAKE_BUILD_TYPE STREQUAL "debug")
        message( "Now building in debug mode")
        SET( TARGETNAME paglib_copad )
else (CMAKE_BUILD_TYPE STREQUAL "debug")
message( "Now building in release mode")
        SET( TARGETNAME paglib_copa )
        SET ( CMAKE_BUILD_TYPE "release" )
endif (CMAKE_BUILD_TYPE STREQUAL "debug")

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")

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

ADD_LIBRARY( pag_lib_static STATIC ${SRC_FILES} ${HEADERS})
ADD_LIBRARY( pag_lib SHARED ${SRC_FILES} ${HEADERS})

set_target_properties(pag_lib_static PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(pag_lib PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries( pag_lib_static copa )
target_link_libraries( pag_lib copa )

set_target_properties(pag_lib_static PROPERTIES OUTPUT_NAME ${TARGETNAME})
set_target_properties(pag_lib PROPERTIES OUTPUT_NAME ${TARGETNAME})

INSTALL(FILES ${HEADERS_EXPORT}
    DESTINATION paglib/include
)

INSTALL(TARGETS pag_lib_static pag_lib
  RUNTIME DESTINATION paglib/bin
  LIBRARY DESTINATION paglib/lib
  ARCHIVE DESTINATION paglib/lib
)
