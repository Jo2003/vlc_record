RESOURCES += oem-record.qrc
WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts \
               lang_pl.ts
               
DEFINES += _TASTE_OEM \
           _IS_OEM \
           _HAS_VOD_MANAGER \
           __API_INCLUDED

HEADERS += ckartinaclnt.h \
           ckartinaxmlparser.h \
           capixmlparser.h \
           tastes/defines_oem_record.h
           
SOURCES += ckartinaclnt.cpp \
           ckartinaxmlparser.cpp \
           capixmlparser.cpp

include (common.pri)
