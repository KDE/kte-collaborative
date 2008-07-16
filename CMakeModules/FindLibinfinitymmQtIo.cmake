# Find libinfinitymm-qtio
#
# This script will define:
#	LIBINFINITYMM_QTIO_FOUND
#	LIBINFINITYMM_QTIO_INCLUDES
#	LIBINFINITYMM_QTIO_LIBRARY
#	LIBINFINITYMM_QTIO_LIBRARIES
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if( LIBINFINITYMM_QTIO_LIBRARIES AND LIBINFINITYMM_QTIO_INCLUDES )
	
	set( LIBINFINITYMM_QTIO_FOUND true )
	
else( LIBINFINITYMM_QTIO_LIBRARIES AND LIBINFINITYMM_QTIO_INCLUDES )
	
	set( LIBINFINITYMM_QTIO_FOUND false )
	
#dependancies
	find_package( Libinfinitymm REQUIRED )
	
	find_path( LIBINFINITYMM_QTIO_INCLUDE_DIR
		NAMES
			qtio.h
		PATHS
			${CMAKE_SOURCE_DIR}
		PATH_SUFFIXES
			libinfinitymm
	)
	
	set( LIBINFINITYMM_QTIO_INCLUDES
		${LIBINFINITYMM_QTIO_INCLUDE_DIR}
		${LIBINFINITYMM_INCLUDES}
	)
	
	set( LIBINFINITYMM_QTIO_LIBRARIES
		"infinitymm-qtio"
	)
	
	set( LIBINFINITYMM_QTIO_FOUND true )
	
endif( LIBINFINITYMM_QTIO_LIBRARIES AND LIBINFINITYMM_QTIO_INCLUDES )
