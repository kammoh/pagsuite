cmake_minimum_required (VERSION 2.8)
project (omcm)

FIND_LIBRARY(SCALP_LIB
        NAMES libScaLP libScaLP.so libScaLP.dylib
	HINTS "${SCALP_PREFIX_PATH}/lib"
	DOC "Directory of the ScaLP library")

FIND_PATH(SCALP_H
          ScaLP/Solver.h
	  HINTS "${SCALP_PREFIX_PATH}/include"
	  DOC "Path to main ScaLP header file Solver.h"
)

IF (SCALP_H AND SCALP_LIB)
    MESSAGE(STATUS "Found ScaLP: ${SCALP_H},${SCALP_LIB}")
ELSE (SCALP_H AND SCALP_LIB)
    MESSAGE(FATAL_ERROR "Could not find ScaLP (${SCALP_H},${SCALP_LIB}). Please specify path by setting SCALP_PREFIX_PATH (-DSCALP_PREFIX_PATH=<path to scalp install dir>)")
ENDIF (SCALP_H AND SCALP_LIB)

#link_directories(${SCALP_LIB})

SET(SOURCES omcm.cpp)

#ToDo: Separate omcm code from main to make it available as library
#ADD_LIBRARY( omcmlib SHARED ${SOURCES} ${HEADERS})
#set_target_properties(omcmlib PROPERTIES OUTPUT_NAME "omcm")

add_executable(omcm ${SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${SCALP_H})

target_link_libraries(omcm ${SCALP_LIB})

INSTALL(FILES ${HEADERS}
		DESTINATION include/pagsuite
		)

#INSTALL(TARGETS omcmlib
#		RUNTIME DESTINATION lib
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib
#		)

INSTALL(TARGETS omcm
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)