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
#define PLAYER_LOG        INI_DIR "/player.log"
#define KARTINA_HOST      "iptv.kartina.tv"
#define LOGO_PATH         "/img/ico/24"
#define LOGO_DIR          INI_DIR "/logos"
#define DEF_TIME_FORMAT   "MMM dd, yyyy hh:mm:ss"
#define LOG_FILE_NAME     "vlc-record.log"
#define DEF_TZ_STEP       1800        // time zone step is min. 30 minutes (1800 sec.) ...
#define DEF_MAX_DIFF      600         // accept system clock inaccuracy up too 600 sec
#define EPG_NAVBAR_HEIGHT 24          // default height for EPG navbar
#define TIMER_REC_OFFSET  300         // 5 minutes in seconds
#define INVALID_ID        0xFFFFFFFF  // mark an id as invalid
#define TIMER_STBY_TIME   30          // 30 sec. before we should start record
#define MAX_NAME_LEN      10          // max. length of show name
#define ARCHIV_OFFSET     900         // 15 minutes after show start, archiv should be available
#define MAX_ARCHIV_AGE    1209000     // < 2 weeks in seconds
#define MAX_NO_FAVOURITES 10          // max. number of favourites ...

#endif /* __011810__DEFDEF_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

