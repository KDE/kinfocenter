# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
# SPDX-FileCopyrightText: 2022 Nate Graham <nate@kde.org>

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_kwinsupportinfo\")

add_library(kcm_kwinsupportinfo MODULE main.cpp)
target_link_libraries(kcm_kwinsupportinfo KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

install(TARGETS kcm_kwinsupportinfo DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_kwinsupportinfo kcms)
