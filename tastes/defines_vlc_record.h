/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/kartina.tv/tastes/defines_kartina_tv.h $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 09:19:48
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: defines_kartina_tv.h 620 2012-01-25 07:41:35Z Olenka.Joerg $
\*************************************************************/
#ifndef __012412__DEFINES_VLC_RECORD_H
   #define __012412__DEFINES_VLC_RECORD_H

#include <QtGlobal>

#ifdef INCLUDE_LIBVLC
   #define APP_NAME          "VLC-Record"
   #define UPD_CHECK_URL     "http://rt.coujo.de/vlc_record_ver.xml"
#else
   #define APP_NAME          "Kartina.TV-classic"
   #define UPD_CHECK_URL     "http://rt.coujo.de/vlc_record_ver_classic.xml"
#endif // INCLUDE_LIBVLC

#define BIN_NAME             "vlc-record"
#define KARTINA_HOST         "iptv.kartina.tv"

#define COMPANY_NAME "Kartina.TV"
#define COMPANY_LINK "<a href='http://www.kartina.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#endif // __012412__DEFINES_VLC_RECORD_H

