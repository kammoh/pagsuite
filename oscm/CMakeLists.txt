cmake_minimum_required (VERSION 2.8)
project (oscm)

FIND_LIBRARY(SCALP_LIB
	NAMES libILP libILP.dylib
	HINTS "${SCALP_PREFIX_PATH}/lib"
	DOC "Directory of the ScaLP library")

FIND_PATH(SCALP_H
	  ILP/Solver.h
	  HINTS "${SCALP_PREFIX_PATH}/include"
	  DOC "Path to main ScaLP header file Solver.h"
)

IF (SCALP_LIB AND SCALP_H)
    MESSAGE(STATUS "Found ScaLP: ${SCALP_LIB},${SCALP_H}")
ELSE (SCALP_LIB AND SCALP_H)
    MESSAGE(FATAL_ERROR "Could not find ScaLP. Please specify path by setting SCALP_PREFIX_PATH (-DSCALP_PREFIX_PATH=<path to scalp install dir>)")
ENDIF (SCALP_LIB AND SCALP_H)

link_directories(/Users/pluto/research/develop/inst/lib)

add_executable(oscm oscm.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/Users/pluto/research/develop/inst/include)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /Users/pluto/research/develop/inst/lib)


target_link_libraries(oscm ILP ILP-Dynamic)

