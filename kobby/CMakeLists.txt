# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

find_package(Libqinfinity REQUIRED)
find_package(KDE4 4.5.0 REQUIRED)
include(KDE4Defaults)

set( kobby_PART_SRCS
    kobbyplugin.cpp
    kobbysettings.cpp
    editor/connection.cpp
    editor/document.cpp
    editor/documentmodel.cpp
    editor/documentbuilder.cpp
    editor/itemfactory.cpp
    editor/noteplugin.cpp
    editor/remotebrowserview.cpp
    dialogs/createitemdialog.cpp
)

kde4_add_plugin(ktexteditor_kobby ${kobby_PART_SRCS})

include_directories(
    ${KDE4_INCLUDES}
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/kobby
    ${CMAKE_SOURCE_DIR}/kobby/dialogs
    ${CMAKE_SOURCE_DIR}/kobby/data
    ${CMAKE_BINARY_DIR}/kobby
)

target_link_libraries( ktexteditor_kobby
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDNSSD_LIBS}
    ${LIBQINFINITY_LIBRARIES}
)

install(TARGETS ktexteditor_kobby DESTINATION ${PLUGIN_INSTALL_DIR} )
install(FILES ktexteditor_kobby.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})

#
# INCLUDE_DIRECTORIES(${KOBBY_INCLUDES} )
#
# set(KOBBY_LIBRARIES
# 	${KDE4_KDEUI_LIBS}
# 	${KDE4_KIO_LIBS}
# 	${KDE4_KFILE_LIBS}
# 	${KDE4_KTEXTEDITOR_LIBS}
# 	${KDE4_KDECORE_LIBS}
# 	${KDE4_KDNSSD_LIBS}
# 	${LIBQINFINITY_LIBRARIES}
# )
#
# set( KOBBY_SRCS
# 	dialogs/createconnectiondialog.cpp
# 	dialogs/createitemdialog.cpp
# 	dialogs/settingsdialog.cpp
# 	dialogs/setupdialog.cpp
# 	dialogs/closedocsdialog.cpp
# 	dialogs/uploaddocumentdialog.cpp
#
# 	editor/noteplugin.cpp
# 	editor/document.cpp
# 	editor/documentbuilder.cpp
# 	editor/documentmodel.cpp
# 	editor/remotebrowserview.cpp
# 	editor/localbrowserview.cpp
# 	editor/documentusersbrowser.cpp
# 	editor/documentlistview.cpp
# 	editor/documenttabwidget.cpp
# 	editor/connection.cpp
# 	editor/itemfactory.cpp
# 	editor/mainwindow.cpp
# 	editor/main.cpp
# )
#
# set(KOBBY_UI
# 	dialogs/createconnectionwidget.ui
# 	dialogs/createitemwidget.ui
# 	dialogs/profilesettingswidget.ui
# 	dialogs/networksettingswidget.ui
# 	dialogs/uisettingswidget.ui
# 	dialogs/setuppagewidget.ui
# 	dialogs/profilesetupwidget.ui
# 	dialogs/closedocswidget.ui
# )
#
# INCLUDE_DIRECTORIES(${KOBBY_INCLUDES} )
#
# kde4_add_ui_files(KOBBY_SRCS ${KOBBY_UI} )
# kde4_add_kcfg_files(KOBBY_SRCS data/kobbysettings.kcfgc )
# kde4_add_executable(kobby ${KOBBY_SRCS} )
#
#
# target_link_libraries(kobby ${KOBBY_LIBRARIES} )
#
# install(TARGETS kobby DESTINATION ${BIN_INSTALL_DIR} )
#
# add_subdirectory(data)
