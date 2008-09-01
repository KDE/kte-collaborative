project(qinfinitymm)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

find_package(Libinfinitymm REQUIRED)

SET( qinfinitymm_SRCS
	qtio.cpp
	ioqsocketnotifier.cpp
)

QT4_AUTOMOC( ${qinfinitymm_SRCS} )

ADD_LIBRARY( qinfinitymm SHARED
	${qinfinitymm_SRCS}
)

INCLUDE_DIRECTORIES( ${LIBINFINITYMM_INCLUDES} ${CMAKE_SOURCE_DIR} )

TARGET_LINK_LIBRARIES( qinfinitymm
	${QT_LIBRARIES}
	${LIBINFINITYMM_LIBRARIES}
)



SUBDIRS( test )
