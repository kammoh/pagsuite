PROJECT( rpag CXX )
cmake_minimum_required(VERSION 2.4)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

SET( PAGSUITE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inc )

FILE( GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
FILE( GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h ${CMAKE_CURRENT_SOURCE_DIR}/inc/pagsuite/*.h)

ADD_LIBRARY( rpaglib SHARED ${SOURCES} ${HEADERS})
set_target_properties(rpaglib PROPERTIES OUTPUT_NAME "rpag")

ADD_EXECUTABLE( rpag ${SOURCES} ${HEADERS})
set_target_properties( rpag PROPERTIES LINKER_LANGUAGE CXX)

#ToDo: search for the following libraries to allow runtime measurements as well as multithreading (currently only used for large CMM problems)
#TARGET_LINK_LIBRARIES( rpag rt pthread)

INSTALL(TARGETS rpag
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

INSTALL(FILES ${HEADERS}
    DESTINATION include/pagsuite
)

INSTALL(TARGETS rpaglib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
