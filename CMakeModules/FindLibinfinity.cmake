# Find libinfinity
#
# This script will define:
#	LIBINFINITY_FOUND
#	LIBINFINITY_INCLUDES
#	LIBINFINITY_LIBRARY
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if( LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES )
	
	set( LIBINFINITY_FOUND true )

else( LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES )
	
	set( LIBINFINITY_FOUND false )

	find_package(Glib2 REQUIRED)
	find_package(GObject REQUIRED)
	find_package(GSASL REQUIRED)
	find_package(LibXml2 REQUIRED)
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(libinfinity-0.5
			_LIBINFINITY_INCLUDE_DIR
			_LIBINFINITY_LIBRARY_DIR
			_LIBINFINITY_LINKER_FLAGS
			_LIBINFINITY_COMPILER_FLAGS
		)
	endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
	
	find_path( LIBINFINITY_INCLUDES
		NAMES
			libinfinity/inf-config.h
		PATHS
			${_LIBINFINITY_INCLUDE_DIR}
			/usr/include
			/usr/local/include
			/opt/local/include
		PATH_SUFFIXES
			libinfinity-0.5
	)
	
	find_library( LIBINFINITY_LIBRARY
		NAMES
			infinity-0.5
		PATHS
			${_LIBINFINITY_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
			/usr/lib${LIB_SUFFIX}
			/usr/local/lib${LIB_SUFFIX}
	)

	set( LIBINFINITY_INCLUDES
		${LIBINFINITY_INCLUDES}
		${GLIB2_INCLUDE_DIR}
		${GOBJECT_INCLUDE_DIR}
		${GSASL_INCLUDE_DIR}
		${LIBXML2_INCLUDES} )
	set( LIBINFINITY_LIBRARIES
		${LIBINFINITY_LIBRARY}
		${GOBJECT_LIBRARIES}
		${GSASL_LIBRARIES}
		${LIBXML2_LIBRARIES} )

endif( LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES )
