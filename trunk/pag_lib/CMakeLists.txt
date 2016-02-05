cmake_minimum_required(VERSION 2.8)
project(paglib)

#SET ( CMAKE_BUILD_TYPE "debug" )
#SET ( CMAKE_BUILD_TYPE "release" )

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( BITNESS 64 )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( BITNESS 32 )
endif()

IF( UNIX )
    SET( SYSTEM_PREFIX "lin" )
elseif( MINGW )
    SET( SYSTEM_PREFIX "win" )
elseif( APPLE )
    SET( SYSTEM_PREFIX "mac" )
else()
    message( FATAL_ERROR "No supported system found, CMake will exit." )
endif()

SET( SYSTEM_NAME ${SYSTEM_PREFIX}${BITNESS} )
message( "Building for " ${SYSTEM_NAME} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/copa/include )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/copa/lib/${SYSTEM_NAME} )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${SYSTEM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${SYSTEM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${SYSTEM_NAME})

add_subdirectory(paglib_copa)

include_directories( ${CMAKE_SOURCE_DIR}/paglib_copa/inc )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_subdirectory(paglib_test)
