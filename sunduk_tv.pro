DEFINES += _TASTE_SUNDUK_TV \
           _HAS_VOD_MANAGER \
           _IS_OEM
RESOURCES += kartina_tv.qrc

WINICO = kartina_tv.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts

DEFINES += _USE_QJSON

include (common.pri)
HEADERS += tastes/defines_sunduk_tv.h \
    csundukclnt.h
SOURCES += \
    csundukclnt.cpp
