# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(Kobby)

find_package(KDE4 REQUIRED)
find_package(Glib REQUIRED)
find_package(Libinfinity REQUIRED)
find_package(Libinfinitymm REQUIRED)
find_package(Libinftextmm REQUIRED)
find_package(Libqinfinitymm REQUIRED)

subdirs(data)

set( KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${GLIBMM_INCLUDES}
	${LIBINFINITY_INCLUDES}
	${LIBINFTEXTYMM_INCLUDES}
	${LIBQINFINITYMM_INCLUDES}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/kobby
	${CMAKE_SOURCE_DIR}/kobby/dialogs
)

set( KOBBY_LIBRARIES
	${KDE4_KDEUI_LIBS}
	${KDE4_KIO_LIBS}
	${KDE4_KTEXTEDITOR_LIBS}
	${GLIBMM_LIBRARIES}
	${LIBINFINITY_LIBRARY}
	${LIBINFTEXTMM_LIBRARIES}
	${LIBQINFINITYMM_LIBRARIES}
)

set( qinfinitymm_source_dir ${CMAKE_SOURCE_DIR}/qinfinitymm )

set( KOBBY_SRCS
	editor/sidebar.cpp
	editor/mainwindow.cpp
	editor/main.cpp
	
	dialogs/createconnectiondialog.cpp
)

set( KOBBY_UI
	dialogs/createconnectionwidget.ui
)

kde4_add_ui_files( KOBBY_SRCS ${KOBBY_UI} )
kde4_add_executable( kobby ${KOBBY_SRCS} )

INCLUDE_DIRECTORIES( ${KOBBY_INCLUDES} )

target_link_libraries( kobby ${KOBBY_LIBRARIES} )

install(
	TARGETS 
		kobby
	DESTINATION
		${BIN_INSTALL_DIR}
)
