DEFINES += _TASTE_IPTV_RECORD \
           _IS_OEM \
           __API_INCLUDED
           
RESOURCES += iptv-record.qrc
WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts \
               lang_pl.ts

HEADERS += crodnoeclient.h \
           crodnoeparser.h \
           capixmlparser.h \
           tastes/defines_iptv_record.h
           
SOURCES += crodnoeclient.cpp \
           crodnoeparser.cpp \
           capixmlparser.cpp

include (common.pri)
