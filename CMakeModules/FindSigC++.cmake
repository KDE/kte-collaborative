# Find sigc++
#
# This script will define:
#	SIGCPP_FOUND
#	SIGCPP_INCLUDES
#	SIGCPP_LIBRARY
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if( SIGCPP_LIBRARIES AND SIGCPP_INCLUDES )
	
	set( SIGCPP_FOUND true )
	
else ( SIGCPP_LIBRARIES AND SIGCPP_INCLUDES )
	
	set( SIGCPP_FOUND false )
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(sigc++-2.0
			_SIGCPP_INCLUDE_DIR
			_SIGCPP_LIBRARY_DIR
			_SIGCPP_LINKER_FLAGS
			_SIGCPP_COMPILER_FLAGS
		)
	endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
	
	find_path( SIGCPP_INCLUDE_DIR
		NAMES
			sigc++/sigc++.h
		PATHS
			${_SIGCPP_INCLUDE_DIR}
			/usr/include
			/usr/local/include
			/opt/local/include
		PATH_SUFFIXES
			sigc++-2.0
	)
	
	find_path( SIGCPP_LIB_INCLUDE_DIR
		NAMES
			sigc++config.h
		PATHS
			${_SIGCPP_LIBRARY_DIR}
			/usr/lib
		PATH_SUFFIXES
			sigc++-2.0/include
	)
	
	find_library( SIGCPP_LIBRARY
		NAMES
			sigc-2.0
		PATHS
			${_SIGCPP_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)
	
	set( SIGCPP_INCLUDES
		${SIGCPP_INCLUDE_DIR}
		${SIGCPP_LIB_INCLUDE_DIR}
	)
	
	set( SIGCPP_LIBRARIES
		${SIGCPP_LIBRARY}
	)

endif( SIGCPP_LIBRARIES AND SIGCPP_INCLUDES )
