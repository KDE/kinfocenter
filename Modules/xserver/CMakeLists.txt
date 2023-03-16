# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(xdpyinfo)
set_package_properties(xdpyinfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_xserver\")

add_library(kcm_xserver MODULE main.cpp)
target_link_libraries(kcm_xserver KF6::CoreAddons KF6::KCMUtilsQuick KF6::I18n KInfoCenterInternal)

install(TARGETS kcm_xserver DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_xserver kcms)
