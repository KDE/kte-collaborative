# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(Kobby)

find_package(KDE4 REQUIRED)
find_package(Glib REQUIRED)
find_package(Libinftextmm REQUIRED)
find_package(Libqinfinitymm REQUIRED)

subdirs(data)

set( KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${LIBINFTEXTMM_INCLUDES}
	${LIBQINFINITYMM_INCLUDES}
	${CMAKE_SOURCE_DIR}
)

set( KOBBY_LIBRARIES
	${KDE4_KDEUI_LIBS}
	${KDE4_KIO_LIBS}
	${KDE4_KTEXTEDITOR_LIBS}
	${LIBINFTEXTMM_LIBRARIES}
	${LIBQINFINITYMM_LIBRARIES}
)

set( qinfinitymm_source_dir ${CMAKE_SOURCE_DIR}/qinfinitymm )

set( KOBBY_SRCS
	editor/mainwindow.cpp
	editor/main.cpp
)

kde4_add_executable( kobby ${KOBBY_SRCS} )

INCLUDE_DIRECTORIES( ${KOBBY_INCLUDES} )

target_link_libraries( kobby ${KOBBY_LIBRARIES} )

install(
	TARGETS 
		kobby
	DESTINATION
		${BIN_INSTALL_DIR}
)
