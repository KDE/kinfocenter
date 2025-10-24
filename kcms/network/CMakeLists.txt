# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

find_package(ip)
set_package_properties(ip PROPERTIES TYPE RUNTIME)

find_package(aha)
set_package_properties(aha PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_network\")

kinfocenter_add_kcm(kcm_network main.cpp)
target_link_libraries(kcm_network PRIVATE Qt::Gui KF6::CoreAddons KF6::KCMUtilsQuick KF6::I18n KInfoCenterInternal)

install(FILES ip.sh DESTINATION ${KDE_INSTALL_DATADIR}/kinfocenter/network)
