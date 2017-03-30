add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)
# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmsamba\")

set(kcm_samba_PART_SRCS 
   main.cpp 
   ksmbstatus.cpp 
   kcmsambaimports.cpp 
   kcmsambalog.cpp 
   kcmsambastatistics.cpp )


add_library(kcm_samba MODULE ${kcm_samba_PART_SRCS})


target_link_libraries(kcm_samba
    KF5::I18n
    KF5::KCMUtils
    KF5::KIOCore
    KF5::KIOWidgets
)

install(TARGETS kcm_samba  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES smbstatus.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
