# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>

find_package(sensors)
set_package_properties(sensors PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_sensors\")

kinfocenter_add_kcm(kcm_sensors main.cpp)
target_link_libraries(kcm_sensors PRIVATE KF6::CoreAddons KF6::KCMUtilsQuick KF6::I18n KInfoCenterInternal)
