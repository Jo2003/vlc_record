#!/bin/bash

QMAKE=~/Qt/4.8.5/bin/qmake
CUSTOMS="vlc-record kartina_tv polsky_tv"

for i in $CUSTOMS; do
	$QMAKE $i.pro
	make clean
	make -j4 release
done

cd release && ./makeall_dmg.sh
