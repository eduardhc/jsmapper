set( _NAME jsmapper-studio )

find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

# add_subdirectory(icons)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( _SRCS
		app.cpp
		main.cpp 
		mainwindow.cpp
		newprofiledlg.cpp 
		actions/actionsmodel.cpp
		actions/actionsview.cpp )

set( _MOCS
		app.h
		mainwindow.h
		newprofiledlg.h
		actions/actionsmodel.h
		actions/actionsview.h )

set( _UIS 
		mainwindow.ui 
		newprofiledlg.ui 
		actions/actionsview.ui )


add_executable( ${_NAME} 
					${_SRCS} 
					${_MOCS} 
					${_UIS} )

target_link_libraries( ${_NAME}
							jsmapper
							${QT_LIBRARIES} )

install( TARGETS ${_NAME} RUNTIME DESTINATION bin )
