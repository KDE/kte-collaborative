# Copyright (c) 2013 Sven Brauch <svenbrauch@gmail.com>
# The common directory (and library) contais functionality which is common to both the KTE plugin and the kioslave.
# Since both need the whole connection and browser stuff, this is a quite large part of the code base.

# Check if KTE is new enough to have the BufferInterface
try_compile( HAVE_BUFFERIFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/testbufferinterface.cpp
             CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${KDE4_INCLUDES}"
             LINK_LIBRARIES ${KDE4_KTEXTEDITOR_LIBS} )
if(HAVE_BUFFERIFACE)
    add_definitions("-DKTEXTEDITOR_HAS_BUFFER_IFACE")
endif(HAVE_BUFFERIFACE)

include_directories(
    ${KDE4_INCLUDES}
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_BINARY_DIR}/ktecollaborative
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/
)

set( KTECOLLABORATIVE_COMMON_SRCS
    connection.cpp
    document.cpp
    itemfactory.cpp
    noteplugin.cpp
    utils.cpp
    selecteditorwidget.cpp
)

kde4_add_library(ktecollaborativecommon SHARED ${KTECOLLABORATIVE_COMMON_SRCS})
set_target_properties( ktecollaborativecommon
    PROPERTIES
    VERSION ${KTECOLLAB_VERSION_STRING}
    SOVERSION ${KTECOLLAB_VERSION_STRING}
)
target_link_libraries( ktecollaborativecommon
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KFILE_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDNSSD_LIBS}
    ${LIBQINFINITY_LIBRARIES}
)

install(TARGETS ktecollaborativecommon DESTINATION ${LIB_INSTALL_DIR}/ktecollaborative)
