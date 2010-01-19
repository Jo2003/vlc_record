/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/defdef.h $
| 
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 09:19:48
| 
| Last edited by: $Author: joergn $
| 
| $Id: defdef.h 175 2010-01-19 14:22:13Z joergn $
\*************************************************************/
#ifndef __011810__DEFDEF_H
   #define __011810__DEFDEF_H

#ifdef WIN32
   #define INI_DIR         "%1/vlc-record"
   #define APPDATA         "APPDATA"
#else
   #define INI_DIR         "%1/.vlc-record"
   #define APPDATA         "HOME"
#endif

#define INI_FILE        "vlc-rcd.ini"
#define KARTINA_HOST    "iptv.kartina.tv"
#define LOGO_PATH       "/img/ico/24"
#define LOGO_DIR        INI_DIR "/logos"
#define DEF_TIME_FORMAT "MMM dd, yyyy hh:mm:ss"
#define DEF_TZ_STEP     1800  // time zone step is min. 30 minutes (1800 sec.) ...
#define DEF_MAX_DIFF    600   // accept inaccuracy up too xxx sec. ...

#endif /* __011810__DEFDEF_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

