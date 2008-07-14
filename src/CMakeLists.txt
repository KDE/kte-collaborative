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

FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(Glib REQUIRED)
FIND_PACKAGE(Libinfinitymm REQUIRED)

set(KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${LIBINFINITYMM_INCLUDES}
)

set(KOBBY_LIBRARIES
	${KDE4_KDECORE_LIBS}
	${LIBINFINITYMM_LIBRARIES}
)

set(KOBBY_PART_SRCS
	document.cpp
	view.cpp
	plugin.cpp
)

kde4_add_plugin(ktexteditor_kobby ${KOBBY_PART_SRCS})

INCLUDE_DIRECTORIES(${KOBBY_INCLUDES})
target_link_libraries(ktexteditor_kobby ${KOBBY_LIBRARIES} ktexteditor)

install(TARGETS ktexteditor_kobby DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES ktexteditor_kobby.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES kobbyui.rc DESTINATION ${DATA_INSTALL_DIR}/ktexteditor_kobby)
