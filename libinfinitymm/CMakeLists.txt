PROJECT(libinfinitymm_qtio)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(Libinfinitymm REQUIRED)

SET(libinfinitymm_qtio_SRCS
	qtio.cpp
	)

TARGET_LINK_LIBRARIES(libinfinitymm-qtio ${QT_LIBRARIES})
