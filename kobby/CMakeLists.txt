# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(kobby)

find_package(KDE4 REQUIRED)
find_package(Libqinfinity REQUIRED)
set(KDE_MIN_VERSION "4.2.0")

subdirs( data )

set( KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${LIBQINFINITY_INCLUDES}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/kobby
	${CMAKE_SOURCE_DIR}/kobby/dialogs
	${CMAKE_SOURCE_DIR}/kobby/data
	${CMAKE_BINARY_DIR}/kobby
)

set( KOBBY_LIBRARIES
	${KDE4_KDEUI_LIBS}
	${KDE4_KIO_LIBS}
	${KDE4_KFILE_LIBS}
	${KDE4_KTEXTEDITOR_LIBS}
	${KDE4_KDECORE_LIBS}
	${LIBQINFINITY_LIBRARIES}
)

set( KOBBY_SRCS
	dialogs/createconnectiondialog.cpp
	dialogs/createitemdialog.cpp
	dialogs/settingsdialog.cpp

	editor/noteplugin.cpp
	editor/document.cpp
	editor/documentbuilder.cpp
	editor/documentmodel.cpp
	editor/remotebrowserview.cpp
	editor/localbrowserview.cpp
	editor/documentlistview.cpp
	editor/documenttabwidget.cpp
	editor/connection.cpp
	editor/itemfactory.cpp
	editor/mainwindow.cpp
	editor/main.cpp
)

set( KOBBY_UI
	dialogs/createconnectionwidget.ui
	dialogs/createitemwidget.ui
	dialogs/profilesettingswidget.ui
	dialogs/networksettingswidget.ui
)

INCLUDE_DIRECTORIES( ${KOBBY_INCLUDES} )

kde4_add_ui_files( KOBBY_SRCS ${KOBBY_UI} )
kde4_add_kcfg_files( KOBBY_SRCS data/kobbysettings.kcfgc )
kde4_add_executable( kobby ${KOBBY_SRCS} )


target_link_libraries( kobby ${KOBBY_LIBRARIES} )

install( TARGETS kobby ${INSTALL_TARGETS_DEFAULT_ARGS} )
