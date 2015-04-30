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
#elif defined _TASTE_MOIDOM_TV
   #include <defines_moidom_tv.h>
#elif defined _TASTE_NOVOE_TV
   #include <defines_novoe_tv.h>
#elif defined _TASTE_IPTV_RECORD
   #include <defines_iptv_record.h>
#elif defined _TASTE_LTVISION
   #include <defines_ltvision.h>
#elif defined _TASTE_OEM
   #include <defines_oem_record.h>
#elif defined _TASTE_SUNDUK_TV
   #include <defines_sunduk_tv.h>
#elif defined _TASTE_CHITRAM_TV
   #include <defines_chitram_tv.h>
#elif defined _TASTE_TELEPROM
   #include <defines_teleprom.h>
#elif defined _TASTE_IDC_TB
   #include <defines_idc_tv.h>
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

// define API path if not already defined ...
#ifndef API_XML_PATH
   #define API_XML_PATH   "/api/xml/"
#endif
#ifndef API_JSON_PATH
   #define API_JSON_PATH  "/api/json/"
#endif

// define classes of api client ...
#ifndef ApiClient
   #define ApiClient      CKartinaClnt
#endif
#ifndef ApiParser
   #define ApiParser      CStdJsonParser
#endif

// software developer (used in software id)
#define SOFTID_DEVELOPER   "jo-001"

// application shortcut ...
#ifndef APPLICATION_SHORTCUT
#  define APPLICATION_SHORTCUT "ktv"
#endif

// operating system ...
#ifdef Q_OS_LINUX
#  define OP_SYS  "linux"
#elif defined Q_OS_WIN32
#  define OP_SYS  "win"
#elif defined Q_OS_MAC
#  define OP_SYS  "mac"
#else
#  define OP_SYS  "ukn"
#endif // op sys

#define PLAYER_LOG_FILE   "player.log"
#define LIBVLC_LOG_FILE   "libvlc.log"
#define MOD_DIR           "modules"
#define LANG_DIR          "language"
#define LANG_DIR_QT       "translations"
#define LOGO_DIR          "logos"
#define VOD_DIR           "vod"
#define DOC_DIR           "doc"
#define RES_DIR           "resources"
#define LOGO_URL          "/img/ico/24"
#define DEF_TIME_FORMAT   "MMM dd, yyyy hh:mm:ss"
#define DEF_TZ_STEP       1800            // time zone step is min. 30 minutes (1800 sec.) ...
#define DEF_MAX_DIFF      600             // accept system clock inaccuracy up too 600 sec
#define EPG_NAVBAR_HEIGHT 24              // default height for EPG navbar
#define TIMER_REC_OFFSET  300             // 5 minutes in seconds
#define INVALID_ID        0xFFFFFFFF      // mark an id as invalid
#define TIMER_STBY_TIME   30              // 30 sec. before we should start record
#define MAX_NAME_LEN      10              // max. length of show name
#define ARCHIV_OFFSET     1800            // 30 minutes after show start, archiv should be available
#define MAX_ARCHIV_AGE    1209000         // < 2 weeks in seconds
#define MAX_NO_FAVOURITES 10              // max. number of favourites ...
#define MAX_FAV_IN_ROW    5               // number of favorites in row
#define MIN_CACHE_SIZE    5000000         // < 5 MB ...
#define TIME_OFFSET       (35 * 365 * 24 * 3600) // make the slider handle gmt
#define VIDEOS_PER_SITE   20              // number of videos / site
#define GPU_ACC_TOKEN     ":ffmpeg-hw"    // mrl option to use GPU acceleration
#define EPG_UPD_TMOUT     120             // make sure epg update comes not to much
#define MAX_LVLC_ARGS     16              // max arguments to give to libVLC at creation
#define VOD_POSTER_WIDTH  120             // picture size for VOD (width)
#define VOD_POSTER_HEIGHT 170             // picture size for VOD (height)
#define HTTP_REQ_TIMEOUT  90000           // 90 seconds (this should be enough)
#define PAUSE_WORKS_FOR_SECS 300          // we assume pause works well for up to 5 minutes ...
#define PAUSE_RESUME_VOD    -2899         // any uncommon number ....
#define ADBLOCK_ACTIVE    5400            // one and a half hour ...
#define EXT_EPG_TIME      7200            // time to include from day before ...
#define PORTABLE_MARKER   ".portable"     // mark release as portable ...
#define PORTABLE_DATA_DIR "portable_data" // data folder for portable version
#define MARK_SPECIAL      (-2899)         // mark something special

// a namespace for translators ...
namespace Msg
{
   enum eMsgType
   {
      Info,
      Warning,
      Error,
      Unknown
   };
}

#endif /* __011810__DEFDEF_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

