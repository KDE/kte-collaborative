PROJECT(test-xmppconnection)
PROJECT(test-clientbrowser)

SET(test_xmppconnection_SRCS
	xmppconnection.cpp
)

SET(test_clientbrowser_SRCS
	clientbrowser.cpp
)

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(QInfinitymm REQUIRED)

INCLUDE_DIRECTORIES(${QINFINITYMM_INCLUDES})

ADD_EXECUTABLE(test-xmppconnection
	${test_xmppconnection_SRCS}
)

ADD_EXECUTABLE(test-clientbrowser
	${test_clientbrowser_SRCS}
)

TARGET_LINK_LIBRARIES(test-xmppconnection
	${QT_LIBRARIES}
	${QINFINITYMM_LIBRARIES}
)

TARGET_LINK_LIBRARIES(test-clientbrowser
	${QT_LIBRARIES}
	${QINFINITYMM_LIBRARIES}
)
