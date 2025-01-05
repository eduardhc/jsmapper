# try to determine lib install folder:
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set( LIB_INSTALL_DIR lib )
  else(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set( LIB_INSTALL_DIR lib64 )
endif()


add_subdirectory( jsmapper )
