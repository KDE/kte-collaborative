set(inftube_SRCS
    inftube.cpp
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

target_link_libraries(inftube
                    ${KDE4_KDEUI_LIBS}
                    ${TELEPATHY_QT4_LIBRARIES}
                    ${KTP_LIBRARIES}
                    ${KDE4_KIO_LIBS}
                    ${KTP_MODELS_LIBRARIES}
                    ${KTP_WIDGETS_LIBRARIES}
)

install(TARGETS inftube ${INSTALL_TARGETS_DEFAULT_ARGS})

# the tube handler
kde4_add_executable(infinotetubehandler tubehandler.cpp)
target_link_libraries(infinotetubehandler
                      inftube
                      ${KTP_LIBRARIES}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${TELEPATHY_QT4_LIBRARIES}
)

# find infinoted and tell the program
find_program(infinoted infinoted-0.5 HINTS /usr/bin)
if ( NOT infinoted )
    message(FATAL_ERROR "Could not find infinoted (infinoted-0.5 executable), which is required to start collaborative sessions.")
endif()
configure_file(infinoted.h.in infinoted.h)

install(TARGETS infinotetubehandler DESTINATION ${LIBEXEC_INSTALL_DIR})
configure_file(org.freedesktop.Telepathy.Client.KTp.infinity.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.infinity.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.infinity.service
        DESTINATION ${DBUS_SERVICES_INSTALL_DIR})
install(FILES KTp.infinity.client DESTINATION ${SHARE_INSTALL_PREFIX}/telepathy/clients/)
install(FILES kte-collaborative.desktop DESTINATION ${SHARE_INSTALL_PREFIX}/kde4/services/ktp)
