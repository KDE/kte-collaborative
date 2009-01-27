# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

project(kobby)

find_package(KDE4 REQUIRED)
find_package(Glib REQUIRED)
find_package(Libinfinity REQUIRED)
find_package(Libinfinitymm REQUIRED)
find_package(Libinftextmm REQUIRED)
find_package(Libqinfinitymm REQUIRED)

set( KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${GLIBMM_INCLUDES}
	${LIBINFINITY_INCLUDES}
	${LIBINFINITYMM_INCLUDES}
	${LIBINFTEXTMM_INCLUDES}
	${LIBQINFINITYMM_INCLUDES}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/kobby
	${CMAKE_SOURCE_DIR}/kobby/dialogs
	${CMAKE_SOURCE_DIR}/kobby/data
	${CMAKE_BINARY_DIR}/kobby
	.
)

set( KOBBY_LIBRARIES
	${KDE4_KDEUI_LIBS}
	${KDE4_KIO_LIBS}
	${KDE4_KTEXTEDITOR_LIBS}
	${KDE4_KDECORE_LIBS}
	${GLIBMM_LIBRARIES}
	${LIBINFINITY_LIBRARY}
	${LIBINFINITYMM_LIBRARIES}
	${LIBINFTEXTMM_LIBRARIES}
	${LIBQINFINITYMM_LIBRARIES}
)

set( KOBBY_SRCS
	editor/noteplugin.cpp
	editor/browseritem.cpp
	editor/browsermodel.cpp
	editor/filebrowserwidget.cpp
	editor/collabdocument.cpp
	editor/collabsession.cpp
	editor/documenttabwidget.cpp
	editor/sidebar.cpp
	editor/mainwindow.cpp
	editor/main.cpp
	
	dialogs/createconnectiondialog.cpp
	dialogs/connectionmanagerwidget.cpp
	dialogs/createitemdialog.cpp
	dialogs/createfolderdialog.cpp
)

set( KOBBY_UI
	dialogs/createconnectionwidget.ui
	dialogs/createitemwidget.ui
	dialogs/createfolderwidget.ui
)

INCLUDE_DIRECTORIES( ${KOBBY_INCLUDES} )

kde4_add_ui_files( KOBBY_SRCS ${KOBBY_UI} )
kde4_add_kcfg_files( KOBBY_SRCS data/kobbysettings.kcfgc )
kde4_add_executable( kobby ${KOBBY_SRCS} )


target_link_libraries( kobby ${KOBBY_LIBRARIES} )

install( TARGETS kobby DESTINATION ${BIN_INSTALL_DIR} )
install( FILES data/kobbyui.rc DESTINATION ${DATA_INSTALL_DIR}/kobby )
