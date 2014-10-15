DEFINES += _TASTE_TELEPROM \
           __VLC_FOR_HLS
RESOURCES += vlc-record.qrc
HEADERS += tastes/defines_teleprom.h
WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts

DEFINES += _USE_QJSON

include (common.pri)
