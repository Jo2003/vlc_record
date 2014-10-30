#!/bin/bash

CUSTOMS="vlc-record kartina_tv polsky_tv"

for i in $CUSTOMS; do
	rm -rf ../${i}_2.*
	qmake $i.pro
	make clean
	make -j4 release
	sudo make -f install.mak
done


