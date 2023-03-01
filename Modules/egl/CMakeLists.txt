# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(eglinfo)
set_package_properties(eglinfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_egl\")

add_library(kcm_egl MODULE main.cpp)
target_link_libraries(kcm_egl KF6::CoreAddons KF6::QuickAddons KF6::I18n KInfoCenterInternal)

install(TARGETS kcm_egl DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_egl kcms)
