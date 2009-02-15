# Find libinfinity
#
# This script will define:
#	LIBQINFINITY_FOUND
#	LIBQINFINITY_INCLUDES
#	LIBQINFINITY_LIBRARY
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if( LIBQINFINITY_LIBRARIES AND LIBQINFINITY_INCLUDES )
	
	set( LIBQINFINITY_FOUND true )

else( LIBQINFINITY_LIBRARIES AND LIBQINFINITY_INCLUDES )
	
	set( LIBQINFINITY_FOUND false )

	find_package(Libinfinity REQUIRED)
	find_package(Libinftext REQUIRED)
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(libqinfinity-1.0
			_LIBQINFINITY_INCLUDE_DIR
			_LIBQINFINITY_LIBRARY_DIR
			_LIBQINFINITY_LINKER_FLAGS
			_LIBQINFINITY_COMPILER_FLAGS
		)
	endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
	
	find_path( LIBQINFINITY_INCLUDE
		NAMES
			libqinfinity/qtio.h
		PATHS
			${_LIBQINFINITY_INCLUDE_DIR}
			/usr/include
			/usr/local/include
			/opt/local/include
		PATH_SUFFIXES
			libqinfinity-1.0
	)
	
	find_library( LIBQINFINITY_LIBRARY
		NAMES
			qinfinity
		PATHS
			${_LIBQINFINITY_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)

	set( LIBQINFINITY_INCLUDES ${LIBQINFINITY_INCLUDE} ${LIBINFINITY_INCLUDES} )
	set( LIBQINFINITY_LIBRARIES ${LIBQINFINITY_LIBRARY} ${LIBINFINITY_LIBRARIES} )

endif( LIBQINFINITY_LIBRARIES AND LIBQINFINITY_INCLUDES )
