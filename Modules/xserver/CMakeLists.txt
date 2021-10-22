# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(xdpyinfo)
set_package_properties(xdpyinfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_xserver\")

add_library(kcm_xserver MODULE main.cpp)
target_link_libraries(kcm_xserver KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

kcoreaddons_desktop_to_json(kcm_xserver "kcm_xserver.desktop")

install(TARGETS kcm_xserver DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_xserver kcms)
install(PROGRAMS kcm_xserver.desktop DESTINATION  ${KDE_INSTALL_DATADIR}/kcms)
