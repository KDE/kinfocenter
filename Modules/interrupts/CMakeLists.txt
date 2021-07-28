# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_interrupts\")

kcoreaddons_add_plugin(kcm_interrupts SOURCES main.cpp INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
target_link_libraries(kcm_interrupts KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

kcoreaddons_desktop_to_json(kcm_interrupts "kcm_interrupts.desktop")

kpackage_install_package(package kcm_interrupts kcms)
