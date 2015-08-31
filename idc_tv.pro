RESOURCES += vlc-record.qrc
HEADERS += tastes/defines_idc_tv.h
WINICO = television.ico
TRANSLATIONS = lang_ru.ts
DEFINES += _TASTE_IDC_TB \
           _HAS_VOD_MANAGER \
           _EXT_EPG \
           __VLC_FOR_HLS

# stuff for QJSON ...
DEFINES     += _USE_QJSON

# include common project (must be last) ...
include (common.pri)
