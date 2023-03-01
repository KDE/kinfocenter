# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(vulkaninfo)
set_package_properties(vulkaninfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_vulkan\")

add_library(kcm_vulkan MODULE main.cpp)
target_link_libraries(kcm_vulkan KF6::CoreAddons KF6::QuickAddons KF6::I18n KInfoCenterInternal)

install(TARGETS kcm_vulkan DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_vulkan kcms)
