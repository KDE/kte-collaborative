PROJECT(test-xmppconnection)

SET(test_xmppconnection_SRCS
	xmppconnection.cpp
)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(LibinfinitymmQtIo REQUIRED)

INCLUDE_DIRECTORIES(${LIBINFINITYMM_QTIO_INCLUDES})

ADD_EXECUTABLE(test-xmppconnection
	${test_xmppconnection_SRCS}
)

TARGET_LINK_LIBRARIES(test-xmppconnection
	${QT_LIBRARIES}
	${LIBINFINITYMM_QTIO_LIBRARIES}
)
