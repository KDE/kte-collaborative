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
