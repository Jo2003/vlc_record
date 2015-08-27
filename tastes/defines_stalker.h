/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/tastes/defines_novoe_tv.h $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 09:19:48
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: defines_novoe_tv.h 1265 2013-12-13 14:40:16Z Olenka.Joerg $
\*************************************************************/
#ifndef __20130626__DEFINES_NOVOE_TV_H
   #define __20130626__DEFINES_NOVOE_TV_H

#include <QtGlobal>

#define APP_NAME      "Stalker"
#define UPD_CHECK_URL "http://rt.coujo.de/stalker_ver.xml"
#define BIN_NAME      "stalker"
#define API_SERVER    "ott.new-rus.tv"
#define COMPANY_NAME  "Stalker"
#define COMPANY_LINK  "<a href='http://www.novoe.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define API_JSON_PATH  "/api/json2/"

// define classes of api client ...
#define ApiClient             QStalkerClient
#define ApiParser             QStalkerParser

#define MAX_ARCHIV_AGE       2418600     // < 4 weeks in seconds
#define MAX_NO_FAVOURITES    20          // max. number of favourites ...

#endif // __20130626__DEFINES_NOVOE_TV_H
