RESOURCES += vlc-record.qrc
HEADERS += tastes/defines_vlc_record.h
RC_FILE = vlc-record.rc
TRANSLATIONS = lang_de.ts \
               lang_ru.ts \
               lang_pl.ts
DEFINES += ENABLE_AD_SWITCH

# stuff for QJSON ...
INCLUDEPATH += qjson/include
DEFINES     += _USE_QJSON
include (qjson/qjson.pri)

# include common project (must be last) ...
include (common.pri)
