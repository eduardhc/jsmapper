set( PRJNAME jsmapper-device )

# ensure ncurses is available:
find_package( Curses REQUIRED )
include_directories( ${CURSES_INCLUDE_DIR} )

add_executable( ${PRJNAME} main.cpp )
target_link_libraries( ${PRJNAME} jsmapper ${CURSES_LIBRARIES} )
install( TARGETS ${PRJNAME} RUNTIME DESTINATION bin )
