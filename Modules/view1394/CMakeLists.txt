# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmview1394\")


########### next target ###############

set(kcm_view1394_PART_SRCS view1394.cpp )


ki18n_wrap_ui(kcm_view1394_PART_SRCS view1394widget.ui )

add_library(kcm_view1394 MODULE ${kcm_view1394_PART_SRCS})

target_link_libraries(kcm_view1394 KF5::KCMUtils KF5::I18n ${RAW1394_LIBRARIES} )

install(TARGETS kcm_view1394  DESTINATION ${KDE_INSTALL_PLUGINDIR} )


########### install files ###############

install( FILES kcmview1394.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )

