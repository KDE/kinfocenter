# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmusb\")

########### next target ###############

set(kcm_usb_PART_SRCS kcmusb.cpp usbdevices.cpp usbdb.cpp )


add_library(kcm_usb MODULE ${kcm_usb_PART_SRCS})


target_link_libraries(kcm_usb KF5::KCMUtils KF5::I18n Libusb-1.0)

install(TARGETS kcm_usb  DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES kcmusb.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )
