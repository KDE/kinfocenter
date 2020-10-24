# - Find Plasma's SystemSettings
# This module defines the following variables:
#
#  SystemSettings_FOUND - true if found
#  SystemSettings_PATH - path to the bin (only when found)
#
# SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
#
# SPDX-License-Identifier: BSD-2-Clause

find_program(SystemSettings_PATH "systemsettings5"
    PATHS ${KDE_INSTALL_FULL_BINDIR} # We symlink by absolute path, so this is the only valid location!
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SystemSettings
    FOUND_VAR SystemSettings_FOUND
    REQUIRED_VARS SystemSettings_PATH
)
mark_as_advanced(SystemSettings_PATH)
