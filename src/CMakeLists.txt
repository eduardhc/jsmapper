cmake_minimum_required( VERSION 3.5 )   # for CMAKE_CURRENT_SOURCE_DIR

project( jsmapper )

# version info (1.1.0)
set( VERSION_MAJOR 1 )
set( VERSION_MINOR 1 )


# create configuration file:
set( JSMAPPER_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" )

set( CONFIG_FILE_TEMPL     ${CMAKE_SOURCE_DIR}/jsmapper_config.h.cmake )
set( CONFIG_FILE           ${CMAKE_BINARY_DIR}/jsmapper_config.h )
configure_file( ${CONFIG_FILE_TEMPL} ${CONFIG_FILE} )
message( STATUS "Created configuration file " ${CONFIG_FILE} )


include_directories( ${CMAKE_SOURCE_DIR} )
include_directories( ${CMAKE_BINARY_DIR} )  # for config file

# ensure libxml2, etc... are available:
find_package( LibXml2 REQUIRED )
include_directories( ${LIBXML2_INCLUDE_DIR} )

find_package( Threads REQUIRED )



# subprojects:
option(WITH_KMOD "Builds kernel module" ON)
if(WITH_KMOD)
	message( STATUS "Including kernel module" )
	add_subdirectory( linux/drivers/input )
else()
	message( STATUS "NOT Including kernel module" )
endif()

add_subdirectory( lib )

include_directories( ${CMAKE_SOURCE_DIR}/lib )
add_subdirectory( bin )

ENABLE_TESTING()
add_subdirectory( test )

add_subdirectory( share )


# Python bindings (experimental...):
# find_package( PythonInterp REQUIRED )
# find_package( PythonLibs REQUIRED )
# find_package( SIP REQUIRED )

# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
