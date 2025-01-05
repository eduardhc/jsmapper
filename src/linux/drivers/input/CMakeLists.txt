# Build the module

Set( KERNEL_DIR 		"/lib/modules/${CMAKE_SYSTEM_VERSION}/build" )
Set( MODULE_INSTALL_DIR "/lib/modules/${CMAKE_SYSTEM_VERSION}/kernel/drivers/input" )

Set( kbuild_cmd ${CMAKE_MAKE_PROGRAM}
                -C ${KERNEL_DIR}
                M=${CMAKE_CURRENT_SOURCE_DIR} modules) 

Set( kclean_cmd ${CMAKE_MAKE_PROGRAM}
                -C ${KERNEL_DIR}
                M=${CMAKE_CURRENT_SOURCE_DIR} clean ) 

set ( kofile ${CMAKE_CURRENT_SOURCE_DIR}/jsmapperdev.ko )

set ( ksrc 
		jsmapper_api.h
		jsmapper_log.h
		jsmapper_core.h 
		jsmapper_core.c 
		jsmapper_evgen.h 
		jsmapper_evgen.c 
		jsmapper_main.c 
		Kbuild )

set ( ktmp
		${CMAKE_CURRENT_SOURCE_DIR}/jsmapperdev.o
		${CMAKE_CURRENT_SOURCE_DIR}/jsmapper_main.o
		${CMAKE_CURRENT_SOURCE_DIR}/jsmapper_evgen.o
		${CMAKE_CURRENT_SOURCE_DIR}/jsmapper_core.o
		${CMAKE_CURRENT_SOURCE_DIR}/.jsmapperdev.o.cmd
		${CMAKE_CURRENT_SOURCE_DIR}/.jsmapper_main.o.cmd
		${CMAKE_CURRENT_SOURCE_DIR}/.jsmapper_core.o.cmd
		${CMAKE_CURRENT_SOURCE_DIR}/.jsmapper_evgen.o.cmd
		${CMAKE_CURRENT_SOURCE_DIR}/jsmapperdev.mod.c
		${CMAKE_CURRENT_SOURCE_DIR}/jsmapperdev.mod.o
		${CMAKE_CURRENT_SOURCE_DIR}/.jsmapperdev.mod.o.cmd
		${CMAKE_CURRENT_SOURCE_DIR}/.jsmapperdev.ko.cmd
		${CMAKE_CURRENT_SOURCE_DIR}/.tmp_versions
		${CMAKE_CURRENT_SOURCE_DIR}/Module.symvers
		${CMAKE_CURRENT_SOURCE_DIR}/modules.order )

add_custom_command ( OUTPUT ${kofile} ${ktmp}
						COMMAND ${kbuild_cmd}
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						DEPENDS ${ksrc}
						COMMENT "Building kernel module..."
						VERBATIM )

add_custom_target ( jsmapperdev ALL DEPENDS ${kofile} )


# Installation of the module
install( FILES jsmapperdev.ko
         DESTINATION ${MODULE_INSTALL_DIR}
         PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
