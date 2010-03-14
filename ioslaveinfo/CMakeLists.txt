


########### next target ###############

set(kcm_ioslaveinfo_PART_SRCS kcmioslaveinfo.cpp )


kde4_add_plugin(kcm_ioslaveinfo ${kcm_ioslaveinfo_PART_SRCS})


target_link_libraries(kcm_ioslaveinfo  ${KDE4_KHTML_LIBS} ${QT_QTGUI_LIBRARY})

install(TARGETS kcm_ioslaveinfo  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES ioslaveinfo.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

