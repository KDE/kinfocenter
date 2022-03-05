# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>

find_package(fwupdmgr)
set_package_properties(fwupdmgr PROPERTIES TYPE RUNTIME)

find_package(aha)
set_package_properties(aha PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_firmware_security\")

add_library(kcm_firmware_security MODULE main.cpp)
target_link_libraries(kcm_firmware_security KF5::CoreAddons KF5::QuickAddons KF5::I18n KF5::Package KInfoCenterInternal)

install(TARGETS kcm_firmware_security DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_firmware_security kcms)
