add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(kcm_samba_PART_SRCS 
   main.cpp 
   ksmbstatus.cpp 
   kcmsambaimports.cpp 
   kcmsambalog.cpp 
   kcmsambastatistics.cpp )


kde4_add_plugin(kcm_samba ${kcm_samba_PART_SRCS})


target_link_libraries(kcm_samba ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kcm_samba  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES smbstatus.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
