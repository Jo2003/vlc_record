#!/bin/bash
TMPFILE=/tmp/plist.tmp
CONTENTS=vlc-record.app/Contents

MINORVER=`cat version.mak | awk -F '=' '{if($1 == "MINORVER"){print $2}}'`
MAJORVER=`cat version.mak | awk -F '=' '{if($1 == "CLMAJORVER"){print $2}}'`
DATESTR=`date +%Y%m%d`
cd release
mkdir -p $CONTENTS/Resources/language
mkdir -p $CONTENTS/PlugIns/modules
cp ../*.qm $CONTENTS/Resources/language/
cp ../modules/*.mod $CONTENTS/PlugIns/modules/
rm -f $CONTENTS/PlugIns/modules/*libvlc*
cp ../resources/vlc-record.icns $CONTENTS/Resources/
sed -e 's/vlc-record.rc/vlc-record.icns/g' -e 's/yourcompany/Jo2003/g' $CONTENTS/Info.plist >$TMPFILE
iconv -f ASCII -t UTF-8 $TMPFILE >$CONTENTS/Info.plist
macdeployqt vlc-record.app -dmg
mv vlc-record.dmg vlc-record-${MAJORVER}.${MINORVER}-${DATESTR}-mac.dmg
cd ..

