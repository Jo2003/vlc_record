DEFINES += _TASTE_NOVOE_TV
RESOURCES += vlc-record.qrc
HEADERS += tastes/defines_novoe_tv.h
RC_FILE = vlc-record.rc
TRANSLATIONS = lang_de.ts \
               lang_ru.ts

DEFINES += _USE_QJSON

include (common.pri)
