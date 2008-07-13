# Find sigc++
#
# This script will define:
#	SIGCPP_FOUND
#	SIGCPP_INCLUDES
#	SIGCPP_LIBRARY
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
	
	find_path( SIGCPP_INCLUDES
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
	
	find_library( SIGCPP_LIBRARY
		NAMES
			sigc-2.0
		PATHS
			${_SIGCPP_LIBRARY_DIR}
			/usr/lib
			/usr/local/lib
	)

endif( SIGCPP_LIBRARIES AND SIGCPP_INCLUDES )
