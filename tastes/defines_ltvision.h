/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 31.01.2013
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __20130716__DEFINES_LTVISION_H
   #define __20130716__DEFINES_LTVISION_H

#include <QtGlobal>

#ifdef INCLUDE_LIBVLC
   #define APP_NAME          "LighTVision"
   #define UPD_CHECK_URL     "http://rt.coujo.de/ltv_ver.xml"
#else
   #define APP_NAME          "LighTVision-classic"
   #define UPD_CHECK_URL     "http://rt.coujo.de/ltv_ver_classic.xml"
#endif // INCLUDE_LIBVLC

#define BIN_NAME             "ltvision"
#define API_SERVER           "irij.lvtv.eu"

#define COMPANY_NAME "LighTVision"
#define COMPANY_LINK "<a href='http://www.lvtv.eu'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

// use xml parser ...
#define ApiParser             CKartinaXMLParser

#endif // __20130716__DEFINES_LTVISION_H

