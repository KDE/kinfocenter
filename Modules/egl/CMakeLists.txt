# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(eglinfo)
set_package_properties(eglinfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_egl\")

add_library(kcm_egl MODULE main.cpp)
target_link_libraries(kcm_egl KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

kcoreaddons_desktop_to_json(kcm_egl "kcm_egl.desktop")

install(TARGETS kcm_egl DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/kcms/kinfocenter)
install(PROGRAMS kcm_egl.desktop DESTINATION  ${KDE_INSTALL_DATADIR}/kcms)

kpackage_install_package(package kcm_egl kcms)
