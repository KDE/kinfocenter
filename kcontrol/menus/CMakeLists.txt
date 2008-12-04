


########### install files ###############
if (WIN32)
install( FILES kde-information.menu DESTINATION ${SHARE_INSTALL_DIR}/xdg/menus/)
else (WIN32)
install( FILES kde-information.menu DESTINATION ${SYSCONF_INSTALL_DIR}/xdg/menus/)
endif (WIN32)

install( FILES kde-information.directory  DESTINATION  ${XDG_DIRECTORY_INSTALL_DIR} )
