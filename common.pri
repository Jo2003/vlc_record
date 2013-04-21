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
    capiparser.cpp
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
    capiparser.h
FORMS += forms/csettingsdlg.ui \
    forms/caboutdialog.ui \
    forms/ctimerrec.ui \
    forms/qftsettings.ui \
    forms/qseccodedlg.ui \
    forms/qhelpdialog.ui \
    forms/qoverlayedcontrol.ui \
    forms/qrecordtimerwidget.ui
RESOURCES += common.qrc \
    lcd.qrc
INCLUDEPATH += tastes

contains(DEFINES, _TASTE_IPTV_RECORD) {
    message (Using alternative api client ...)
    HEADERS += crodnoeclient.h \
               crodnoeparser.h
    SOURCES += crodnoeclient.cpp \
               crodnoeparser.cpp
} else {
    message (Using standard api client ...)
    HEADERS += ckartinaclnt.h
    SOURCES += ckartinaclnt.cpp
}

contains(DEFINES, _USE_QJSON) {
    message (using QJson parser ...)
    INCLUDEPATH += qjson/include
    include (qjson/qjson.pri)
    DEFINES += QJSON_EXPORT=""
    SOURCES += cstdjsonparser.cpp
    HEADERS += cstdjsonparser.h
} else {
    message (using XML parser ...)
    SOURCES += ckartinaxmlparser.cpp \
               capixmlparser.cpp
    HEADERS += ckartinaxmlparser.h \
               capixmlparser.h
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

   win32:INCLUDEPATH += include

   HEADERS += cplayer.h \
        qvlcvideowidget.h \
        qclickandgoslider.h \
        qtimelabel.h
        # qbufferprogressbar.h
   FORMS += forms/cplayer.ui \
        forms/recorder_inc.ui
   SOURCES += qvlcvideowidget.cpp \
              cplayer.cpp
   LIBS += -lvlc
   win32:LIBS += -Llib

   mac {
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

#    unix:LIBS += -L/opt/vlc-1.1.1/lib \
#        -Wl,-rpath \
#        /opt/vlc-1.1.1/lib
}
else:FORMS += forms/recorder.ui

# translation stuff ...
include (language.pri)

