#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
# SPDX-FileCopyrightText: 2025 Michał Kula <michal.kula@kdemail.net>

set -e

DARKNESS="$1"

AHA_ARGS=""
if [ "$DARKNESS" -eq "1" ]; then
    # Use colors that look better in dark mode with --black, but revert to initial colors,
    # since --black also sets explicit background and text colors to black and white respectively.
    AHA_ARGS="--black --style 'color:initial' --style 'background-color:initial'"
fi

# The sed line prepends a newline before every interface entry other than the first one
# (so before entries in index range 2-9, or anything double-or-more digit).

# For sed testing please use --posix on GNU sed so you don't end up using gnu-isms.
ip -color -statistics -human-readable address 2>&1 \
    | sed -E 's@^([2-9]|[0-9]{2,}):@\n\1:@g' \
    | aha $AHA_ARGS
