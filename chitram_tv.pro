DEFINES += _TASTE_CHITRAM_TV \
           _HAS_VOD_MANAGER \
           _HAS_OWN_STYLES \
           _HAS_VOD_LANG \
           _EXT_EPG \
           __VLC_FOR_HLS

RESOURCES += chitram_tv.qrc
HEADERS += tastes/defines_chitram_tv.h \
           tastes/more_chitram_translations.h
WINICO = chitram_tv.ico

DEFINES += _USE_QJSON

include (common.pri)
