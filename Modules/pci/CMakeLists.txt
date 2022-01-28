# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(lspci)
set_package_properties(lspci PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pci\")

add_library(kcm_pci MODULE main.cpp)
target_link_libraries(kcm_pci KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

install(TARGETS kcm_pci DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_pci kcms)
