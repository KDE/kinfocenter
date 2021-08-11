# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_interrupts\")

add_library(kcm_interrupts MODULE main.cpp)
target_link_libraries(kcm_interrupts KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

kcoreaddons_desktop_to_json(kcm_interrupts "kcm_interrupts.desktop")

install(FILES kcm_interrupts.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_interrupts DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)

kpackage_install_package(package kcm_interrupts kcms)
