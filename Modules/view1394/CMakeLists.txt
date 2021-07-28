# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmview1394\")


########### next target ###############

set(kcm_view1394_PART_SRCS view1394.cpp )


ki18n_wrap_ui(kcm_view1394_PART_SRCS view1394widget.ui )

kcoreaddons_add_plugin(kcm_view1394 SOURCES ${kcm_view1394_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
kcoreaddons_desktop_to_json(kcm_view1394 "kcmview1394.desktop")

target_link_libraries(kcm_view1394 KF5::KCMUtils KF5::I18n ${RAW1394_LIBRARIES} )
