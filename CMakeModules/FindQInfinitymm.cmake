# Find libinfinitymm-qtio
#
# This script will define:
#	QINFINITYMM_FOUND
#	QINFINITYMM_INCLUDES
#	QINFINITYMM_LIBRARY
#	QINFINITYMM_LIBRARIES
#
# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if( QINFINITYMM_LIBRARIES AND QINFINITYMM_INCLUDES )
	
	set( QINFINITYMM_FOUND true )
	
else( QINFINITYMM_LIBRARIES AND QINFINITYMM_INCLUDES )
	
	set( QINFINITYMM_FOUND false )
	
#dependancies
	find_package( Libinfinitymm REQUIRED )
	
	find_path( QINFINITYMM_INCLUDE_DIR
		NAMES
			qtio.h
		PATHS
			${CMAKE_SOURCE_DIR}
		PATH_SUFFIXES
			qinfinitymm
	)
	
	set( QINFINITYMM_INCLUDES
		${QINFINITYMM_INCLUDE_DIR}
		${LIBINFINITYMM_INCLUDES}
	)
	
	set( QINFINITYMM_LIBRARIES
		"qinfinitymm"
	)
	
	set( QINFINITYMM_FOUND true )
	
endif( QINFINITYMM_LIBRARIES AND QINFINITYMM_INCLUDES )
