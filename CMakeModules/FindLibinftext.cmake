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

if( LIBINFTEXT_LIBRARIES AND LIBINFTEXT_INCLUDES )
	
	set( LIBINFTEXT_FOUND true )

else( LIBINFTEXT_LIBRARIES AND LIBINFTEXT_INCLUDES )
	
	set( LIBINFTEXT_FOUND false )
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(libinftext-0.3
			_LIBINFTEXT_INCLUDE_DIR
			_LIBINFTEXT_LIBRARY_DIR
			_LIBINFTEXT_LINKER_FLAGS
			_LIBINFTEXT_COMPILER_FLAGS
		)
	endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
	
	find_path( LIBINFTEXT_INCLUDES
		NAMES
			libinftext/inf-text-session.h
		PATHS
			${_LIBINFTEXT_INCLUDE_DIR}
			/usr/include
			/usr/local/include
			/opt/local/include
		PATH_SUFFIXES
			libinftext-0.3
	)
	
	find_library( LIBINFTEXT_LIBRARY
		NAMES
			inftext-0.3
		PATHS
			${_LIBINFTEXT_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)

	set(LIBINFTEXT_LIBRARIES ${LIBINFTEXT_LIBRARY})

endif( LIBINFTEXT_LIBRARIES AND LIBINFTEXT_INCLUDES )
