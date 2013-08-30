set(inftube_SRCS
    connectionswidget.cpp
    inftube.cpp
    ../ui/selecteditorwidget.cpp
)

set(IS_KTP_INTERNAL_MODULE TRUE) # aw yeah
find_package(TelepathyQt4 0.8.9 REQUIRED)
find_package(KTp REQUIRED)
include_directories(
    ${QT_INCLUDES}
    ${KDE4_INCLUDES}
    ${TELEPATHY_QT4_INCLUDE_DIR}
    ${KTP_INCLUDE_DIR}
)

kde4_add_library(inftube SHARED ${inftube_SRCS})
set_target_properties( inftube
    PROPERTIES
    VERSION ${KTECOLLAB_VERSION_STRING}
    SOVERSION ${KTECOLLAB_VERSION_STRING}
)

target_link_libraries(inftube
                      ${KDE4_KDEUI_LIBS}
                      ${TELEPATHY_QT4_LIBRARIES}
                      ${KTP_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KTP_MODELS_LIBRARIES}
                      ${KTP_WIDGETS_LIBRARIES}
)

install(TARGETS inftube ${INSTALL_TARGETS_DEFAULT_ARGS})

# the tube handlers
kde4_add_executable(infinoteclienttubehandler clienttubehandler.cpp)
target_link_libraries(infinoteclienttubehandler
                      inftube
                      ${KTP_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${TELEPATHY_QT4_LIBRARIES}
)

kde4_add_executable(infinoteservertubehandler servertubehandler.cpp)
target_link_libraries(infinoteservertubehandler
                      inftube
                      ${KTP_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${TELEPATHY_QT4_LIBRARIES}
)

# find infinoted and tell the program
find_program(infinoted infinoted-0.6 NAMES infinoted infinoted-0.6 HINTS /usr/bin)
if ( NOT infinoted )
    message(FATAL_ERROR "Could not find infinoted (infinoted-0.6 executable), which is required to start collaborative sessions.")
endif()
configure_file(infinoted.h.in infinoted.h)

install(TARGETS infinoteclienttubehandler DESTINATION ${LIBEXEC_INSTALL_DIR})
install(TARGETS infinoteservertubehandler DESTINATION ${LIBEXEC_INSTALL_DIR})
configure_file(org.freedesktop.Telepathy.Client.KTp.infinote.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.infinote.service)
configure_file(org.freedesktop.Telepathy.Client.KTp.infinoteServer.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.infinoteServer.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.infinote.service
              ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.infinoteServer.service
        DESTINATION ${DBUS_SERVICES_INSTALL_DIR})
install(FILES KTp.infinote.client KTp.infinoteServer.client DESTINATION ${SHARE_INSTALL_PREFIX}/telepathy/clients/)
install(FILES kte-collaborative.desktop DESTINATION ${SHARE_INSTALL_PREFIX}/kde4/services/ktp)
