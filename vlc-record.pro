RESOURCES += vlc-record.qrc
HEADERS += tastes/defines_vlc_record.h \
           tastes/more_kartina_translations.h
WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts \
               lang_pl.ts
DEFINES += ENABLE_AD_SWITCH \
           _HAS_VOD_MANAGER \
           _EXT_EPG \
           __MY_EXT_EPG \
           __VLC_FOR_HLS \
           _TASTE_VLC_RECORD

# stuff for QJSON ...
DEFINES     += _USE_QJSON

# include common project (must be last) ...
include (common.pri)
