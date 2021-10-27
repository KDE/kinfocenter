# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_energyinfo\")

set(kcm_energyinfo_SRCS
  kcm.cpp
  batterymodel.cpp
  wakeupmodel.cpp
  statisticsprovider.cpp
)

kcoreaddons_add_plugin(kcm_energyinfo SOURCES ${kcm_energyinfo_SRCS} INSTALL_NAMESPACE "plasma/kcms")
kcoreaddons_desktop_to_json(kcm_energyinfo "kcm_energyinfo.desktop")

target_link_libraries(kcm_energyinfo
  Qt::DBus
  Qt::Widgets
  KF5::KIOWidgets
  KF5::CoreAddons
  KF5::KCMUtils
  KF5::I18n
  KF5::Solid
  KF5::QuickAddons
)

kpackage_install_package(package kcm_energyinfo kcms)
