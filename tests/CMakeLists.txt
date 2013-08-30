automoc4(collaborativeeditingtest collaborativeeditingtest.cpp)
kde4_add_unit_test(
    collaborativeeditingtest
    collaborativeeditingtest.cpp
    $<TARGET_OBJECTS:ktexteditor_kobby_objects>
)

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
    kobbycommon
    inftube
)