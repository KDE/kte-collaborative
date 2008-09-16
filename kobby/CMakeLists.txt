# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(Kobby)

find_package(KDE4 REQUIRED)
find_package(Glib REQUIRED)
find_package(Libinfinitymm REQUIRED)

# data files
add_subdirectory( data )

set( KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${LIBINFINITYMM_INCLUDES}
	${CMAKE_SOURCE_DIR}
)

set( KOBBY_LIBRARIES
	${KDE4_KDECORE_LIBS}
	${LIBINFINITYMM_LIBRARIES}
)

set( qinfinitymm_source_dir ${CMAKE_SOURCE_DIR}/qinfinitymm )

set( KOBBY_SRCS

	# dialogs
	dialogs/settingsdialog.cpp
	dialogs/connectionmanager.cpp
	dialogs/filebrowser.cpp
	dialogs/controldialog.cpp

	# editor components
	editor/document.cpp
	editor/view.cpp
	editor/plugin.cpp

	# infinote related
	infinote/infinotemanager.cpp
	infinote/connection.cpp

	# qinfinitymm
	${qinfinitymm_source_dir}/qtio.cpp
	${qinfinitymm_source_dir}/ioqsocketnotifier.cpp
)

kde4_add_ui_files( KOBBY_SRCS
	dialogs/connectioneditor.ui
	dialogs/addconnectiondialog.ui
	dialogs/filebrowserwidget.ui
	dialogs/controlwidget.ui
	dialogs/settingswidget.ui
	dialogs/usersettingswidget.ui
)

kde4_add_plugin( ktexteditor_kobby ${KOBBY_SRCS} )

INCLUDE_DIRECTORIES( ${KOBBY_INCLUDES} )

target_link_libraries( ktexteditor_kobby ${KOBBY_LIBRARIES} ktexteditor )

install(
	TARGETS 
		ktexteditor_kobby
	DESTINATION
		${PLUGIN_INSTALL_DIR}
)
