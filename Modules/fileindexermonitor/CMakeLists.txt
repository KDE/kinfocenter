# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_fileindexermonitor\")

set(kcm_fileindexer_SRCS
  kcm.cpp
)

add_library(kcm_fileindexermonitor MODULE ${kcm_fileindexer_SRCS})

target_link_libraries(kcm_fileindexermonitor
  Qt5::Widgets
  KF5::KIOWidgets
  KF5::CoreAddons
  KF5::KCMUtils
  KF5::I18n
  KF5::QuickAddons
)

kcoreaddons_desktop_to_json(kcm_fileindexermonitor "kcm_fileindexermonitor.desktop")

install(FILES kcm_fileindexermonitor.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_fileindexermonitor DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)

kpackage_install_package(package kcm_fileindexermonitor kcms)
