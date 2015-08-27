DEFINES += _TASTE_LTVISION \
           __API_INCLUDED
RESOURCES += vlc-record.qrc
WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts

HEADERS += ckartinaclnt.h \
           ckartinaxmlparser.h \
           capixmlparser.h \
           tastes/defines_ltvision.h
SOURCES += ckartinaclnt.cpp \
           ckartinaxmlparser.cpp \
           capixmlparser.cpp
   
# DEFINES += _IS_OEM

include (common.pri)
