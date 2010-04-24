


########### next target ###############

set(kcm_usb_PART_SRCS kcmusb.cpp usbdevices.cpp usbdb.cpp )


kde4_add_plugin(kcm_usb ${kcm_usb_PART_SRCS})


target_link_libraries(kcm_usb  ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY})

install(TARGETS kcm_usb  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcmusb.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES usb.ids  DESTINATION  ${DATA_INSTALL_DIR}/kcmusb )
