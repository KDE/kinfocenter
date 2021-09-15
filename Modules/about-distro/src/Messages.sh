#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2012 Harald Sitter <sitter@kde.org>
$EXTRACTRC `find . -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT `find . -name \*.cc -o -name \*.cpp -o -name \*.h` -o $podir/kcm_about-distro.pot
rm -f rc.cpp
