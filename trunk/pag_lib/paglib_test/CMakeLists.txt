set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../paglib_copa/inc )

SET(SRC_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/main
)

add_executable( paglib_test ${SRC_FILES} )
target_link_libraries ( paglib_test pag_lib copa )
set_target_properties(paglib_test PROPERTIES LINKER_LANGUAGE CXX)
