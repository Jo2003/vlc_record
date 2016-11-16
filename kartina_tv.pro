DEFINES += _TASTE_KARTINA_TV \
           _HAS_VOD_MANAGER \
           _EXT_EPG
RESOURCES += kartina_tv.qrc
HEADERS += tastes/defines_kartina_tv.h \
    tastes/more_kartina_translations.h
WINICO = kartina_tv.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts

DEFINES += _USE_QJSON

include (common.pri)
