#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
# SPDX-FileCopyrightText: 2025 Michał Kula <michal.kula@kdemail.net>

set -e

# For sed testing please use --posix on GNU sed so you don't end up using gnu-isms.
ip -color -statistics -human-readable address 2>&1 \
    | sed -E 's@^(([2-9]|[0-9]{2,}):)@\n\1@g' \
    | aha
