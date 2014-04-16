


########### next target ###############

set(kcm_usb_PART_SRCS kcmusb.cpp usbdevices.cpp usbdb.cpp )


add_library(kcm_usb MODULE ${kcm_usb_PART_SRCS})


target_link_libraries(kcm_usb KF5::KCMUtils KF5::I18n KF5::KDELibs4Support)

install(TARGETS kcm_usb  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcmusb.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES usb.ids  DESTINATION  ${DATA_INSTALL_DIR}/kcmusb )
