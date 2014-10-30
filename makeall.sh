#!/bin/bash
ARCH="i386"

if [ "$(uname -i)" == "x86_64" ] ; then
	ARCH="amd64"
fi

CUSTOMS="vlc-record kartina_tv polsky_tv"

for i in $CUSTOMS; do
	sudo rm -rf ../${i}_2.*${ARCH}*
	qmake-qt4 $i.pro
	make clean
	make -j4 release
	sudo make -f install.mak
done

