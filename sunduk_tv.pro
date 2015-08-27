DEFINES += _TASTE_SUNDUK_TV \
           _HAS_VOD_MANAGER \
           _IS_OEM \
           __API_INCLUDED
RESOURCES += kartina_tv.qrc

WINICO = kartina_tv.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts
include (qtjson/qtjson.pri)
SOURCES += cstdjsonparser.cpp \
           csundukclnt.cpp \
           ckartinaclnt.cpp
           
HEADERS += cstdjsonparser.h \
           tastes/defines_sunduk_tv.h \
           csundukclnt.h \
           ckartinaclnt.h

include (common.pri)
