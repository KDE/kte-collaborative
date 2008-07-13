set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} )

FIND_PACKAGE(Libinfinitymm REQUIRED)

message(STATUS "Libinfinitymm includes: ${LIBINFINITYMM_INCLUDES}" )

macro_log_feature(libinfinitymm_FOUND "libinfinitymm" "libinfinity C++ bindings." "http://libinfinitymm.org" TRUE "0_1" "Needed for collaborative support.")
INCLUDE_DIRECTORIES( ${LIBINFINITYMM_INCLUDES} )

set(ktexteditor_kobby_PART_SRCS document.cpp view.cpp plugin.cpp)

kde4_add_plugin(ktexteditor_kobby ${ktexteditor_kobby_PART_SRCS})

target_link_libraries(ktexteditor_kobby ${KDE4_KDECORE_LIBS} ${LIBINFINITYMM_LIBS} ktexteditor)

install(TARGETS ktexteditor_kobby DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES ktexteditor_kobby.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES kobbyui.rc DESTINATION ${DATA_INSTALL_DIR}/ktexteditor_kobby)
