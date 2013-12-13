/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.03.2013
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __20130319__DEFINES_IPTV_RECORD_H
   #define __20130319__DEFINES_IPTV_RECORD_H

#include <QtGlobal>

#define APP_NAME      "Iptv-Record"
#define UPD_CHECK_URL "http://rt.coujo.de/rodnoe_tv_ver.xml"
#define BIN_NAME      "iptv-record"
#define API_SERVER    "file-teleport.com"
#define COMPANY_NAME  "Iptv-Provider"
#define COMPANY_LINK  "<a href='http://vlc-record.coujo.de'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define API_XML_PATH   "/iptv/api/v1/xml/"
#define API_JSON_PATH  "/iptv/api/v1/json/"

// define classes of api client ...
#define ApiClient             CRodnoeClient
#define ApiParser             CRodnoeParser

// define an offset to make radio cid unique!
#define RADIO_OFFSET          0x0ff00000

#endif // __20130319__DEFINES_IPTV_RECORD_H
