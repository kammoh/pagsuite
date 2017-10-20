PROJECT( scm_opt CXX )
cmake_minimum_required(VERSION 2.4)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
configure()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc ${PAGSUITE_ROOT_DIR}/rpag/inc )

FILE( GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/compute_successor_set.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/rpag_functions.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/norm.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/log2_64.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/debug.cpp)
FILE( GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h)

ADD_EXECUTABLE( scm_opt ${SOURCES} ${HEADERS})

set_target_properties( scm_opt PROPERTIES LINKER_LANGUAGE CXX)
IF( ${SYSTEM_PREFIX} STREQUAL "lin" )
	TARGET_LINK_LIBRARIES( rpag rt )
ENDIF()

INSTALL(TARGETS scm_opt
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
