DEFINES += _TASTE_STALKER \
           __API_INCLUDED \
           __VLC_FOR_HLS

RESOURCES += vlc-record.qrc

include (qtjson/qtjson.pri)
HEADERS += tastes/defines_stalker.h \
           qstalkerclient.h \
           cstdjsonparser.h \
           qstalkerparser.h \
           qstalker_rec.hpp
SOURCES += qstalkerclient.cpp \
           qstalkerparser.cpp \
           cstdjsonparser.cpp

TRANSLATIONS = lang_de.ts \
               lang_ru.ts

WINICO = television.ico
DEFINES += _IS_OEM
include (common.pri)
