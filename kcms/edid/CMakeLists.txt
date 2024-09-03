# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021-2024 Harald Sitter <sitter@kde.org>

find_package(di-edid-decode)
set_package_properties(di-edid-decode PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_edid\")

kinfocenter_add_kcm(kcm_edid main.cpp)
target_link_libraries(kcm_edid PRIVATE KF6::CoreAddons KF6::KCMUtilsQuick KF6::I18n KInfoCenterInternal)

install(FILES edid.sh DESTINATION ${KDE_INSTALL_DATADIR}/kinfocenter/edid)
