PROJECT( pag_fusion CXX )
cmake_minimum_required(VERSION 2.4)

#add_definitions(-std=c++11)

if (CMAKE_BUILD_TYPE STREQUAL "debug")
	message( "Now building in debug mode")
else (CMAKE_BUILD_TYPE STREQUAL "debug")
	message( "Now building in release mode")
	SET ( CMAKE_BUILD_TYPE "release" )
endif (CMAKE_BUILD_TYPE STREQUAL "debug")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( BITNESS 64 )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( BITNESS 32 )
endif()

IF( UNIX )
    SET( SYSTEM_PREFIX "lin" )
ADD_DEFINITIONS(-DOS_LINUX)
elseif( MINGW )
    SET( SYSTEM_PREFIX "win" )
ADD_DEFINITIONS(-DOS_WIN)
elseif( APPLE )
    SET( SYSTEM_PREFIX "mac" )
ADD_DEFINITIONS(-DOS_MAC)
else()
    message( FATAL_ERROR "No supported system found, CMake will exit." )
endif()
SET( SYSTEM_NAME ${SYSTEM_PREFIX}${BITNESS} )
message( "Building for " ${SYSTEM_NAME} )


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/copa/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/paglib/include )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/copa/lib/${SYSTEM_NAME} )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libs/paglib/lib/${SYSTEM_NAME} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc )

FILE( GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
#SET( 	SOURCES )

FILE( GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h)
#SET( 	HEADERS )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE( pag_fusion ${SOURCES} ${HEADERS})

set_target_properties( pag_fusion PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES( pag_fusion paglib_copa copa )
IF( ${SYSTEM_PREFIX} STREQUAL "lin" )
TARGET_LINK_LIBRARIES( pag_fusion rt )
ENDIF()

