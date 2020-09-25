set(info_SRCS # Specify source files for the library
    kcm.cpp
    distroinfo.cpp
    softwareinfo.cpp
    hardwareinfo.cpp
)

add_library(kcm_about_distro MODULE ${info_SRCS})

target_link_libraries(kcm_about_distro
    Qt5::Core
    KF5::CoreAddons
    KF5::I18n
    KF5::QuickAddons
    KF5::ConfigCore
    KF5::Solid
)

kcoreaddons_desktop_to_json(kcm_about_distro "kcm_about_distro.desktop") # Convert our desktop file to json

install(TARGETS kcm_about_distro DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms) # Install the library to the kcm location

install(FILES kcm_about_distro.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR}) # Install the desktop file
kpackage_install_package(package kcm_about_distro kcms) # Finally install our QML kpackage.

