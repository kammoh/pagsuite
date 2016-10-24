PROJECT( rpag_exe CXX )

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
SET( STATIC_ONLY 1 )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()

IF( NOT TARGET rpag_static )
		add_subdirectory( ${PAGSUITE_ROOT_DIR}/rpag/librpag ${CMAKE_CURRENT_SOURCE_DIR}/CMakeTmp/librpag)
ENDIF( NOT TARGET rpag_static )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc )

FILE( GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE( GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h)

ADD_EXECUTABLE( rpag_exe ${SOURCES} ${HEADERS})
set_target_properties( rpag_exe PROPERTIES OUTPUT_NAME rpag)
set_target_properties( rpag_exe PROPERTIES LINKER_LANGUAGE CXX)
IF( ${SYSTEM_PREFIX} STREQUAL "lin" )
	TARGET_LINK_LIBRARIES( rpag_exe rt )
ENDIF()

TARGET_LINK_LIBRARIES( rpag_exe rpag_static )

INSTALL(TARGETS rpag_exe
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
