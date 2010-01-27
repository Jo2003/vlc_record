/*********************** Information *************************\
| $HeadURL$
| 
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 09:19:48
| 
| Last edited by: $Author$
| 
| $Id$
\*************************************************************/
#ifndef __011810__DEFDEF_H
   #define __011810__DEFDEF_H

#include <QtGlobal>

#ifdef Q_OS_WIN32
   #define INI_DIR         "%1/vlc-record"
   #define APPDATA         "APPDATA"
#else
   #define INI_DIR         "%1/.vlc-record"
   #define APPDATA         "HOME"
#endif

#define INI_FILE          "vlc-rcd.ini"
#define KARTINA_HOST      "iptv.kartina.tv"
#define LOGO_PATH         "/img/ico/24"
#define LOGO_DIR          INI_DIR "/logos"
#define DEF_TIME_FORMAT   "MMM dd, yyyy hh:mm:ss"
#define LOG_FILE_NAME     "vlc-record.log"
#define DEF_TZ_STEP       1800  // time zone step is min. 30 minutes (1800 sec.) ...
#define DEF_MAX_DIFF      600   // accept inaccuracy up too xxx sec. ...
#define EPG_NAVBAR_HEIGHT 24
#define TIMER_REC_OFFSET  300   // 300 sec ... (5 minutes)
#define INVALID_ID        0xFFFFFFFF
#define TIMER_STBY_TIME   30    // 30 sec. before we should start record
#define MAX_NAME_LEN      10    // max. length of show name ...

#endif /* __011810__DEFDEF_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

