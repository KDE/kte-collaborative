automoc4(collaborativeeditingtest collaborativeeditingtest.cpp)
kde4_add_unit_test(
    collaborativeeditingtest
    collaborativeeditingtest.cpp
    ../kobby/manageddocument.cpp
    ../kobby/kobbyplugin.cpp
)

include_directories(
    ${KDE4_INCLUDES}
    ../common
    ../kobby
    ../
    ../kobby/dialogs
    ../kobby/data
    ${LIBQINFINITY_INCLUDES}
)

target_link_libraries( collaborativeeditingtest
    ${KDE4_KFILE_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDNSSD_LIBS}
    ${LIBQINFINITY_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
    kobbycommon
)