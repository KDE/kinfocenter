# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(wayland-info)
set_package_properties(wayland-info PROPERTIES
    TYPE RUNTIME
    URL "https://gitlab.freedesktop.org/wayland/wayland-utils"
    PURPOSE "Wayland KCM")

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_wayland\")

add_library(kcm_wayland MODULE main.cpp)
target_link_libraries(kcm_wayland KF6::CoreAddons KF6::QuickAddons KF6::I18n KInfoCenterInternal)

install(TARGETS kcm_wayland DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_wayland kcms)
