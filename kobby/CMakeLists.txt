# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(Kobby)

find_package(KDE4 REQUIRED)
find_package(Glib REQUIRED)
find_package(Libinftextmm REQUIRED)

subdirs(data)

set( KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${LIBINFTEXTMM_INCLUDES}
	${CMAKE_SOURCE_DIR}
)

set( KOBBY_LIBRARIES
	${KDE4_KDEUI_LIBS}
	${KDE4_KIO_LIBS}
	${KDE4_KTEXTEDITOR_LIBS}
	${LIBINFTEXTMM_LIBRARIES}
)

set( qinfinitymm_source_dir ${CMAKE_SOURCE_DIR}/qinfinitymm )

set( KOBBY_SRCS

	# dialogs
	dialogs/settingsdialog.cpp
	dialogs/connectionmanager.cpp
	dialogs/requestprogress.cpp
	dialogs/filebrowser.cpp
	dialogs/controldialog.cpp

	# editor components
	editor/mainwindow.cpp

	# infinote related
	infinote/infinotemanager.cpp
	infinote/connection.cpp

	# qinfinitymm
	${qinfinitymm_source_dir}/qtio.cpp
	${qinfinitymm_source_dir}/ioqsocketnotifier.cpp
	
	main.cpp
)

kde4_add_ui_files( KOBBY_SRCS
	dialogs/connectioneditor.ui
	dialogs/addconnectiondialog.ui
	dialogs/createnodewidget.ui
	dialogs/requestprogresswidget.ui
	dialogs/filebrowserwidget.ui
	dialogs/controlwidget.ui
	dialogs/settingswidget.ui
	dialogs/usersettingswidget.ui
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
