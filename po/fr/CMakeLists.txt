file(GLOB _po_files *.po)
# Add a line like the following when you add a translation
GETTEXT_PROCESS_PO_FILES( fr ALL INSTALL_DESTINATION ${LOCALE_INSTALL_DIR} ${_po_files} )
