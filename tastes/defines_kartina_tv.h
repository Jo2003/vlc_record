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
#ifndef __012412__DEFINES_KARTINA_TV_H
   #define __012412__DEFINES_KARTINA_TV_H

#include <QtGlobal>

#ifdef INCLUDE_LIBVLC
   #define APP_NAME          "Kartina.TV"
   #define UPD_CHECK_URL     "http://rt.coujo.de/kartina_tv_ver.xml"
#else
   #define APP_NAME          "Kartina.TV-classic"
   #define UPD_CHECK_URL     "http://rt.coujo.de/kartina_tv_ver_classic.xml"
#endif // INCLUDE_LIBVLC

#define BIN_NAME             "kartina_tv"
#define KARTINA_HOST         "iptv.kartina.tv"

#define COMPANY_NAME "Kartina.TV"
#define COMPANY_LINK "<a href='http://www.kartina.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#endif // __012412__DEFINES_KARTINA_TV_H

