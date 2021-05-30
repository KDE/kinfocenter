# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmsamba\")

file(GLOB QML_SRCS package/contents/*.qml)
add_custom_target(QmlFiles ALL echo SOURCES ${QML_SRCS})

set(kcm_samba_PART_SRCS
    smbmountmodel.cpp
    ksambasharemodel.cpp
)

qt_add_dbus_interface(kcm_samba_PART_SRCS org.freedesktop.Avahi.Server.xml org.freedesktop.Avahi.Server)

# Intermediate lib for use in testing.
add_library(kcm_samba_static STATIC ${kcm_samba_PART_SRCS})
target_link_libraries(kcm_samba_static
    KF5::KIOCore
    KF5::Solid
    KF5::I18n
    KF5::KCMUtils
    KF5::KIOWidgets
)

set(kcm_samba_SRCS main.cpp)
qt_add_dbus_interface(kcm_samba_SRCS org.freedesktop.DBus.Properties.xml org.freedesktop.DBus.Properties)
add_library(kcm_samba MODULE ${kcm_samba_SRCS})
target_link_libraries(kcm_samba
    KF5::QuickAddons
    kcm_samba_static
)

kcoreaddons_desktop_to_json(kcm_samba smbstatus.desktop)

install(TARGETS kcm_samba DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
install(FILES smbstatus.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
kpackage_install_package(package kcmsamba kcms) # NB: the target name follows the kaboutdata name which in turn follows the i18n domain name

add_subdirectory(autotests)
