# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
# Copyright (c) 2013-2016 Sven Brauch <svenbrauch@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

cmake_minimum_required(VERSION 2.8)

add_subdirectory(settings)
# add_subdirectory(ktpintegration) # disabled, not ported

set( ktecollaborative_PART_SRCS
    documentchangetracker.cpp
    ktecollaborativepluginview.cpp
    ktecollaborativeplugin.cpp
    manageddocument.cpp
#     ui/remotechangenotifier.cpp
    ui/sharedocumentdialog.cpp
    ui/opencollabdocumentdialog.cpp
#     ui/statusoverlay.cpp
    settings/kcm_kte_collaborative.cpp
    ../kte_collab_debug.cpp
)

add_definitions(-fPIC)
add_library(ktecollaborative_objects OBJECT ${ktecollaborative_PART_SRCS})

set(CMAKE_INSTALL_RPATH ${LIB_INSTALL_DIR}/ktecollaborative/)
add_library(ktexteditor_collaborative $<TARGET_OBJECTS:ktecollaborative_objects>)

include_directories(
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/common/
    KF5::TextEditor
)

target_link_libraries( ktexteditor_collaborative
    ktecollaborativecommon
    inftube
    Qt5::Core
    Qt5::Widgets
    KF5::TextEditor
    KF5::Parts
    ${KDE4_KCMUTILS_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDNSSD_LIBS}
    ${LIBQINFINITY_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
    ${TELEPATHY_QT4_LIBRARIES}
    ${KTP_LIBRARIES}
    ${KDE4_KIO_LIBS}
    ${KTP_MODELS_LIBRARIES}
    ${KTP_WIDGETS_LIBRARIES}
)

install(TARGETS ktexteditor_collaborative DESTINATION ${PLUGIN_INSTALL_DIR} )
install(FILES ktexteditor_collaborative.desktop settings/ktexteditor_collaborative_config.desktop DESTINATION  ${SERVICES_INSTALL_DIR})
install(FILES ktexteditor_collaborativeui.rc  DESTINATION  ${DATA_INSTALL_DIR}/ktecollaborative/)
install(FILES ui/notifywidget.qml ui/overlay.qml DESTINATION ${DATA_INSTALL_DIR}/ktecollaborative/ui)
