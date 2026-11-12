# This module defines the following variables:
#
#  lsusb_FOUND - true if found
#  lsusb_PATH - path to the bin (only when found)
#
# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>

include(ProgramFinder)
program_finder(lsusb)
