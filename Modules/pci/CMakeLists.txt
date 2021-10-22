# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(lspci)
set_package_properties(lspci PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pci\")

add_library(kcm_pci MODULE main.cpp)
target_link_libraries(kcm_pci KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

kcoreaddons_desktop_to_json(kcm_pci "kcm_pci.desktop")

install(TARGETS kcm_pci DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_pci kcms)
install(PROGRAMS kcm_pci.desktop DESTINATION  ${KDE_INSTALL_DATADIR}/kcms)
