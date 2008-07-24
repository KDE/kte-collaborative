PROJECT(infinitymm-qtio)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(Libinfinitymm REQUIRED)

SET(infinitymm_qtio_SRCS
	qtio.cpp
	ioqsocketnotifier.cpp
)

QT4_AUTOMOC(${infinitymm_qtio_SRCS})

ADD_LIBRARY(infinitymm-qtio STATIC
	${infinitymm_qtio_SRCS}
)

INCLUDE_DIRECTORIES(${LIBINFINITYMM_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR})

TARGET_LINK_LIBRARIES(infinitymm-qtio
	${QT_LIBRARIES}
	${LIBINFINITYMM_LIBRARIES}
)

SUBDIRS(test)
