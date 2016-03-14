# -------------------------------------------------
# Project created by QtCreator 2009-12-27T18:33:08
# -------------------------------------------------
QT += core \
    network \
    sql \
    xml

greaterThan(QT_MAJOR_VERSION, 4) {

    QT += widgets \
          help
    DEFINES += __QT_5

    INCLUDEPATH += qhttp
}
else {

    CONFIG += help
    DEFINES += __QT_4
}


# build debug and release ...
CONFIG += debug_and_release \
    windows
TEMPLATE = app
INCLUDEPATH += .

# build shared !
CONFIG += shared

# force traces on debug ...
CONFIG(debug,debug|release): DEFINES += __TRACE

# -------------------------------------
# support C++11
# Depending on your compiler you
# have to use different options
# to enable C++11
# MinGW for used for Qt4 needs
# -std=c++0x!
# For Qt5 you should use:
# CONFIG += c++11
# -------------------------------------
#win32: QMAKE_CXXFLAGS += -std=c++11
#else:mac: QMAKE_CXXFLAGS += -std=c++11
#else:unix: QMAKE_CXXFLAGS += -std=c++11

# -------------------------------------
# program version
# -------------------------------------
PROGMAJ=2
PROGMIN=72
PROGBUILD=1

# here you can enable traces ...
# DEFINES += __TRACE

# this flag will be checked on linux only!
# DEFINES += __PORTABLE

SOURCES += main.cpp \
    recorder.cpp \
    csettingsdlg.cpp \
    cepgbrowser.cpp \
    caboutdialog.cpp \
    clogfile.cpp \
    ctimerrec.cpp \
    cvlcctrl.cpp \
    ctranslit.cpp \
    cdirstuff.cpp \
    clcddisplay.cpp \
    cshowinfo.cpp \
    cvlcrecdb.cpp \
    cstreamloader.cpp \
    cvodbrowser.cpp \
    cpixloader.cpp \
    cshortcutgrabber.cpp \
    qchanlistdelegate.cpp \
    qshortcuttable.cpp \
    qftsettings.cpp \
    qseccodedlg.cpp \
    qfavframe.cpp \
    qhelpdialog.cpp \
    qoverlayedcontrol.cpp \
    qfusioncontrol.cpp \
    qrecordtimerwidget.cpp \
    qiptvctrlclient.cpp \
    qcustparser.cpp \
    capiparser.cpp \
    qnotifydlg.cpp \
    qupdatenotifydlg.cpp \
    qwatchlistdlg.cpp \
    chtmlwriter.cpp \
    qoverlayicon.cpp \
    qchannelmap.cpp \
    qvlcvideowidget.cpp \
    cplayer.cpp \
    qexpirenotifydlg.cpp \
    qauthdlg.cpp \
    qwaitwidget.cpp \
    qclickandgoslider.cpp \
    qdatetimesyncro.cpp \
    qstatemessage.cpp \
    qwatchstats.cpp \
    qstrstandarddlg.cpp \
    qspeedtestdlg.cpp
HEADERS += recorder.h \
    csettingsdlg.h \
    templates.h \
    cepgbrowser.h \
    caboutdialog.h \
    version_info.h \
    clogfile.h \
    defdef.h \
    ctimerrec.h \
    cvlcctrl.h \
    ctranslit.h \
    cfavaction.h \
    cdirstuff.h \
    cshortcutex.h \
    clcddisplay.h \
    playstates.h \
    qtimerex.h \
    cshowinfo.h \
    cvlcrecdb.h \
    tables.h \
    cstreamloader.h \
    cvodbrowser.h \
    cpixloader.h \
    cshortcutgrabber.h \
    qchanlistdelegate.h \
    qshortcuttable.h \
    qftsettings.h \
    small_helpers.h \
    qseccodedlg.h \
    qfavframe.h \
    qhelpdialog.h \
    qoverlayedcontrol.h \
    qfusioncontrol.h \
    qmovehandle.h \
    qcomboboxex.h \
    qrecordtimerwidget.h \
    qiptvctrlclient.h \
    ciptvdefs.h \
    cparser.h \
    qcustparser.h \
    api_inc.h \
    capiparser.h \
    qnotifydlg.h \
    qupdatenotifydlg.h \
    qstringfilterwidgetaction.h \
    qwatchlistdlg.h \
    chtmlwriter.h \
    qoverlayicon.h \
    qchannelmap.h \
    qfadewidget.h \
    cplayer.h \
    qvlcvideowidget.h \
    qclickandgoslider.h \
    qtimelabel.h \
    qexpirenotifydlg.h \
    qauthdlg.h \
    qwaitwidget.h \
    qdatetimesyncro.h \
    qstatemessage.h \
    externals_inc.h \
    qwatchstats.h \
    qurlex.h \
    qtextbrowserex.h \
    qstrstandarddlg.h \
    qspeedtestdlg.h
FORMS += forms/csettingsdlg.ui \
    forms/caboutdialog.ui \
    forms/ctimerrec.ui \
    forms/qftsettings.ui \
    forms/qseccodedlg.ui \
    forms/qhelpdialog.ui \
    forms/qoverlayedcontrol.ui \
    forms/qrecordtimerwidget.ui \
    forms/qnotifydlg.ui \
    forms/qwatchlistdlg.ui \
    forms/cplayer.ui \
    forms/recorder_inc.ui \
    forms/qauthdlg.ui \
    forms/qwaitwidget.ui \
    forms/qstatemessage.ui \
    forms/qstrstandarddlg.ui \
    forms/qspeedtestdlg.ui
