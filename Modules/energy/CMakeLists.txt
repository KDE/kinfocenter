# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_energyinfo\")

configure_file (config-kcm.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kcm.h )

set(kcm_energyinfo_SRCS
  kcm.cpp
  batterymodel.cpp
  wakeupmodel.cpp
  statisticsprovider.cpp
)

add_library(kcm_energyinfo MODULE ${kcm_energyinfo_SRCS})

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

kcoreaddons_desktop_to_json(kcm_energyinfo "kcm_energyinfo.desktop")

install(FILES kcm_energyinfo.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(TARGETS kcm_energyinfo DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)

kpackage_install_package(package kcm_energyinfo kcms)
