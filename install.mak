INSTALL=install
TARGETNW=/usr
TARGETCL=/usr
PROGFOLDER=Kartina.TV
PROGNEW=kartina_tv
PROGCLC=kartina_tv-classic
# SUDO=sudo
CHKINST=checkinstall

include version.mak
include checkinstall.mak

all: debnew

instclassic: install_base

instnew: install_libvlc

rename:
	mv release/${PROGNEW} release/${PROGCLC}

install_base:
	${INSTALL} -m 755 -d ${TARGET}/share/${PROGFOLDER}/
	${INSTALL} -m 755 -d ${TARGET}/share/${PROGFOLDER}/language/
	${INSTALL} -m 755 -d ${TARGET}/share/${PROGFOLDER}/modules/
	${INSTALL} -s -t ${TARGET}/bin release/${PROG}
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/language lang_*.qm
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/1_vlc-player.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/2_MPlayer.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/3_vlc-mp4.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/4_vlc-player-avi.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/7_vlc-mpeg2.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/10_vlc-player_odl.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER} resources/television.png

install_libvlc: install_base
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/5_libvlc.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/6_libvlc-mpeg2.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/8_libvlc_xvid_avi.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/9_libvlc_odl.mod
	${INSTALL} -m 644 -t ${TARGET}/share/${PROGFOLDER}/modules modules/11_libvlc-mp4.mod

debnew:
	${CHKINST} ${PKGNAMENW} ${PKGGRP} ${PKGVERNW} ${MAINT} ${REQUIRES} ${PKGSRC} ${MAKE} TARGET=${TARGETNW} PROG=${PROGNEW} -f install.mak instnew

debclc: rename
	${CHKINST} ${PKGNAMECL} ${PKGGRP} ${PKGVERCL} ${MAINT} ${REQUIRESCLC} ${PKGSRC} ${MAKE} TARGET=${TARGETCL} PROG=${PROGCLC} -f install.mak instclassic
