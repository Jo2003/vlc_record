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

#ifdef INCLUDE_LIBVLC
   #define APP_NAME          "Iptv-Record"
   #define UPD_CHECK_URL     "http://rt.coujo.de/rodnoe_tv_ver.xml"
#else
   #define APP_NAME          "Iptv-Record-classic"
   #define UPD_CHECK_URL     "http://rt.coujo.de/rodnoe_tv_ver_classic.xml"
#endif // INCLUDE_LIBVLC

#define BIN_NAME             "iptv-record"
#define KARTINA_HOST         "file-teleport.com"

#define COMPANY_NAME "Iptv-Provider"
#define COMPANY_LINK "<a href='http://vlc-record.coujo.de'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define KARTINA_API_XML_PATH   "/iptv/api/v1/xml/"
#define KARTINA_API_JSON_PATH  "/iptv/api/v1/json/"

#endif // __20130319__DEFINES_IPTV_RECORD_H
