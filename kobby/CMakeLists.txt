# Copyright (c) 2008 Gregory Haynes <greg@greghaynes.net>
# Copyright (c) 2013 Sven Brauch <svenbrauch@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

find_package(KDE4 4.5.0 REQUIRED)
include(KDE4Defaults)

set( kobby_PART_SRCS
    kobbyplugin.cpp
    manageddocument.cpp
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
    kobbycommon
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
