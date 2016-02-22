/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/tastes/defines_iptv_record.h $
|
| Author: Jo2003
|
| Begin: 19.03.2013
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: defines_iptv_record.h 1265 2013-12-13 14:40:16Z Olenka.Joerg $
\*************************************************************/
#ifndef __20130319__DEFINES_OTRADA_TV_H
   #define __20130319__DEFINES_OTRADA_TV_H

#include <QtGlobal>

#define APP_NAME      "Otrada.TV"
#define UPD_CHECK_URL "http://rt.coujo.de/otrada_tv_ver.xml"
#define BIN_NAME      "otrada_tv"
#define API_SERVER    "core.otradatv.com"
#define COMPANY_NAME  "Otrada.TV"
#define COMPANY_LINK  "<a href='http://otradatv.com'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define API_XML_PATH   "/iptv/api/v1/xml/"
#define API_JSON_PATH  "/iptv/api/v1/json/"

// define classes of api client ...
#define ApiClient             COtradaClient
#define ApiParser             COtradaParser

// define an offset to make radio cid unique!
#define RADIO_OFFSET          0x0ff00000

#endif // __20130319__DEFINES_OTRADA_TV_H
