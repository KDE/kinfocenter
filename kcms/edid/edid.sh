#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
# SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

set -eu

for card in /sys/class/drm/card*; do
    edid="$card/edid"
    if [ ! -e "$edid" ]; then
        continue
    fi

    # Try to read any data. If it fails continue. This prevents errors from di-edid-decode.
    IFS= read -r data < "$edid" || continue

    data=$(di-edid-decode "$edid" 2>&1) || true
    printf "# %s #######################\n" "$(basename "$card")"
    printf "%s\n\n" "$data"
done
