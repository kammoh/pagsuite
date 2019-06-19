cmake_minimum_required(VERSION 2.8)
project(paglib)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MACOSX_RPATH 1)

# Detect GMP / GMPXX
FIND_PATH(GMPXX_H
  gmpxx.h
  )

FIND_LIBRARY(GMP_LIB
  NAMES gmp gmp.lib
  DOC "Directory of the GMP library")

FIND_PATH(GMP_H
  gmp.h
  )

FIND_LIBRARY(GMPXX_LIB
  NAMES gmpxx gmpxx.lib
  DOC "Directory of the GMPXX library")

IF (GMPXX_H AND GMPXX_LIB AND GMP_H AND GMP_LIB)
  MESSAGE(STATUS "Found gmp and gmpxx: ${GMP_H},${GMP_LIB},${GMPXX_H},${GMPXX_LIB}")
  ADD_DEFINITIONS(-DHAVE_GMPXX)
ELSE (GMPXX_H AND GMPXX_LIB AND GMP_H AND GMP_LIB)
  MESSAGE(WARNING "Could not find gmp and/or gmpxx (found: ${GMP_H},${GMP_LIB},${GMPXX_H},${GMPXX_LIB}), paglib may fail for large constants > 64 bits")
ENDIF (GMPXX_H AND GMPXX_LIB AND GMP_H AND GMP_LIB)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/inc ${GMP_H} ${GMPXX_H})

SET(SRC_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/adder_graph.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vector_util.cpp
)

SET(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/pagsuite/adder_graph.h
)

ADD_LIBRARY(paglib SHARED ${SRC_FILES} ${HEADERS})

set_target_properties(paglib PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(paglib PROPERTIES OUTPUT_NAME "pag")

SET_TARGET_PROPERTIES(
paglib
PROPERTIES
	SOVERSION 0
	VERSION 2.1.0
)

IF (GMPXX_H AND GMPXX_LIB AND GMP_H AND GMP_LIB)
    target_link_libraries(paglib ${GMP_LIB} ${GMPXX_LIB})
ENDIF (GMPXX_H AND GMPXX_LIB AND GMP_H AND GMP_LIB)

INSTALL(FILES ${HEADERS}
    DESTINATION include/pagsuite
)

INSTALL(TARGETS paglib
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

