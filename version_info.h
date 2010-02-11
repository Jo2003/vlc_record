/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 16:23:07
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011910__VERSION_INFO_H
   #define __011910__VERSION_INFO_H
#include <QtGlobal>
#include "customization.h"

#if defined Q_OS_WIN32
   #define MY_BUILD_SYS "Windows"
#elif defined Q_OS_LINUX
   #define MY_BUILD_SYS "Linux (x86)"
#elif defined Q_OS_DARWIN
   #define MY_BUILD_SYS "Mac"
#else
   #define MY_BUILD_SYS "unknown"
#endif

#define __MY__VERSION__ \
   "1.16" VERSION_APPENDIX " - " MY_BUILD_SYS ", built at " __DATE__

#endif /* __011910__VERSION_INFO_H */
/************************* History ***************************\
| 2010-02-11: Version 1.16
| - bugfix for crash if no player module is there
|
| 2010-02-11: Version 1.15
| - feature player modules (to support more than vlc only)
| - option to start player detached
| - option to translit file name
| - update translation with Alex' hints
|
| 2010-02-05: Version 1.14
| - remove mutex from http classes, use request id instead
| - archive available for 2 weeks only
| - archive available 10 minutes after the show starts
| - title tags in epg view for icons added
| - don't store window position if maximized
|
| 2010-02-02: Version 1.13
| - add enlarge / reduce text size function
| - some new and updated icons
| - replace icons with PNGs to reduce app size
|
| 2010-02-02: Version 1.12
| - don't start vlc detached
| - more control vlc using vlc-record
| - shutdown added
| - logging macros added
|
| 2010-01-28: Version 1.11
| - supports silent record
| - rework settings dialog
| - better systray handling
| - save and restore window position
|
| 2010-01-26: Version 1.10
| - first version with working timer record (more tests needed)
|
| 2010-01-23: Version 1.09
| - switch language only when settings are saved
|   (bugfix for segfault on some XP and Linux)
|
| 2010-01-21: Version 1.08
| - need stream url as former
| - add params for VLC (buffer)
|
| 2010-01-21: Version 1.07
| - can now set stream server
| - can now set http cache time
| - parsing stream url no longer removes additional params
|    (needs testing!!)
|
| 2010-01-21: Version 1.06
| - create / translate epg navbar using only one function
| - double click on channel list starts channel play
|
| 2010-01-20: Version 1.05
| - add advanced epg navbar
|
| 2010-01-20: Version 1.01
| - try to fix time offset problem
|
| 2010-01-19: Version 1.00
| - missing comments added
| - prepare for linux build
| - reload epg only if we are at current day
\*************************************************************/

