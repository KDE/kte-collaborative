include(ECMMarkAsTest)

add_executable(
    collaborativeeditingtest
    collaborativeeditingtest.cpp
    $<TARGET_OBJECTS:ktecollaborative_objects>
)
ecm_mark_as_test(collabtest)

include_directories(
    ${KDE4_INCLUDES}
    ../common
    ../kte-plugin
    ../
    ${LIBQINFINITY_INCLUDES}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries( collaborativeeditingtest
    ${KDE4_KFILE_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${KDE4_KCMUTILS_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDNSSD_LIBS}
    ${LIBQINFINITY_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    ${TELEPATHY_QT4_LIBRARIES}
    ${KTP_LIBRARIES}
    ${KDE4_KIO_LIBS}
    ${KTP_MODELS_LIBRARIES}
    ${KTP_WIDGETS_LIBRARIES}
    ktecollaborativecommon
    inftube
)