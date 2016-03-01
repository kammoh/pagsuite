cmake_minimum_required(VERSION 2.8)
project(pag_tools CXX)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()
include_paglib( ${PAGSUITE_ROOT_DIR} )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/pag_lib/paglib_test ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/pag_print )
set_target_properties(paglib_test PROPERTIES OUTPUT_NAME pag_print)
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/pag_fusion ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/pag_fusion)
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/pag_split ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/pag_split)
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/rpag ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/rpag)

IF( BUILD_FLOPOCO )
	add_subdirectory( ${BUILD_FLOPOCO} ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/flopoco)
ENDIF( BUILD_FLOPOCO )
