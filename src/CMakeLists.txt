# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

# TODO in systemsettings we had trouble with static libs in modules. make sure this doesn't fall us on the head
add_library(KInfoCenterInternal STATIC CommandOutputContext.cpp)
target_link_libraries(KInfoCenterInternal Qt::Qml KF5::I18n)
# Disable legacy stuff to get rid of some deprecation warnings. Notably duplicated QProcess::finished overloads.
target_compile_definitions(KInfoCenterInternal PRIVATE -DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
target_include_directories(KInfoCenterInternal INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

install(DIRECTORY qml/ DESTINATION ${QML_INSTALL_DIR}/org/kde/kinfocenter/private)
