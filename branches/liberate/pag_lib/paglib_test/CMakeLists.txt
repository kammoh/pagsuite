cmake_minimum_required(VERSION 2.8)
project(paglib_test)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()
include_copa( ${PAGSUITE_ROOT_DIR} )
include_paglib( ${PAGSUITE_ROOT_DIR} )

SET(SRC_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/main
)

add_executable( paglib_test ${SRC_FILES} )
target_link_libraries ( paglib_test pag_lib_static ${COPA_STATIC} )
set_target_properties(paglib_test PROPERTIES LINKER_LANGUAGE CXX)

INSTALL(TARGETS paglib_test
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
