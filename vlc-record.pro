# -------------------------------------------------
# Project created by QtCreator 2009-12-27T18:33:08
# -------------------------------------------------
QT += network
CONFIG += debug_and_release \
    windows
TEMPLATE = app
INCLUDEPATH += .

# debug version will be built shared ...
CONFIG(debug, debug|release):CONFIG += shared
else:CONFIG += static

# -------------------------------------
# customization ...
# - make a define here and put needed
# values into customization.h
# -------------------------------------
# DEFINES += _CUST_RUSS_TELEK
# -------------------------------------
# Build with or without
# included player or without?
# -------------------------------------
DEFINES += INCLUDE_LIBVLC
SOURCES += main.cpp \
    recorder.cpp \
    cinifile.cpp \
    csettingsdlg.cpp \
    ckartinaclnt.cpp \
    ckartinaxmlparser.cpp \
    cwaittrigger.cpp \
    cepgbrowser.cpp \
    caboutdialog.cpp \
    clogfile.cpp \
    cchanlogo.cpp \
    ctimerrec.cpp \
    cvlcctrl.cpp \
    ctranslit.cpp
HEADERS += recorder.h \
    chanlistwidgetitem.h \
    cinifile.h \
    csettingsdlg.h \
    ckartinaclnt.h \
    ckartinaxmlparser.h \
    cwaittrigger.h \
    templates.h \
    cepgbrowser.h \
    caboutdialog.h \
    version_info.h \
    clogfile.h \
    chttptime.h \
    cchanlogo.h \
    defdef.h \
    ctimerrec.h \
    cvlcctrl.h \
    customization.h \
    ctranslit.h \
    cfavaction.h \
    playstates.h
FORMS += forms/csettingsdlg.ui \
    forms/caboutdialog.ui \
    forms/ctimerrec.ui
RESOURCES += vlc-record.qrc
RC_FILE = vlc-record.rc
TRANSLATIONS = lang_de.ts \
    lang_ru.ts

# for static build ...
static { 
    DEFINES += DSTATIC
    DEFINES += DINCLUDEPLUGS
    QTPLUGIN += qico \
        qgif \
        qjpeg
}
win32:TARGET = vlc-record
else { 
    static:TARGET = release/vlc-record
    shared:TARGET = debug/vlc-record
}

# -------------------------------------
# add includes if we want to build
# with included player!
# -------------------------------------
contains(DEFINES,INCLUDE_LIBVLC) { 
    INCLUDEPATH += include
    HEADERS += include/vlc/deprecated.h \
        include/vlc/libvlc.h \
        include/vlc/libvlc_events.h \
        include/vlc/libvlc_media.h \
        include/vlc/libvlc_media_discoverer.h \
        include/vlc/libvlc_media_library.h \
        include/vlc/libvlc_media_list.h \
        include/vlc/libvlc_media_list_player.h \
        include/vlc/libvlc_media_list_view.h \
        include/vlc/libvlc_media_player.h \
        include/vlc/libvlc_structures.h \
        include/vlc/libvlc_vlm.h \
        include/vlc/mediacontrol.h \
        include/vlc/mediacontrol_structures.h \
        include/vlc/vlc.h \
        cplayer.h
    FORMS += forms/cplayer.ui \
        forms/recorder_inc.ui
    SOURCES += cplayer.cpp
    LIBS += -Llib \
        -lvlc
}
else:FORMS += forms/recorder.ui

# translation stuff ...
include (language.pri)
