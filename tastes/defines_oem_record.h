/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 09:19:48
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __012412__DEFINES_OEM_RECORD_H
   #define __012412__DEFINES_OEM_RECORD_H

#include <QtGlobal>

#ifdef INCLUDE_LIBVLC
   #define APP_NAME          "OEM-Record"
   #define UPD_CHECK_URL     "http://rt.coujo.de/oem_record_ver.xml"
#else
   #define APP_NAME          "OEM-Record-classic"
   #define UPD_CHECK_URL     "http://rt.coujo.de/oem_record_ver_classic.xml"
#endif // INCLUDE_LIBVLC

#define BIN_NAME             "oem-record"
#define API_SERVER           "iptv.kartina.tv"

#define COMPANY_NAME "Kartina.TV"
#define COMPANY_LINK "<a href='http://www.kartina.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#endif // __012412__DEFINES_OEM_RECORD_H

