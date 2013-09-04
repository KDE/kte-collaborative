
include_directories(
    ${KDE4_KIO_INCLUDES}
    ${KDE4_INCLUDES}
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_SOURCE_DIR}
)

set(kded_infinote_SRCS
   infinotenotifier.cpp
)

kde4_add_plugin(kded_infinotenotifier ${kded_infinote_SRCS})

target_link_libraries(kded_infinotenotifier
                      kobbycommon
                      ${KDE4_KIO_LIBS}
                      ${LIBQINFINITY_LIBRARIES}
)

install( TARGETS kded_infinotenotifier DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES infinotenotifier.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded )
