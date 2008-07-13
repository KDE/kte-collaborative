# Find glibmm
#
# This script will define:
#	GLIBMM_FOUND
#	GLIBMM_INCLUDES
#	GLIBMM_LIBRARY
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

if( GLIBMM_LIBRARY AND GLIBMM_INCLUDES )
	
	set( LIBINFINITYMM_FOUND true )
	
else( GLIBMM_LIBRARY AND GLIBMM_INCLUDES )
	
	set( LIBINFINITYMM_FOUND false )
	
	if( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
		include(UsePkgConfig)
		pkgconfig(glibmm-2.4
			_GLIBMM_INCLUDE_DIR
			_GLIBMM_LIBRARY_DIR
			_GLIBMM_LINKER_FLAGS
			_GLIBMM_COMPILER_FLAGS
		)
	endif( ${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4 )
	
	find_path( GLIBMM_INCLUDES
		NAMES
			glibmm.h
		PATHS
			${_GLIBMM_INCLUDE_DIR}
			/usr/include
			/usr/local/include
			/opt/local/include
		PATH_SUFFIXES
			glibmm-2.4
	)
	
	find_library( GLIBMM_LIBRARY
		NAMES
			glibmm-2.4
		PATHS
			${_GLIBMM_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)
	
endif( GLIBMM_LIBRARY AND GLIBMM_INCLUDES )
