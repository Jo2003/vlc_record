#!/bin/bash
APPNAME=${1}
TMPFILE=/tmp/plist.tmp
CONTENTS=$APPNAME.app/Contents
OFFNAME=""
if [ -z $QTDIR ] ; then
    QTDIR=~/Qt/4.8.5
fi
QTTRANS=$QTDIR/translations

# create official name ...
case $APPNAME in
    moidom_tv)
        OFFNAME="Moi-Dom.TV"
        ;;
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
    iptv-record)
        OFFNAME="Iptv-Record"
        ;;
    tvclub)
        OFFNAME="TVClub.us"
        ;;
    *)
        OFFNAME=$APPNAME
        ;;
esac

# get version info from version_info.h header ...
MAJORVER=`sed -n 's/^#define[ \t]*VERSION_MAJOR[^0-9]*\([0-9]*\).*/\1/p' version_info.h`
MINORVER=`sed -n 's/^#define[ \t]*VERSION_MINOR[^0-9]*\([0-9]*\).*/\1/p' version_info.h`
BUILDVER=`sed -n 's/^#define[ \t]*VERSION_BUILD[^0-9]*\([0-9]*\).*/\1/p' version_info.h`
BETAEXT=`sed -n 's/^#define[ \t]*BETA_EXT[^0-9B]*\([^"]*\).*/\1/p' version_info.h`
DATESTR=`date +%Y%m%d`

# create destination folder name ...
DSTFOLDER=$APPNAME-${MAJORVER}.${MINORVER}.${BUILDVER}${BETAEXT}-${DATESTR}-mac
cd release
rm -rf $APPNAME-*-mac*
mkdir -p $CONTENTS/Resources/language
mkdir -p $CONTENTS/Resources/doc
mkdir -p $CONTENTS/PlugIns/modules
mkdir -p $CONTENTS/Frameworks
mkdir -p $CONTENTS/translations
cp ../*.qm $CONTENTS/Resources/language/
cp ../modules/*.mod $CONTENTS/PlugIns/modules/
cp ../resources/$APPNAME.icns $CONTENTS/Resources/$APPNAME.icns
cp ../qhc/$OFFNAME/*.qhc $CONTENTS/Resources/doc/
cp ../qhc/$OFFNAME/*.qch $CONTENTS/Resources/doc/
cp -a ../mac/* $CONTENTS/MacOS

# copy Qt translations
cp $QTTRANS/qt_de.qm $CONTENTS/translations
cp $QTTRANS/qt_fr.qm $CONTENTS/translations
cp $QTTRANS/qt_pl.qm $CONTENTS/translations
cp $QTTRANS/qt_ru.qm $CONTENTS/translations

# create Info.plist file ...
cat << EOF > $TMPFILE
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist SYSTEM "file://localhost/System/Library/DTDs/PropertyList.dtd">
<plist version="0.9">
<dict>
	<key>CFBundleIconFile</key>
	<string>$APPNAME.icns</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleGetInfoString</key>
	<string>Created by Qt/QMake</string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>CFBundleExecutable</key>
	<string>$APPNAME</string>
	<key>CFBundleIdentifier</key>
	<string>com.Jo2003.$APPNAME</string>
	<key>CFBundleName</key>
	<string>$OFFNAME</string>
</dict>
</plist>
EOF

iconv -f ASCII -t UTF-8 $TMPFILE >$CONTENTS/Info.plist
$QTDIR/bin/macdeployqt $APPNAME.app -verbose=0
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

