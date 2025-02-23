# This module defines the following variables:
#
#  sensors_FOUND - true if found
#  sensors_PATH - path to the bin (only when found)
#
# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>

include(ProgramFinder)
program_finder(sensors)
