# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(glxinfo)
set_package_properties(glxinfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_glx\")

add_library(kcm_glx MODULE main.cpp)
target_link_libraries(kcm_glx KF6::CoreAddons KF6::QuickAddons KF6::I18n KInfoCenterInternal)

install(TARGETS kcm_glx DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_glx kcms)
