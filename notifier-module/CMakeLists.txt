
include_directories(
    ${KDE4_KIO_INCLUDES}
    ${KDE4_INCLUDES}
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

set(infinotenotifier_SRCS
   infinotenotifier.cpp
)

set(CMAKE_INSTALL_RPATH ${LIB_INSTALL_DIR}/ktecollaborative/)
kde4_add_executable(infinotenotifier ${infinotenotifier_SRCS})

target_link_libraries(infinotenotifier
                      kobbycommon
                      ${KDE4_KIO_LIBS}
                      ${LIBQINFINITY_LIBRARIES}
)

# TODO can't we do this without an absolute path? It sucks.
configure_file( infinotenotifier.desktop.in infinotenotifier.desktop )
install( TARGETS infinotenotifier DESTINATION ${LIBEXEC_INSTALL_DIR} )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/infinotenotifier.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES infinotenotifier.notifyrc DESTINATION ${DATA_INSTALL_DIR}/infinotenotifier )
