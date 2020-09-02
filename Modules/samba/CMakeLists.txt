# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmsamba\")

set(kcm_samba_PART_SRCS
    smbmountmodel.cpp
    ksambasharemodel.cpp
)

# Intermediate lib for use in testing.
add_library(kcm_samba_static STATIC ${kcm_samba_PART_SRCS})
target_link_libraries(kcm_samba_static
    KF5::KIOCore
    KF5::Solid
    KF5::I18n
    KF5::I18n
    KF5::KCMUtils
)

add_library(kcm_samba MODULE main.cpp)
target_link_libraries(kcm_samba
    kcm_samba_static
)

install(TARGETS kcm_samba DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES smbstatus.desktop DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory(autotests)
