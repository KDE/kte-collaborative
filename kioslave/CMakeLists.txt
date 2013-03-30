include_directories(
    ${KDE4_INCLUDES}
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/kobby/
    ${CMAKE_SOURCE_DIR}/kobby/editor/
    ${CMAKE_SOURCE_DIR}/kobby/dialogs/
)

set(kio_infinity_PART_SRCS
    kio_infinity.cpp
    ../kobby/editor/connection.cpp
    ../kobby/editor/documentmodel.cpp
    ../kobby/editor/itemfactory.cpp
    ../kobby/kobbysettings.cpp
    ../kobby/editor/documentmodel.cpp
    ../kobby/editor/document.cpp
)
kde4_add_plugin(kio_infinity ${kio_infinity_PART_SRCS})

target_link_libraries(kio_infinity
    ${KDE4_KIO_LIBS}
    ${LIBQINFINITY_LIBRARIES}
    ${KDE4_KTEXTEDITOR_LIBS}
)

install( TARGETS kio_infinity DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES infinity.protocol DESTINATION  ${SERVICES_INSTALL_DIR} )

