#!/usr/bin/env bash
$XGETTEXT  `find . -name '*.cpp' -o -name '*.h' -o -name '*.qml' -o -name '*.js'` -o $podir/kcmsamba.pot
