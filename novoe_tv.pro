DEFINES += _TASTE_NOVOE_TV
RESOURCES += vlc-record.qrc
HEADERS += tastes/defines_novoe_tv.h
TRANSLATIONS = lang_de.ts \
               lang_ru.ts
WINICO = television.ico
DEFINES += _USE_QJSON __VLC_FOR_HLS
# DEFINES += _IS_OEM
include (common.pri)
