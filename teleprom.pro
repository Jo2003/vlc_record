DEFINES += _TASTE_TELEPROM \
           __VLC_FOR_HLS \
           __API_INCLUDED
RESOURCES += vlc-record.qrc

WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts

include (qtjson/qtjson.pri)
HEADERS += ctelepromclient.h \
           tastes/defines_teleprom.h \
           cstdjsonparser.h
SOURCES += ctelepromclient.cpp \
           cstdjsonparser.cpp

include (common.pri)
