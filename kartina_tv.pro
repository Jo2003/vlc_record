DEFINES += _TASTE_KARTINA_TV
RESOURCES += kartina_tv.qrc
HEADERS += tastes/defines_kartina_tv.h
RC_FILE = kartina_tv.rc
TRANSLATIONS = lang_de.ts \
               lang_ru.ts

DEFINES += _USE_QJSON

include (common.pri)
