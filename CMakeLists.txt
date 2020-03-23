cmake_minimum_required(VERSION 3.4)
project(pagsuite CXX)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} )

set(CMAKE_MACOSX_RPATH 1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory( ${PAGSUITE_ROOT_DIR}/omcm CMakeTmp/omcm)
add_subdirectory( ${PAGSUITE_ROOT_DIR}/paglib CMakeTmp/paglib)
add_subdirectory( ${PAGSUITE_ROOT_DIR}/oscm CMakeTmp/oscm)
add_subdirectory( ${PAGSUITE_ROOT_DIR}/rpag CMakeTmp/rpag)
