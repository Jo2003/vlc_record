#!/bin/bash

NAME=${1}
OFFNAME=""
SSERVER=""
ARCH=

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
   *)
      OFFNAME=${NAME}
      ;;
esac

if [ "$(uname -m)" = "x86_64" ] ; then
    ARCH=amd64
else
    ARCH=i386
fi


# get release information ...
MAJORVER=`sed -n 's/^#define[ \t]*VERSION_MAJOR[^0-9]\+\([^"]\+\).*/\1/p' version_info.h`
MINORVER=`sed -n 's/^#define[ \t]*VERSION_MINOR[^0-9]\+\([^"]\+\).*/\1/p' version_info.h`
BETAEXT=`sed -n 's/^#define[ \t]*BETA_EXT[^0-9B]\+\([^"]\+\).*/\1/p' version_info.h`

# most of the work will be done by the new create_deb.sh script!
cat << EOF >install.mak
all: deb

deb:
	./create_deb.sh ${NAME} ${OFFNAME} ${MAJORVER}.${MINORVER}${BETAEXT} ${SSERVER} ${ARCH}

EOF
