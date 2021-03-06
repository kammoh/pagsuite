PROJECT( oscm CXX )
cmake_minimum_required(VERSION 2.4)

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )

#include(${PAGSUITE_ROOT_DIR}/PagSuiteConfig.cmake)
#configure()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc ${PAGSUITE_ROOT_DIR}/rpag/inc )

FILE( GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/compute_successor_set.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/rpag_functions.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/norm.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/log2_64.cpp ${PAGSUITE_ROOT_DIR}/rpag/src/debug.cpp)
FILE( GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/pagsuite/*.hpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

ADD_LIBRARY( oscmlib SHARED ${SOURCES} ${HEADERS})
set_target_properties(oscmlib PROPERTIES OUTPUT_NAME "oscm")

ADD_EXECUTABLE( oscm ${SOURCES} ${HEADERS})


INSTALL(FILES ${HEADERS}
  DESTINATION include/pagsuite
)

INSTALL(TARGETS oscmlib
		RUNTIME DESTINATION lib
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)

INSTALL(TARGETS oscm
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
