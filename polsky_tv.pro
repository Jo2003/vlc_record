DEFINES += _TASTE_POLSKY_TV \
           _HAS_VOD_MANAGER \
           _EXT_EPG \
           __VLC_FOR_HLS
RESOURCES += polsky_tv.qrc
HEADERS += tastes/defines_polsky_tv.h \
           tastes/more_polsky_translations.h
WINICO = polsky_tv.ico
TRANSLATIONS = lang_de.ts \
               lang_pl.ts

# stuff for QJSON ...
DEFINES     += _USE_QJSON

# include common project (must be last) ...
include (common.pri)
