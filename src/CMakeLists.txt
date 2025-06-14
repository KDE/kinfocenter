# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

add_definitions(-DTRANSLATION_DOMAIN=\"kinfocenter\")
add_library(KInfoCenterInternal SHARED CommandOutputContext.cpp CommandOutputContext.h)
target_link_libraries(KInfoCenterInternal PRIVATE Qt::Qml KF6::I18n KF6::CoreAddons)
target_include_directories(KInfoCenterInternal INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

# NB: this is a private library that may not be linked to. Symbol visibility is of no concern. Neither is versioning.
set_target_properties(KInfoCenterInternal
    PROPERTIES
        C_VISIBILITY_PRESET default
        CXX_VISIBILITY_PRESET default
        VISIBILITY_INLINES_HIDDEN OFF)

ecm_add_qml_module(KInfoCenterInternal
    URI "org.kde.kinfocenter.private"
    VERSION 1.0
    GENERATE_PLUGIN_SOURCE
    QML_FILES qml/CommandOutputKCM.qml
)
ecm_finalize_qml_module(KInfoCenterInternal)

install(TARGETS KInfoCenterInternal ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
