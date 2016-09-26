cmake_minimum_required(VERSION 2.8)
project(paglib)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()
include_copa( ${PAGSUITE_ROOT_DIR} )

add_subdirectory(paglib_copa)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/paglib_copa/inc )
add_subdirectory(paglib_test)
