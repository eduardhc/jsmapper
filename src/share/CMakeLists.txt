# install device files:
install( DIRECTORY ./devices DESTINATION  
		${CMAKE_INSTALL_PREFIX}/share/jsmapper 
		PATTERN "*~" EXCLUDE )
