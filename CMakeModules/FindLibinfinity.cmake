# Find libinfinity
#
# This script will define:
#	LIBINFINITY_FOUND
#	LIBINFINITY_INCLUDE_DIR
#	LIBINFINITY_LIBRARY
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES)
	# Already found
	set( LIBINFINITY_FIND_QUIETLY true )
endif(LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES)

find_package(Glib2 REQUIRED)
find_package(GObject REQUIRED)
find_package(GSASL REQUIRED)
find_package(LibXml2 REQUIRED)
	
if(NOT WIN32)
	include(UsePkgConfig)
	pkgconfig(libinfinity-0.3
		_LIBINFINITY_INCLUDE_DIR
		_LIBINFINITY_LIBRARY_DIR
		_LIBINFINITY_LINKER_FLAGS
		_LIBINFINITY_COMPILER_FLAGS
	)
endif(NOT WIN32)

find_path( LIBINFINITY_INCLUDES
	NAMES
		libinfinity/inf-config.h
	PATHS
		${_LIBINFINITY_INCLUDE_DIR}
		/usr/include
		/usr/local/include
		/opt/local/include
	PATH_SUFFIXES
		libinfinity-0.3
)

find_library( LIBINFINITY_LIBRARY
	NAMES
		infinity-0.3
	PATHS
		${_LIBINFINITY_LIBRARY_DIR}
		/usr/lib
		/usr/local/lib
		/opt/local/lib
)

set( LIBINFINITY_INCLUDES
	${LIBINFINITY_INCLUDES}
	${GLIB2_INCLUDE_DIR}
	${GOBJECT_INCLUDE_DIR}
	${GSASL_INCLUDE_DIR}
	${LIBXML2_INCLUDES} )
set( LIBINFINITY_LIBRARIES
	${LIBINFINITY_LIBRARY}
	${GLIB2_LIBRARIES}
	${GOBJECT_LIBRARIES}
	${GSASL_LIBRARIES}
	${LIBXML2_LIBRARIES} )
