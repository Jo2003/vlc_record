/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 15.01.2017
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __20170115__DEFINES_TV_CLUB_H
   #define __20170115__DEFINES_TV_CLUB_H

#include <QtGlobal>

#define APP_NAME      "TVClub.us"
#define UPD_CHECK_URL "http://tvclub.us/pc_app.xml"
#define BIN_NAME      "tvclub"
#define API_SERVER    "api.iptv.so"
#define COMPANY_NAME  "TVClub.us"
#define COMPANY_LINK  "<a href='http://tvclub.us'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define API_XML_PATH   "/0.8/xml/"
#define API_JSON_PATH  "/0.8/json/"

// define classes of api client ...
#define ApiClient             CTVClubClient
#define ApiParser             CTVClubParser

#endif // __20170115__DEFINES_TV_CLUB_H
