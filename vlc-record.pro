# -------------------------------------------------
# Project created by QtCreator 2009-12-27T18:33:08
# -------------------------------------------------
TARGET = vlc-record
QT += network
# CONFIG += static
TEMPLATE = app
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
    ctimerrec.cpp
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
    ctimerrec.h
FORMS += recorder.ui \
    csettingsdlg.ui \
    caboutdialog.ui \
    ctimerrec.ui
RESOURCES += vlc-record.qrc
OTHER_FILES += resources/tools.ico \
    vlc-record.rc \
    resources/television.ico \
    resources/record.ico \
    example.txt \
    resources/play.ico \
    resources/logo.png \
    resources/info.ico \
    resources/quit.ico
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
