RESOURCES += vlc-record.qrc
HEADERS += tastes/defines_vlc_record.h
WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts \
               lang_pl.ts
DEFINES += ENABLE_AD_SWITCH \
           _HAS_VOD_MANAGER

# stuff for QJSON ...
DEFINES     += _USE_QJSON

# include common project (must be last) ...
include (common.pri)
