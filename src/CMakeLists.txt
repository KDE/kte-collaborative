set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )

FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(Glibmm REQUIRED)
FIND_PACKAGE(Libinfinitymm REQUIRED)

set(KOBBY_INCLUDES
	${KDE4_INCLUDES}
	${GLIBMM_INCLUDES}
	${LIBINFINITYMM_INCLUDES}
	)

set(KOBBY_LIBRARIES
	${KDE4_KDECORE_LIBS}
	${GLIBMM_LIBRARY}
	${LIBINFINITYMM_LIBRARY}
	)

set(KOBBY_PART_SRCS document.cpp view.cpp plugin.cpp)

kde4_add_plugin(ktexteditor_kobby ${KOBBY_PART_SRCS})

INCLUDE_DIRECTORIES(${KOBBY_INCLUDES})
target_link_libraries(ktexteditor_kobby ${KOBBY_LIBRARIES} ktexteditor)

install(TARGETS ktexteditor_kobby DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES ktexteditor_kobby.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES kobbyui.rc DESTINATION ${DATA_INSTALL_DIR}/ktexteditor_kobby)
