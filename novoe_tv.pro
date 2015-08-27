DEFINES += _TASTE_NOVOE_TV \
           __API_INCLUDED
RESOURCES += vlc-record.qrc
TRANSLATIONS = lang_de.ts \
               lang_ru.ts
WINICO = television.ico

include (qtjson/qtjson.pri)
HEADERS += cnovoeclient.h \
           cnovoeparser.h \
           cstdjsonparser.h \
           tastes/defines_novoe_tv.h
SOURCES += cnovoeclient.cpp \
           cnovoeparser.cpp \
           cstdjsonparser.cpp
              
# DEFINES += _IS_OEM
include (common.pri)
