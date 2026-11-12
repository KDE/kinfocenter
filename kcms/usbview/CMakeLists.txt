# SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

find_package(lsusb)
set_package_properties(lsusb PROPERTIES TYPE RUNTIME)

add_definitions(-DTRANSLATION_DOMAIN=\"kcmusb\")

kinfocenter_add_kcm(kcm_usb
    SOURCES
        kcmusb.cpp
        kcmusb.h
)

target_link_libraries(kcm_usb PRIVATE
    KF6::CoreAddons
    KF6::KCMUtilsQuick
    KF6::I18n
    KInfoCenterInternal
)
