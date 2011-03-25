#  have we defined lrelease variable ... ?
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

#  have we defined lupdate variable ... ?
isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

# create ts files, make sure to NOT delete them on clean!
PRO_FILE = $$basename(TARGET).pro
createts.input = PRO_FILE
createts.output = create_ts_files
createts.commands = @$$QMAKE_LUPDATE -silent ${QMAKE_FILE_IN}
createts.CONFIG += no_link no_clean
QMAKE_EXTRA_COMPILERS += createts
PRE_TARGETDEPS += compiler_createts_make_all

# create qm files from ts files.
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = @$$QMAKE_LRELEASE -compress -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all

