# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_interrupts\")

kcoreaddons_add_plugin(kcm_interrupts SOURCES main.cpp INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
target_link_libraries(kcm_interrupts KF6::CoreAddons KF6::KCMUtilsQuick KF6::I18n KInfoCenterInternal)

kpackage_install_package(package kcm_interrupts kcms)
