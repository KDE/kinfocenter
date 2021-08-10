# This module defines the following variables:
#
#  vulkaninfo_FOUND - true if found
#  vulkaninfo_PATH - path to the bin (only when found)
#
# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_program(vulkaninfo_PATH vulkaninfo)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(vulkaninfo
    FOUND_VAR vulkaninfo_FOUND
    REQUIRED_VARS vulkaninfo_PATH
)
mark_as_advanced(vulkaninfo_PATH)
