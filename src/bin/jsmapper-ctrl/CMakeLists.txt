set( PRJNAME jsmapper-ctrl )

add_executable( ${PRJNAME} main.cpp )
target_link_libraries( ${PRJNAME} jsmapper )
install( TARGETS ${PRJNAME} RUNTIME DESTINATION bin )
