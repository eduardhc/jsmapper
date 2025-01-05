set( _NAME jsmapper-chooser-kde )

find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

find_package( KDE4 REQUIRED )
include( KDE4Defaults )

add_subdirectory(icons)

set( _SRCS
		actions.cpp
		itemdelegate.cpp
		model.cpp
		main.cpp 
		maindialog.cpp
		monitorclient.cpp
		settings.cpp
		systray.cpp )
		
kde4_add_ui_files( _SRCS
		maindialog.ui )

qt_wrap_cpp( _SRCS
		actions.h
		itemdelegate.h
		model.h
		maindialog.h
		systray.h )


add_definitions( ${QT_DEFINITIONS}
					${KDE4_DEFINITIONS} )

include_directories( ${KDE4_INCLUDES}
						${CMAKE_CURRENT_BINARY_DIR} )


kde4_add_executable( ${_NAME} ${_SRCS} )
target_link_libraries( ${_NAME}
							jsmapper
							${QT_LIBRARIES}
							${KDE4_KDEUI_LIBS}
							${KDE4_KIO_LIBS}
							${KDE4_KNOTIFYCONFIG_LIBS} )

install( TARGETS ${_NAME} ${INSTALL_TARGETS_DEFAULT_ARGS} )

