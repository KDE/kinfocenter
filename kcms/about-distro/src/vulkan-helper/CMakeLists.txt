# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

add_executable(kinfocenter-vulkan-helper main.cpp)
target_link_libraries(kinfocenter-vulkan-helper Qt::Gui)
install(TARGETS kinfocenter-vulkan-helper DESTINATION ${KDE_INSTALL_LIBEXECDIR})
