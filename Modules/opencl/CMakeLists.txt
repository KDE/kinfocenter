# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
# SPDX-FileCopyrightText: 2022 Linus Dierheimer <linus@dierheimer.de>
#
# SPDX-License-Identifier: BSD-3-Clause

find_package(clinfo)
set_package_properties(clinfo PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_opencl\")

add_library(kcm_opencl MODULE main.cpp)
target_link_libraries(kcm_opencl KF6::CoreAddons KF6::KCMUtilsQuick KF6::I18n KInfoCenterInternal)

install(TARGETS kcm_opencl DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_opencl kcms)
