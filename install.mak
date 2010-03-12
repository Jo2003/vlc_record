INSTALL=/usr/bin/install
TARGETNW=/opt/vlc-record
TARGETCL=/opt/vlc-rcd-classic
SUDO=/usr/bin/sudo
CHKINST=/usr/bin/checkinstall

include version.mak
include checkinstall.mak

all: debnew

instclassic: install_base

instnew: install_base install_libvlc

install_base:
	${INSTALL} -d ${TARGET}
	${INSTALL} -d ${TARGET}/language
	${INSTALL} -d ${TARGET}/modules
	${INSTALL} -s -t ${TARGET} release/vlc-record
	${INSTALL} -m 644 -t ${TARGET}/language lang_*.qm
	${INSTALL} -m 644 -t ${TARGET}/modules modules/1_vlc-player.mod
	${INSTALL} -m 644 -t ${TARGET}/modules modules/2_MPlayer.mod
	${INSTALL} -m 644 -t ${TARGET}/modules modules/3_vlc-mp4.mod
	${INSTALL} -m 644 -t ${TARGET}/modules modules/4_vlc-player-avi.mod
	${INSTALL} -m 644 -t ${TARGET} resources/television.png

install_libvlc:
	${INSTALL} -m 644 -t ${TARGET}/modules modules/5_libvlc.mod

debnew:
	${SUDO} ${CHKINST} ${PKGNAMENW} ${PKGVERNW} ${MAINT} ${REQUIRES} ${PKGSRC} ${MAKE} TARGET=${TARGETNW} -f install.mak instnew
	
debclc:
	${SUDO} ${CHKINST} ${PKGNAMECL} ${PKGVERCL} ${MAINT} ${REQUIRES} ${PKGSRC} ${MAKE} TARGET=${TARGETCL} -f install.mak instclassic
