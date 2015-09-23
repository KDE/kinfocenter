#! /usr/bin/env bash
$XGETTEXT `find . -name "*.cpp" -o -name "*.qml"` -o $podir/kcm_fileindexermonitor.pot
rm -f rc.cpp
