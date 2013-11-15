add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(kcm_samba_PART_SRCS 
   main.cpp 
   ksmbstatus.cpp 
   kcmsambaimports.cpp 
   kcmsambalog.cpp 
   kcmsambastatistics.cpp )


add_library(kcm_samba MODULE ${kcm_samba_PART_SRCS})


target_link_libraries(kcm_samba KF5::KCMUtils KF5::KI18n KF5::KIOCore KF5::KIOWidgets KF5::KDE4Support )

install(TARGETS kcm_samba  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES smbstatus.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
