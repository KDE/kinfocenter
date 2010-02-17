

set(KCM_PARTITION_PART_SRCS kcm_partition.cpp usedSizeWidget.cpp )

kde4_add_plugin(kcm_partition ${KCM_PARTITION_PART_SRCS})

target_link_libraries(kcm_partition  ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} )

install(TARGETS kcm_partition  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kcm_partition.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
