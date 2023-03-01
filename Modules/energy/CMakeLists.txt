# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_energyinfo\")

set(kcm_energyinfo_SRCS
    batterymodel.cpp
    batterymodel.h
    kcm.cpp
    kcm.h
    statisticsprovider.cpp
    statisticsprovider.h
)

kcoreaddons_add_plugin(kcm_energyinfo SOURCES ${kcm_energyinfo_SRCS} INSTALL_NAMESPACE "plasma/kcms")

target_link_libraries(kcm_energyinfo
  Qt::DBus
  Qt::Widgets
  KF6::KIOWidgets
  KF6::CoreAddons
  KF6::KCMUtils
  KF6::I18n
  KF6::Solid
  KF6::QuickAddons
)

kpackage_install_package(package kcm_energyinfo kcms)
