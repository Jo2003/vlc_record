DEFINES += _TASTE_POLSKY_TV \
           _HAS_VOD_MANAGER
RESOURCES += polsky_tv.qrc
HEADERS += tastes/defines_polsky_tv.h
WINICO = polsky_tv.ico
TRANSLATIONS = lang_de.ts \
               lang_pl.ts

# include common project (must be last) ...
include (common.pri)
