


########### next target ###############

set(kcm_view1394_PART_SRCS view1394.cpp )


kde4_add_ui_files(kcm_view1394_PART_SRCS view1394widget.ui )

kde4_add_plugin(kcm_view1394 ${kcm_view1394_PART_SRCS})

target_link_libraries(kcm_view1394 KF5::KCMUtils KF5::KI18n ${KDE4Support_LIBRARIES} ${RAW1394_LIBRARIES} )

install(TARGETS kcm_view1394  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcmview1394.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES oui.db  DESTINATION  ${DATA_INSTALL_DIR}/kcmview1394 )

