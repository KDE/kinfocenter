#!/bin/sh
$EXTRACTRC *.rc >> rc.cpp
$XGETTET *.cpp -o $podir/kinfocenter.pot
