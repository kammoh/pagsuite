cmake_minimum_required(VERSION 2.8)
project(rpag CXX)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()
include_paglib( ${PAGSUITE_ROOT_DIR} )

add_subdirectory(librpag)
add_subdirectory(rpag)
