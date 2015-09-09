#!/bin/bash

NAME=${1}
OFFNAME=""
SSERVER=""

# create official name ...
case ${NAME} in
   vlc-record)
      OFFNAME="VLC-Record"
      SSERVER="Kartina.TV"
      ;;
   kartina_tv)
      OFFNAME="Kartina.TV"
      SSERVER=$OFFNAME
      ;;
   polsky_tv)
      OFFNAME="Polsky.TV"
      SSERVER=$OFFNAME
      ;;
   afrobox)
      OFFNAME="AfroBox"
      SSERVER=$OFFNAME
      ;;
   moidom_tv)
      OFFNAME="Moi-Dom.TV"
      SSERVER=$OFFNAME
      ;;
   iptv-record)
      OFFNAME="Iptv-Record"
      SSERVER="some Iptv stations"
      ;;
   sunduk_tv)
      OFFNAME="Sunduk.TV"
      SSERVER=$OFFNAME
      ;;
   stalker)
      OFFNAME="Stalker.TV"
      SSERVER=$OFFNAME
      ;;
   *)
      OFFNAME=${NAME}
      ;;
esac


# get release information ...
MINORVER=`sed -n 's/^#define[ \t]*VERSION_MINOR[^0-9]\+\([^"]\+\).*/\1/p' version_info.h`
BETAEXT=`sed -n 's/^#define[ \t]*BETA_EXT[^0-9B]\+\([^"]\+\).*/\1/p' version_info.h`

# create application launcher ...
cat << EOF >$NAME.desktop
[Desktop Entry]
Name=${OFFNAME}
Comment=A tool to watch / record IPTV program streams from ${OFFNAME}.
Exec=/usr/bin/${NAME}
Terminal=false
Type=Application
Icon=/usr/share/${NAME}/${NAME}.png
Categories=Application;VideoPlayer;

EOF

# create install makefile ...
cat << EOF >install.mak
INSTALL=install
TARGETNW=/usr
TARGETCL=/usr
PROGFOLDER=${NAME}
PROGNEW=${NAME}
PROGCLC=${NAME}-classic

CHKINST=checkinstall

PKGNAMENW=--pkgname ${NAME}
PKGNAMECL=--pkgname ${NAME}-classic
REQUIRES=--requires "libqtcore4, libqtgui4, libqt4-network, libqt4-sql-sqlite, vlc, libvlc5, libqt4-xml, libqt4-help, gtk2-engines-pixbuf"
REQUIRESCLC=--requires "libqtcore4, libqtgui4, libqt4-network, libqt4-sql-sqlite, vlc, libqt4-xml, libqt4-help, gtk2-engines-pixbuf"
MAINT=--maintainer "Jo2003 \(coujo@gmx.net\)"
PKGVERCL=--pkgversion 1.${MINORVER}${BETAEXT}
PKGVERNW=--pkgversion 2.${MINORVER}${BETAEXT}
PKGGRP=--pkggroup video
PKGSRC=--pkgsource "https://code.google.com/p/vlc-record"

all: debnew

descr:
	echo "A tool to watch / record IPTV streams from $SSERVER" > description-pak

instclassic: install_base

instnew: install_libvlc

rename:
	mv release/\${PROGNEW} release/\${PROGCLC}

install_base:
	\${INSTALL} -m 755 -d \${TARGET}/share/\${PROGFOLDER}/
	\${INSTALL} -m 755 -d \${TARGET}/share/\${PROGFOLDER}/language/
	\${INSTALL} -m 755 -d \${TARGET}/share/\${PROGFOLDER}/modules/
	\${INSTALL} -m 755 -d \${TARGET}/share/\${PROGFOLDER}/doc/
	\${INSTALL} -s -t \${TARGET}/bin release/\${PROG}
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/language lang_*.qm
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/doc qhc/${OFFNAME}/*.qch
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/doc qhc/${OFFNAME}/*.qhc
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/1_vlc-player.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/2_MPlayer.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/3_vlc-mp4.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/4_vlc-player-avi.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/7_vlc-mpeg2.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/10_vlc-player_odl.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER} resources/${NAME}.png
	\${INSTALL} -m 644 -t \${TARGET}/share/applications ${NAME}.desktop

install_libvlc: install_base
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/5_libvlc.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/6_libvlc-mpeg2.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/8_libvlc_xvid_avi.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/9_libvlc_odl.mod
	\${INSTALL} -m 644 -t \${TARGET}/share/\${PROGFOLDER}/modules modules/11_libvlc-mp4.mod

debnew: descr
	\${CHKINST} -y \${PKGNAMENW} \${PKGGRP} \${PKGVERNW} \${MAINT} \${REQUIRES} \${PKGSRC} \${MAKE} TARGET=\${TARGETNW} PROG=\${PROGNEW} -f install.mak instnew

debclc: descr rename
	\${CHKINST} -y \${PKGNAMECL} \${PKGGRP} \${PKGVERCL} \${MAINT} \${REQUIRESCLC} \${PKGSRC} \${MAKE} TARGET=\${TARGETCL} PROG=\${PROGCLC} -f install.mak instclassic

EOF
