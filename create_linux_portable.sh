#!/bin/bash

if [ -z ${1} ] ; then
	echo Usage: ${0} [binary name]!
	exit 1
fi

BINARY=${1}
STARTDIR=$(pwd)
BINARYDIR=${STARTDIR}/../${BINARY}-portable

# delete maybe existing portable folder
rm -rf ${BINARYDIR}

# create portable folder with subfolders ...
mkdir -p ${BINARYDIR}/bin
mkdir -p ${BINARYDIR}/lib/vlc/plugins
mkdir -p ${BINARYDIR}/share/modules
mkdir -p ${BINARYDIR}/share/language
mkdir -p ${BINARYDIR}/share/resources

# copy binary, modules, language
cp ${STARTDIR}/release/${BINARY} ${BINARYDIR}/bin/
cp ${STARTDIR}/modules/*.mod ${BINARYDIR}/share/modules/
cp ${STARTDIR}/*.qm ${BINARYDIR}/share/language/
cp ${STARTDIR}/resources/${BINARY}.png ${BINARYDIR}/share

# copy plugins folder
cp -r /usr/lib/vlc/plugins ${BINARYDIR}/lib/vlc/
cp /usr/lib/vlc/vlc-cache-gen ${BINARYDIR}/lib/vlc/
rm -f ${BINARYDIR}/lib/vlc/plugins/*.dat

# remove unwanted plugins 
rm -rf ${BINARYDIR}/lib/vlc/plugins/lua
rm -rf ${BINARYDIR}/lib/vlc/plugins/gui

# create portable marker
touch ${BINARYDIR}/bin/.portable

# copy all needed libraries ...
NEEDEDLIBS=$(ldd ${BINARYDIR}/bin/${BINARY} | sed -n 's/^[^/]*\([^ ]*\).*$/\1/p')

for i in ${NEEDEDLIBS} ; do
	echo "Copy $i -> ${BINARYDIR}/lib/ ..."
	cp "$i" ${BINARYDIR}/lib/
done

# create new plugin cache ...
cd ${BINARYDIR}/lib/vlc/
./vlc-cache-gen -f ./plugins

# create desktop file
cd ${BINARYDIR}
cat << EOF >${BINARY}.sh
#!/bin/bash
bin/${BINARY} &

EOF
chmod +x ${BINARY}.sh