RESOURCES += common.qrc \
    lcd.qrc
INCLUDEPATH += tastes
LIBS += -lvlc


# -------------------------------------------------
# create Windows rc file ...
# -------------------------------------------------
win32 {
    # Define how to create program.rc
    VERINC="$${LITERAL_HASH}define VER_FILEVERSION 0,$${PROGMAJ},$${PROGMIN},$${PROGBUILD}"
    VERSTR="$${LITERAL_HASH}define VER_FILEVERSION_STR \"0,$${PROGMAJ},$${PROGMIN},$${PROGBUILD}\""
    PRODVER="$${LITERAL_HASH}define PROD_VER_STR \"$${PROGMAJ}.$${PROGMIN}\""
    FILEVER="$${LITERAL_HASH}define FILE_VER_STR \"$${PROGMAJ}.$${PROGMIN}.$${PROGBUILD}\""
    WININC="$${LITERAL_HASH}include ^<windows.h^>"


    rc.target = program.rc
    rc.commands = echo $${WININC} > program.rc \
      && echo IDI_ICON1 ICON DISCARDABLE \"resources/$${WINICO}\" >> program.rc \
      && echo $${VERINC} >> program.rc \
      && echo $${VERSTR} >> program.rc \
      && echo $${PRODVER} >> program.rc \
      && echo $${FILEVER} >> program.rc \
      && type rc.tmpl >> program.rc
    rc.depends =

    QMAKE_EXTRA_TARGETS += rc

    PRE_TARGETDEPS += program.rc

    QMAKE_CLEAN += /q program.rc

    RC_FILE = program.rc

    INCLUDEPATH += include
    LIBS += -Llib
}
else:mac {
   HEADERS += $$PWD/macNoSleep.h
   OBJECTIVE_SOURCES += $$PWD/macNoSleep.mm

   OTHER_FILES += create_mac_bundle.sh \
                  release/create_dmg.sh
   INCLUDEPATH += mac/include

   CONFIG(debug,debug|release):appclean.commands = cd debug && rm -rf *.app && rm -f *.dmg
   CONFIG(release,debug|release):appclean.commands = cd release && rm -rf *.app && rm -f *.dmg
   QMAKE_EXTRA_TARGETS += appclean

   # Hook our appclean target in between qmake's Makefile update and the actual project target.
   appcleanhook.depends = appclean
   CONFIG(debug,debug|release):appcleanhook.target = Makefile.Debug
   CONFIG(release,debug|release):appcleanhook.target = Makefile.Release
   QMAKE_EXTRA_TARGETS += appcleanhook

   LIBS += -L./mac/lib -framework IOKit -framework Foundation
   QMAKE_POST_LINK = ./create_mac_bundle.sh $$basename(TARGET)
}
else:unix {

   contains(QMAKE_HOST.arch, x86_64) {
       DEFINES += __ARCH__X86_64
   } else {
       DEFINES += __ARCH__I386
   }

   LIBS += -lX11
   OTHER_FILES += create_install_mak.sh \
                  create_deb.sh

   QMAKE_POST_LINK = ./create_install_mak.sh $$basename(TARGET) $$QT_MAJOR_VERSION

   contains(DEFINES,__PORTABLE) {
        LIBS += -Wl,-rpath ../lib
   }
#   LIBS += -L/home/joergn/libvlc \
#        -Wl,-rpath lib
}

#############
contains(DEFINES, _TASTE_IPTV_RECORD) {
    message (Using alternative api client ...)
    HEADERS += crodnoeclient.h \
               crodnoeparser.h
    SOURCES += crodnoeclient.cpp \
               crodnoeparser.cpp
} else:contains(DEFINES, _TASTE_NOVOE_TV) {
   message (Using novoe api client ...)
   HEADERS += cnovoeclient.h \
              cnovoeparser.h
   SOURCES += cnovoeclient.cpp \
              cnovoeparser.cpp
} else:contains(DEFINES, _TASTE_TELEPROM) {
   message (Using teleprom api client ...)
   HEADERS += ctelepromclient.h
   SOURCES += ctelepromclient.cpp
} else {
   message (Using standard api client ...)
   HEADERS += ckartinaclnt.h
   SOURCES += ckartinaclnt.cpp
}

#############

contains(DEFINES, _USE_QJSON) {
   message (using QtJson parser ...)
   include (qtjson/qtjson.pri)
   SOURCES += cstdjsonparser.cpp
   HEADERS += cstdjsonparser.h
} else {
   message (using XML parser ...)
   !contains(DEFINES, _TASTE_IPTV_RECORD) {
      SOURCES += ckartinaxmlparser.cpp
      HEADERS += ckartinaxmlparser.h
   }
   SOURCES += capixmlparser.cpp
   HEADERS += capixmlparser.h
}

# where the target should be stored ...
!win32 {
    CONFIG(debug, debug|release):DESTDIR = debug
    else:DESTDIR = release
}

# translation stuff ...
include (language.pri)

contains(DEFINES, __QT_5) {
    include (qhttp/qhttp.pri)
}
