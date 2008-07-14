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
# This program is free software; you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
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
	)
	
	set( GLIB_LIBRARIES
		${GLIB_LIBRARY}
	)
	
endif( GLIB_LIBRARIES AND GLIB_INCLUDES )
