#!/bin/bash

CUSTOMS="vlc-record kartina_tv polsky_tv"

sudo rm -f *.deb

for i in $CUSTOMS; do
	qmake $i.pro
	make clean
	make -j4 release
	sudo make -f install.mak
done


