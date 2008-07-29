PROJECT(test-xmppconnection)

SET(test_xmppconnection_SRCS
	xmppconnection.cpp
)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(QInfinitymm REQUIRED)

INCLUDE_DIRECTORIES(${QINFINITYMM_INCLUDES})

ADD_EXECUTABLE(test-xmppconnection
	${test_xmppconnection_SRCS}
)

TARGET_LINK_LIBRARIES(test-xmppconnection
	${QT_LIBRARIES}
	${QINFINITYMM_LIBRARIES}
)
