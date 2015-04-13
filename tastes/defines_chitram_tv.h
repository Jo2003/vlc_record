/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/tastes/defines_chitram_tv.h $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 09:19:48
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: defines_chitram_tv.h 1290 2014-01-28 12:16:02Z Olenka.Joerg $
\*************************************************************/
#ifndef __20140128__DEFINES_CHITRAM_TV_H
   #define __20140128__DEFINES_CHITRAM_TV_H

#include <QtGlobal>

#define APP_NAME      "Chitram.TV"
#define UPD_CHECK_URL "http://rt.coujo.de/chitram_tv_ver.xml"
#define BIN_NAME      "chitram_tv"
#define API_SERVER    "iptv.chitram.tv"
#define COMPANY_NAME  "Chitram.TV"
#define COMPANY_LINK  "<a href='http://www.chitram.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#ifdef _HAS_OWN_STYLES
   // css styles for html writer ...
   #define TMPL_TAB_STYLE  "color: black; background-color: #AC905E; width: 100%;"
   #define TMPL_A_STYLE    "color: black; background-color: #FFF4DE; padding: 3px;"
   #define TMPL_B_STYLE    "color: black; background-color: #FFEDCB; padding: 3px;"
   #define TMPL_INNER_TAB  "color: black; background-color: #FFEDCB; padding: 5px;"
   #define TMPL_TH_STYLE   "color: white; background-color: #AC905E; padding: 3px; font: bold; text-align: center; vertical-align: middle;"
   #define TMPL_CUR_STYLE  "color: black; background-color: #FFC85B; padding: 3px;"
   #define TMPL_VOD_STYLE  "color: #800; background-color: #FFEDCB; font-weight: bold; padding: 10px;"
   #define TMPL_IMG_RFLOAT "float: right; margin: 15px; padding: 2px; border: 1px solid #666;"
   #define TMPL_ONE_CELL   "background-color: #FFF4DE; color: black; padding: 3px;"
   #define TMPL_VOD_BODY   "body {background-color: #FFF4DE;}"
#endif // _HAS_OWN_STYLES
#endif // __20140128__DEFINES_CHITRAM_TV_H

