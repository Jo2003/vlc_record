#!/bin/bash
TMPFILE=/tmp/plist.tmp
CONTENTS=vlc-record.app/Contents

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
cd ..

