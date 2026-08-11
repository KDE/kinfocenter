# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2026 Satakun Utama <linesofcodes@dailitation.xyz>

find_package(libva-utils)
set_package_properties(libva-utils PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_vaapi\")

kinfocenter_add_kcm(kcm_vaapi main.cpp)

target_link_libraries(kcm_vaapi PRIVATE KF6::CoreAddons KF6::KCMUtilsQuick KF6::I18n KInfoCenterInternal)
