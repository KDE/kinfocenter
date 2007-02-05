


########### install files ###############

install( FILES kde-settings.menu kde-information.menu kde-kcontrol.menu
    DESTINATION ${SYSCONF_INSTALL_DIR}/xdg/menus/)
install( FILES kde-information.directory  kde-settings-accessibility.directory  kde-settings-components.directory  kde-settings-desktop.directory  kde-settings.directory  kde-settings-hardware.directory  kde-settings-looknfeel.directory  kde-settings-network.directory  kde-settings-power.directory  kde-settings-security.directory  kde-settings-sound.directory  kde-settings-system.directory  kde-settings-webbrowsing.directory  DESTINATION  ${XDG_DIRECTORY_DIR} )

