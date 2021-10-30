# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmusb\")

########### next target ###############

set(kcm_usb_PART_SRCS kcmusb.cpp usbdevices.cpp usbdb.cpp )

kcoreaddons_add_plugin(kcm_usb SOURCES ${kcm_usb_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/kinfocenter")

target_link_libraries(kcm_usb KF5::KCMUtils KF5::I18n Libusb-1.0)
