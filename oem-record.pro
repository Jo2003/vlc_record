RESOURCES += oem-record.qrc
HEADERS += tastes/defines_oem_record.h
RC_FILE = vlc-record.rc
TRANSLATIONS = lang_de.ts \
               lang_ru.ts \
               lang_pl.ts
DEFINES += _TASTE_OEM \
           _IS_OEM
include (common.pri)
