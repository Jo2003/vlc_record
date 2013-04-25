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
#ifndef __310113__DEFINES_MOIDOM_TV_H
   #define __310113__DEFINES_MOIDOM_TV_H

#include <QtGlobal>

#ifdef INCLUDE_LIBVLC
   #define APP_NAME          "Moi-Dom.TV"
   #define UPD_CHECK_URL     "http://rt.coujo.de/moidom_tv_ver.xml"
#else
   #define APP_NAME          "Moi-Dom.TV-classic"
   #define UPD_CHECK_URL     "http://rt.coujo.de/moidom_tv_ver_classic.xml"
#endif // INCLUDE_LIBVLC

#define BIN_NAME             "moidom_tv"
#define API_SERVER           "iptv.moi-dom.tv"

#define COMPANY_NAME "Moi-Dom.TV"
#define COMPANY_LINK "<a href='http://www.moi-dom.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

// use xml parser ...
#define ApiParser             CKartinaXMLParser

#endif // __310113__DEFINES_MOIDOM_TV_H

