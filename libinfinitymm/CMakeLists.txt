PROJECT(libinfinitymm-qtio)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(Libinfinitymm REQUIRED)
INCLUDE_DIRECTORIES(${LIBINFINITYMM_INCLUDES})

SET(libinfinitymm_qtio_SRCS
	ioqsocketnotifier.cpp
	qtio.cpp
	)

ADD_LIBRARY(libinfinitymm-qtio ${libinfinitymm_qtio_SRCS})

TARGET_LINK_LIBRARIES(libinfinitymm-qtio ${QT_LIBRARIES} ${LIBINFINITYMM_LIBRARIES})
