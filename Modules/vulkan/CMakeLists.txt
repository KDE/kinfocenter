# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(vulkaninfo)
set_package_properties(vulkaninfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_vulkan\")

add_library(kcm_vulkan MODULE main.cpp)
target_link_libraries(kcm_vulkan KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

kcoreaddons_desktop_to_json(kcm_vulkan "kcm_vulkan.desktop")

install(FILES kcm_vulkan.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_vulkan DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)

kpackage_install_package(package kcm_vulkan kcms)
