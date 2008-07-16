# Find glib
#
# This script will define:
#	GLIB_FOUND
#	GLIB_INCLUDES
#	GLIB_LIBRARY
#	GLIB_LIBRARIES
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if( GLIB_LIBRARIES AND GLIB_INCLUDES )
	
	set( GLIB_FOUND true )
	
else( GLIB_LIBRARIES AND GLIB_INCLUDES )
	
	set( GLIB_FOUND false )
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(glib-2.0
			_GLIB_INCLUDE_DIR
			_GLIB_LIBRARY_DIR
			_GLIB_LINKER_FLAGS
			_GLIB_COMPILER_FLAGS
		)
	endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
	
	find_path( GLIB_INCLUDE_DIR
		NAMES
			glib.h
		PATHS
			${_GLIB_INCLUDE_DIR}
			/usr/include
			/usr/local/include
			/opt/local/include
		PATH_SUFFIXES
			glib-2.0
	)
	
	find_path( GLIB_LIB_INCLUDE_DIR
		NAMES
			glibconfig.h
		PATHS
			${_GLIB_INCLUDE_DIR}
			/usr/lib
			/usr/local/lib
		PATH_SUFFIXES
			glib-2.0/include
	)
	
	find_library( GLIB_LIBRARY
		NAMES
			glib-2.0
		PATHS
			${_GLIB_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)
	
	set( GLIB_INCLUDES
		${GLIB_INCLUDE_DIR}
		${GLIB_LIB_INCLUDE_DIR}
	)
	
	set( GLIB_LIBRARIES
		${GLIB_LIBRARY}
	)
	
	set( GLIB_FOUND true )
	
endif( GLIB_LIBRARIES AND GLIB_INCLUDES )
