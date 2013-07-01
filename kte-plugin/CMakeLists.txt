# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
# Copyright (c) 2013 Sven Brauch <svenbrauch@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

cmake_minimum_required(VERSION 2.8)

add_subdirectory(settings)
add_subdirectory(ktpintegration)

find_package(KDE4 4.5.0 REQUIRED)
include(KDE4Defaults)

set( kobby_PART_SRCS
    documentchangetracker.cpp
    kobbypluginview.cpp
    kobbyplugin.cpp
    manageddocument.cpp
    ui/remotechangenotifier.cpp
    settings/kcm_kte_collaborative.cpp
)

add_definitions(-fPIC)
automoc4(ktexteditor_kobby_objects kobby_PART_SRCS)
add_library(ktexteditor_kobby_objects OBJECT ${kobby_PART_SRCS})
kde4_add_plugin(ktexteditor_kobby $<TARGET_OBJECTS:ktexteditor_kobby_objects>)

include_directories(
    ${TELEPATHY_QT4_INCLUDE_DIR}
    ${KTP_INCLUDE_DIR}
    ${KDE4_INCLUDES}
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/kobby
    ${CMAKE_SOURCE_DIR}/kobby/dialogs
    ${CMAKE_SOURCE_DIR}/kobby/data
    ${CMAKE_BINARY_DIR}/kobby
)

target_link_libraries( ktexteditor_kobby
    kobbycommon
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

install(TARGETS ktexteditor_kobby DESTINATION ${PLUGIN_INSTALL_DIR} )
install(FILES ktexteditor_kobby.desktop settings/ktexteditor_kobby_config.desktop DESTINATION  ${SERVICES_INSTALL_DIR})
install(FILES ktexteditor_kobbyui.rc  DESTINATION  ${DATA_INSTALL_DIR}/ktexteditor_kobby)
install(FILES ui/notifywidget.qml DESTINATION ${DATA_INSTALL_DIR}/kte-kobby/ui)
