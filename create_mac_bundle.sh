#!/bin/bash
APPNAME=${1}
TMPFILE=/tmp/plist.tmp
CONTENTS=$APPNAME.app/Contents
OFFNAME=""
if [ -z $QTTRANS ] ; then
    QTTRANS=/Users/joergn/Qt/4.8.1/translations
fi

# create official name ...
case $APPNAME in
    kartina_tv)
        OFFNAME="Kartina.TV"
        ;;
    polsky_tv)
        OFFNAME="Polsky.TV"
        ;;
    vlc-record)
        OFFNAME="VLC-Record"
        ;;
    afrobox)
        OFFNAME="AfroBox"
        ;;
    *)
        OFFNAME=$APPNAME
        ;;
esac

# get version info from version_info.h header ...
MINORVER=`sed -n 's/^#define[ \t]*VERSION_MINOR[^0-9]*\([^"]*\).*/\1/p' version_info.h`
BETAEXT=`sed -n 's/^#define[ \t]*BETA_EXT[^0-9B]*\([^"]*\).*/\1/p' version_info.h`
DATESTR=`date +%Y%m%d`

# create destination folder name ...
DSTFOLDER=$APPNAME-2.${MINORVER}${BETAEXT}-${DATESTR}-mac
cd release
rm -rf $APPNAME-*-mac*
mkdir -p $CONTENTS/Resources/language
mkdir -p $CONTENTS/PlugIns/modules
mkdir -p $CONTENTS/Frameworks
mkdir -p $CONTENTS/translations
cp ../*.qm $CONTENTS/Resources/language/
cp ../modules/*.mod $CONTENTS/PlugIns/modules/
cp ../resources/$APPNAME.icns $CONTENTS/Resources/$APPNAME.icns
cp -R ../mac/* $CONTENTS/MacOS

# copy Qt translations
cp $QTTRANS/qt_de.qm $CONTENTS/translations
cp $QTTRANS/qt_fr.qm $CONTENTS/translations
cp $QTTRANS/qt_pl.qm $CONTENTS/translations
cp $QTTRANS/qt_ru.qm $CONTENTS/translations

# patch Info.plist file ...
sed -e "s/$APPNAME.rc/$APPNAME.icns/g" -e "s/yourcompany/Jo2003/g" -e "s/<\/dict>/	<key>CFBundleName<\/key>\\
	<string>$OFFNAME<\/string>\\
<\/dict>/g" $CONTENTS/Info.plist >$TMPFILE
iconv -f ASCII -t UTF-8 $TMPFILE >$CONTENTS/Info.plist
macdeployqt $APPNAME.app -verbose=0
POS=`pwd`
cd $CONTENTS/Frameworks
rm -rf QtDeclarative.framework
rm -rf QtScript.framework
rm -rf QtSvg.framework
rm -rf QtXmlPatterns.framework

cd $POS
mkdir -p $DSTFOLDER
mv $APPNAME.app $DSTFOLDER/$OFFNAME.app
ln -s /Applications $DSTFOLDER/

cd ..

