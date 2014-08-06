


########### install files ###############
if (WIN32)
install( FILES kinfocenter.menu DESTINATION ${SHARE_INSTALL_DIR}/xdg/menus/)
else ()
install( FILES kinfocenter.menu DESTINATION ${SYSCONF_INSTALL_DIR}/xdg/menus/)
endif ()

install( FILES kinfocenter.directory  DESTINATION  ${XDG_DIRECTORY_INSTALL_DIR} )
