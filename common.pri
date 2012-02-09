# -------------------------------------------------
# Project created by QtCreator 2009-12-27T18:33:08
# -------------------------------------------------
QT += network \
    sql \
    xml

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
SOURCES += main.cpp \
    recorder.cpp \
    csettingsdlg.cpp \
    ckartinaclnt.cpp \
    ckartinaxmlparser.cpp \
    cwaittrigger.cpp \
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
    qftsettings.cpp
HEADERS += recorder.h \
    csettingsdlg.h \
    ckartinaclnt.h \
    ckartinaxmlparser.h \
    cwaittrigger.h \
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
    ctimerex.h \
    cshowinfo.h \
    cvlcrecdb.h \
    tables.h \
    cstreamloader.h \
    cvodbrowser.h \
    cpixloader.h \
    cshortcutgrabber.h \
    qchanlistdelegate.h \
    qshortcuttable.h \
    qftsettings.h
FORMS += forms/csettingsdlg.ui \
    forms/caboutdialog.ui \
    forms/ctimerrec.ui \
    forms/qftsettings.ui
RESOURCES += common.qrc \
    lcd.qrc
INCLUDEPATH += tastes

# for static build ...
static {
    DEFINES += DSTATIC
    DEFINES += DINCLUDEPLUGS
    QTPLUGIN += qsqlite
    # QTPLUGIN += qico qgif qjpeg

}

# where the target should be stored ...
win32 {
# nothing
}
else  {
    CONFIG(debug, debug|release):DESTDIR = debug
    else:DESTDIR = release
}

# -------------------------------------
# add includes if we want to build
# with included player!
# -------------------------------------
contains(DEFINES,INCLUDE_LIBVLC) {

   win32:INCLUDEPATH += include

   HEADERS += cplayer.h \
        qvlcvideowidget.h
   FORMS += forms/cplayer.ui \
        forms/recorder_inc.ui
   SOURCES += qvlcvideowidget.cpp \
              cplayer.cpp
   LIBS += -lvlc
   win32:LIBS += -Llib

   mac {
      OTHER_FILES += create_mac_bundle.sh
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

#    unix:LIBS += -L/opt/vlc-1.1.1/lib \
#        -Wl,-rpath \
#        /opt/vlc-1.1.1/lib
}
else:FORMS += forms/recorder.ui

# translation stuff ...
include (language.pri)
