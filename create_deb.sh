#!/bin/bash

#-----------------------------------------------------------------------
# Finally I found the time (and the need) to create a more or less
# good loking deb package!
# Many thanks to Ilja for the first steps already done!
# (c)2014 By Jo2003 All rights reserved!
#-----------------------------------------------------------------------
BIN_NAME=${1}
PACKAGE=$(echo -n ${BIN_NAME} | sed 's/_/-/')
OFF_NAME=${2}
VER="${3}-$(date +%Y%m%d)"
SERVICE=${4}
ARCH=${5}
QTVER=${6}
BUILD_FOLDER="$(pwd)/packages/${BIN_NAME}_${VER}_${ARCH}"

usage () {
    echo "Usage: ${0} [binary name] [official name] [version] [Service] [arch] [qt major version]"
}

create_folders() {
    rm -rf ${BUILD_FOLDER}
    mkdir -p "${BUILD_FOLDER}/usr/bin"
    mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/language"
    mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/modules"
    mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/doc"
    mkdir -p "${BUILD_FOLDER}/usr/share/${BIN_NAME}/resources"
    mkdir -p "${BUILD_FOLDER}/usr/share/doc/${PACKAGE}"
    mkdir -p "${BUILD_FOLDER}/usr/share/man/man7"
    mkdir -p "${BUILD_FOLDER}/usr/share/applications"
    mkdir -p "${BUILD_FOLDER}/DEBIAN"
}

copy_content() {
    strip -s release/${BIN_NAME}
    cp -f release/${BIN_NAME} "${BUILD_FOLDER}/usr/bin/"
    cp -f *.qm "${BUILD_FOLDER}/usr/share/${BIN_NAME}/language/"
    cp -f eula_*.txt "${BUILD_FOLDER}/usr/share/${BIN_NAME}/language/"
    cp -f modules/*.mod "${BUILD_FOLDER}/usr/share/${BIN_NAME}/modules/"
    cp -f qhc/${OFF_NAME}/* "${BUILD_FOLDER}/usr/share/${BIN_NAME}/doc/"
    cp -f resources/${BIN_NAME}.png "${BUILD_FOLDER}/usr/share/${BIN_NAME}/"
}

create_desktop_file() {
    cat << EOF > "${BUILD_FOLDER}/usr/share/applications/${OFF_NAME}.desktop"
[Desktop Entry]
Name=${OFF_NAME}
Comment=A tool to watch and record IPTV program streams from ${SERVICE}.
Exec=/usr/bin/${BIN_NAME}
Terminal=false
Type=Application
Icon=/usr/share/${BIN_NAME}/${BIN_NAME}.png
Categories=AudioVideo;Player;Recorder

EOF
}

create_control_file() {
    cat << EOF > "${BUILD_FOLDER}/DEBIAN/control"
Package: ${PACKAGE}
Version: ${VER}
Section: video
Priority: extra
Architecture: ${ARCH}
Installed-Size: $(($(du -b --max-depth=0 ${BUILD_FOLDER}/usr|gawk '{print $1}') / 1024))
EOF

if [ "${QTVER}" == "4" ] ; then
    cat << EOF >> "${BUILD_FOLDER}/DEBIAN/control"
Depends: gtk2-engines-pixbuf (>= 2.24.10), libqt4-help (>= 4:4.8.1), libqt4-network (>= 4:4.8.1), libqt4-sql-sqlite (>= 4:4.8.1), libqt4-xml (>= 4:4.8.1), libqtcore4 (>= 4:4.8.1), libqtgui4 (>= 4:4.8.1), libvlc5 (>= 2.0.3), vlc (>= 2.0.3), libc6 (>= 2.13)
EOF
else
    cat << EOF >> "${BUILD_FOLDER}/DEBIAN/control"
Depends: gtk2-engines-pixbuf (>= 2.24.10), libqt5help5 (>= 5.0.2), libqt5network5 (>= 5.0.2), libqt5sql5-sqlite (>= 5.0.2), libqt5xml5 (>= 5.0.2), libqt5core5a (>= 5.0.2), libqt5gui5 (>= 5.0.2), libvlc5 (>= 2.0.8), vlc (>= 2.0.8), libc6 (>= 2.15)
EOF
fi

cat << EOF >> "${BUILD_FOLDER}/DEBIAN/control"
Maintainer: Jo2003 <olenka.joerg@gmail.com>
Description: IPTV program stream player for ${SERVICE}
 It uses the Qt framework as well as libVLC from VLC player.
 Please note: You need to buy a subscription from ${SERVICE} to find this
 program to be useful!
Homepage: http://jo2003.github.io/vlc_record/

EOF
}

create_changelog() {
    cat << EOF | gzip -9 -c >"${BUILD_FOLDER}/usr/share/doc/${PACKAGE}/changelog.Debian.gz"
${PACKAGE} (${VER}) precise; urgency=minor

  * New release, makes us really happy!

 -- Jo2003 <olenka.joerg@gmail.com>  $(date -R)
EOF
    sed -n -e 's/^|\(.*\)$/\1/p' version_info.h | gzip -9 -c >"${BUILD_FOLDER}/usr/share/doc/${PACKAGE}/changelog.gz"
}

create_deb() {
    dpkg-deb --build "${BUILD_FOLDER}"
}

create_man_page() {
    cat << EOF | gzip -9 -c > "${BUILD_FOLDER}/usr/share/man/man7/${BIN_NAME}.7.gz"
.\" Manpage for ${BIN_NAME}.
.\" Contact olenka.joerg@gmail.com to correct errors or typos.
.TH man 7 "$(date -R)" "1.0" "${BIN_NAME} man page"
.SH NAME
${BIN_NAME} \- starts ${OFF_NAME} in GUI mode
.SH SYNOPSIS
${BIN_NAME}
.SH DESCRIPTION
For me it doesn't look like this program needs a man page. Nevertheless lintian wants one - so here it is. If you need help with the program use the nice looking in program help.
.SH OPTIONS
No options so far.
.SH BUGS
There are for sure bugs. If you find one please contact the author!
.SH AUTHOR
Jo2003 (olenka.joerg@gmail.com)
EOF
}

create_copyright_file() {
    cat << EOF > "${BUILD_FOLDER}/usr/share/doc/${PACKAGE}/copyright"
${OFF_NAME}
Copyright 2010-$(date +%Y) Jörg Neubert (olenka.joerg@gmail.com)
All rights reserved!
This program uses the libVLC from Videolans VLC-Player (http://www.videolan.org)
and the Qt framework (c) by Trolltech, Nokia, Digia, Qt-Project, who knows ...
${OFF_NAME} is released under the GPL 3.
A copy of this license can be found here: /usr/share/common-licenses/GPL-3 .

Many thanks to Ilja(s), Victor, Sascha, Dima!

For Olenka!

EOF
}

if [ ${#} -lt 6 ] ; then
    usage
    exit 1
fi

create_folders
copy_content
create_desktop_file
create_changelog
create_man_page
create_copyright_file
create_control_file
create_deb
