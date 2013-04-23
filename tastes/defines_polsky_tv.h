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
#ifndef __012412__DEFINES_POLSKY_TV_H
   #define __012412__DEFINES_POLSKY_TV_H

#include <QtGlobal>

#ifdef INCLUDE_LIBVLC
   #define APP_NAME          "Polsky.TV"
   #define UPD_CHECK_URL     "http://rt.coujo.de/polsky_tv_ver.xml"
#else
   #define APP_NAME          "Polsky.TV-classic"
   #define UPD_CHECK_URL     "http://rt.coujo.de/polsky_tv_ver_classic.xml"
#endif // INCLUDE_LIBVLC

#define BIN_NAME             "polsky_tv"
#define API_SERVER           "iptv.polsky.tv"

#define COMPANY_NAME "Polsky.TV"
#define COMPANY_LINK "<a href='http://www.polsky.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define APPLICATION_SHORTCUT "pol"

#endif // __012412__DEFINES_POLSKY_TV_H
