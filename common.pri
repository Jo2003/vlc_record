# -------------------------------------------------
# Project created by QtCreator 2009-12-27T18:33:08
# -------------------------------------------------
QT += network \
    sql \
    xml

CONFIG += help

# build debug and release ...
CONFIG += debug_and_release \
    windows
TEMPLATE = app
INCLUDEPATH += .

# build shared or static ... ?
# CONFIG += static
CONFIG += shared

# -------------------------------------
# program version
# -------------------------------------
PROGMAJ=2
PROGMIN=67

# -------------------------------------
# customization ...
# - make a define here and put needed
# values into customization.h
# -------------------------------------
# DEFINES += _CUST_RUSS_TELEK
# DEFINES += _CUST_RUSS_SERVICES
# -------------------------------------
# Build with or without
# included player or without?
# -------------------------------------
DEFINES += INCLUDE_LIBVLC
# DEFINES += __TRACE
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
    qupdatenotifydlg.cpp \
    qwatchlistdlg.cpp \
    ctimeshift.cpp \
    chtmlwriter.cpp \
    qoverlayicon.cpp \
    qchannelmap.cpp \
    qextm3uparser.cpp
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
    qupdatenotifydlg.h \
    qstringfilterwidgetaction.h \
    qwatchlistdlg.h \
    ctimeshift.h \
    chtmlwriter.h \
    qoverlayicon.h \
    qchannelmap.h \
    qfadewidget.h \
    qextm3uparser.h
FORMS += forms/csettingsdlg.ui \
    forms/caboutdialog.ui \
    forms/ctimerrec.ui \
    forms/qftsettings.ui \
    forms/qseccodedlg.ui \
    forms/qhelpdialog.ui \
    forms/qoverlayedcontrol.ui \
    forms/qrecordtimerwidget.ui \
    forms/qupdatenotifydlg.ui \
    forms/qwatchlistdlg.ui
RESOURCES += common.qrc \
    lcd.qrc
INCLUDEPATH += tastes


# -------------------------------------------------
# create Windows rc file ...
# -------------------------------------------------
win32 {
    # Define how to create program.rc
    VERINC="$${LITERAL_HASH}define VER_FILEVERSION 0,$${PROGMAJ},$${PROGMIN},0"
    VERSTR="$${LITERAL_HASH}define VER_FILEVERSION_STR \"0,$${PROGMAJ},$${PROGMIN},0\""
    PRODVER="$${LITERAL_HASH}define PROD_VER_STR \"$${PROGMAJ}.$${PROGMIN}\""
    FILEVER="$${LITERAL_HASH}define FILE_VER_STR \"$${PROGMAJ}.$${PROGMIN}.0\""
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


# for static build ...
static {
    DEFINES += DSTATIC
    DEFINES += DINCLUDEPLUGS
    QTPLUGIN += qsqlite
}

# where the target should be stored ...
win32 {
# nothing
}
else  {
    CONFIG(debug, debug|release):DESTDIR = debug
    else:DESTDIR = release
}

unix {
   OTHER_FILES += create_install_mak.sh \
      documentation/create_qthelp.sh
   QMAKE_POST_LINK = ./create_install_mak.sh $$basename(TARGET)
}

# -------------------------------------
# add includes if we want to build
# with included player!
# -------------------------------------
contains(DEFINES,INCLUDE_LIBVLC) {

   win32 {
      INCLUDEPATH += include
      LIBS += -Llib
   }
   else:mac {
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

      LIBS += -L./mac/lib
      QMAKE_POST_LINK = ./create_mac_bundle.sh $$basename(TARGET)
   }
   else {
      unix:LIBS += -lX11
   }

   HEADERS += cplayer.h \
        qvlcvideowidget.h \
        qclickandgoslider.h \
        qtimelabel.h

   FORMS += forms/cplayer.ui \
        forms/recorder_inc.ui

   SOURCES += qvlcvideowidget.cpp \
              cplayer.cpp
   LIBS += -lvlc

#    unix:LIBS += -L/opt/vlc-1.1.1/lib \
#        -Wl,-rpath \
#        /opt/vlc-1.1.1/lib
}
else:FORMS += forms/recorder.ui

# translation stuff ...
include (language.pri)

