# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

add_executable(kinfocenter-opengl-helper main.cpp)
target_link_libraries(kinfocenter-opengl-helper Qt::Gui)
install(TARGETS kinfocenter-opengl-helper DESTINATION ${KDE_INSTALL_LIBEXECDIR})
