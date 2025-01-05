set( PRJNAME jsmapper )

set( SOURCES 
	action.cpp
	axisaction.cpp
	buttonaction.cpp
	common.cpp
	condition.cpp
	device.cpp
	devicemap.cpp
	keyaction.cpp
	keymap.cpp
	log.cpp
	macroaction.cpp
	mode.cpp
	monitor.cpp
	nullaction.cpp
	profile.cpp
	xmlhelpers.cpp
)

set( HEADERS
	action.h
	axisaction.h
	band.h
	buttonaction.h
	common.h
	condition.h
	device.h
	devicemap.h
	keyaction.h
	keymap.h
	log.h
	macroaction.h
	mode.h
	monitor.h
	nullaction.h
	profile.h
	xmlhelpers.h
)

add_library( ${PRJNAME} SHARED ${SOURCES} )
target_link_libraries( ${PRJNAME}
							${LIBXML2_LIBRARIES}
							${CMAKE_THREAD_LIBS_INIT}
							udev )

set_target_properties( ${PRJNAME} PROPERTIES
						VERSION ${VERSION_MAJOR}.${VERSION_MINOR} 
						SOVERSION ${VERSION_MAJOR} )

install( TARGETS ${PRJNAME} 
			LIBRARY DESTINATION ${LIB_INSTALL_DIR} )
install( FILES ${HEADERS} 
			DESTINATION include/jsmapper )

# Installation of module API header: 
#     Doing it here avoids having to create a -devel package for kernel
# module, which is kinda complicated to do (at least, in Fedora).
install( FILES ${CMAKE_SOURCE_DIR}/linux/drivers/input/jsmapper_api.h 
			DESTINATION include/jsmapper )
