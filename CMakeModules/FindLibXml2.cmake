# Find libxml2
#
# This script will define:
#	LIBXML2_FOUND
#	LIBXML2_INCLUDES
#	LIBXML2_LIBRARY
#	LIBXML2_LIBRARIES
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if( LIBXML2_LIBRARIES AND LIBXML2_INCLUDES )
	
	set( LIBXML2_FOUND true )

else( LIBXML2_LIBRARIES AND LIBXML2_INCLUDES )
	
	set( LIBXML2_FOUND false )
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(libinfinitymm-1.0
			_LIBXML2_INCLUDE_DIR
			_LIBXML2_LIBRARY_DIR
			_LIBXML2_LINKER_FLAGS
			_LIBXML2_COMPILER_FLAGS
		)
	endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
	
	find_path( LIBXML2_INCLUDE_DIR
		NAMES
			libxml/xmlIO.h
		PATHS
			${_LIBXML2_INCLUDE_DIR}
			/usr/include
			/usr/local/include
			/opt/local/include
		PATH_SUFFIXES
			libxml2
	)
	
	find_library( LIBXML2_LIBRARY
		NAMES
			xml2
		PATHS
			${_LIBXML2_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)
	
	set( LIBXML2_INCLUDES
		${LIBXML2_INCLUDE_DIR}
	)
	
	set( LIBXML2_LIBRARIES
		${LIBXML2_LIBRARY}
	)

endif( LIBXML2_LIBRARIES AND LIBXML2_INCLUDES )
