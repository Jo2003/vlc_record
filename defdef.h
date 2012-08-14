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

#ifdef _TASTE_KARTINA_TV
   #include <defines_kartina_tv.h>
#elif defined _TASTE_POLSKY_TV
   #include <defines_polsky_tv.h>
#elif defined _TASTE_AFROBOX
   #include <defines_afrobox.h>
#else
   #include <defines_vlc_record.h>
#endif // _TASTE_XXX


#ifdef Q_OS_WIN32
   #define DATA_DIR_ENV   "APPDATA"
   #define DATA_DIR       APP_NAME
#else
   #define DATA_DIR_ENV   "HOME"
   #define DATA_DIR       "." APP_NAME
#endif

#ifdef __GNUC__
   #define __UNUSED        __attribute__ ((unused))
#else
   #define __UNUSED
#endif

#define APP_LOG_FILE      BIN_NAME ".log"
#define PLAYER_LOG_FILE   "player.log"
#define LIBVLC_LOG_FILE   "libvlc.log"
#define MOD_DIR           "modules"
#define LANG_DIR          "language"
#define LANG_DIR_QT       "translations"
#define LOGO_DIR          "logos"
#define VOD_DIR           "vod"
#define DOC_DIR           "doc"
#define KARTINA_API_PATH  "/api/xml/"
#define LOGO_URL          "/img/ico/24"
#define DEF_TIME_FORMAT   "MMM dd, yyyy hh:mm:ss"
#define DEF_TZ_STEP       1800            // time zone step is min. 30 minutes (1800 sec.) ...
#define DEF_MAX_DIFF      600             // accept system clock inaccuracy up too 600 sec
#define EPG_NAVBAR_HEIGHT 24              // default height for EPG navbar
#define TIMER_REC_OFFSET  300             // 5 minutes in seconds
#define INVALID_ID        0xFFFFFFFF      // mark an id as invalid
#define TIMER_STBY_TIME   30              // 30 sec. before we should start record
#define MAX_NAME_LEN      10              // max. length of show name
#define ARCHIV_OFFSET     900             // 15 minutes after show start, archiv should be available
#define MAX_ARCHIV_AGE    1209000         // < 2 weeks in seconds
#define MAX_NO_FAVOURITES 10              // max. number of favourites ...
#define MIN_CACHE_SIZE    6000000         // < 6 MB ...
#define TIME_OFFSET       (35 * 365 * 24 * 3600) // make the slider handle gmt
#define VIDEOS_PER_SITE   20              // number of videos / site
#define GPU_ACC_TOKEN     ":ffmpeg-hw"    // mrl option to use GPU acceleration

// a namespace for translators ...
namespace Translators
{
   enum ETranslators
   {
      TRANS_QT,
      TRANS_OWN,
      TRANS_MAX
   };
}

#endif /* __011810__DEFDEF_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

