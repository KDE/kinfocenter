# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024-2026 Harald Sitter <sitter@kde.org>

add_executable(kinfocenter-egl-helper main.cpp)
target_link_libraries(kinfocenter-egl-helper
    PRIVATE
        Qt::Gui
        PkgConfig::egl
)
install(TARGETS kinfocenter-egl-helper DESTINATION ${KDE_INSTALL_LIBEXECDIR})
