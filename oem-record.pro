RESOURCES += oem-record.qrc
HEADERS += tastes/defines_oem_record.h
WINICO = television.ico
TRANSLATIONS = lang_de.ts \
               lang_ru.ts \
               lang_pl.ts
DEFINES += _TASTE_OEM \
           _IS_OEM \
           _HAS_VOD_MANAGER
include (common.pri)
