# Find libinfinity
#
# This script will define:
#	LIBINFINITY_FOUND
#	LIBINFINITY_INCLUDES
#	LIBINFINITY_LIBRARY
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

if( LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES )
	
	set( LIBINFINITY_FOUND true )

else( LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES )
	
	set( LIBINFINITY_FOUND false )
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(libinfinity-1.0
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
			libinfinity-1.0
	)
	
	find_library( LIBINFINITY_LIBRARY
		NAMES
			infinity-1.0
		PATHS
			${_LIBINFINITY_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)

endif( LIBINFINITY_LIBRARIES AND LIBINFINITY_INCLUDES )
