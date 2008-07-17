# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

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
	configdialog.cpp
)

kde4_add_plugin(ktexteditor_kobby ${KOBBY_PART_SRCS})

INCLUDE_DIRECTORIES(${KOBBY_INCLUDES})
target_link_libraries(ktexteditor_kobby ${KOBBY_LIBRARIES} ktexteditor)

install(TARGETS ktexteditor_kobby DESTINATION ${PLUGIN_INSTALL_DIR})

install(
	FILES
		ktexteditor_kobby.desktop
		ktexteditor_kobby_config.desktop
	DESTINATION
		${SERVICES_INSTALL_DIR}
)

install(
	FILES
		kobbyui.rc
	DESTINATION
		${DATA_INSTALL_DIR}/ktexteditor_kobby
)
