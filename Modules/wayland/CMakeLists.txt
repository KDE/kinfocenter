# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(wayland-info)
set_package_properties(wayland-info PROPERTIES
    TYPE RUNTIME
    URL "https://gitlab.freedesktop.org/wayland/wayland-utils"
    PURPOSE "Wayland KCM")

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_wayland\")

add_library(kcm_wayland MODULE main.cpp)
target_link_libraries(kcm_wayland KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

kcoreaddons_desktop_to_json(kcm_wayland "kcm_wayland.desktop")

install(TARGETS kcm_wayland DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_wayland kcms)
