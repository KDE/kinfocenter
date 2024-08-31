# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

add_executable(kinfocenter-gpu-helper main.cpp)
target_link_libraries(kinfocenter-gpu-helper Qt::Gui)
install(TARGETS kinfocenter-gpu-helper DESTINATION ${KDE_INSTALL_LIBEXECDIR})
