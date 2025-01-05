set( NAME jsmapper-test-action )

add_executable( ${NAME} main.cpp )
target_link_libraries( ${NAME} jsmapper gtest )

add_test( ${NAME} ${CMAKE_CURRENT_BINARY_DIR}/${NAME} )
