install(
	FILES
		ktexteditor_kobby.desktop
		ktexteditor_kobby_settings.desktop
	DESTINATION
		${SERVICES_INSTALL_DIR}
)

install(
	FILES
		kobbyui.rc
	DESTINATION
		${DATA_INSTALL_DIR}/ktexteditor_kobby
)