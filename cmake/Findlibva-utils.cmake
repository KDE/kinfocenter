# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2026 Satakun Utama <linesofcodes@dailitation.xyz>
#
# This module defines the following variables:
#
#  libva-utils_FOUND - true if found
#  libva-utils_PATH - path to the bin (only when found)

include(ProgramFinder)
program_finder(vainfo)
