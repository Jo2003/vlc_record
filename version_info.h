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
#elif defined Q_OS_MAC
   #define MY_BUILD_SYS "Mac"
#else
   #define MY_BUILD_SYS "unknown"
#endif

#ifdef INCLUDE_LIBVLC
   #define VERSION_MAJOR "2"
#else
   #define VERSION_MAJOR "1"
#endif

#define __MY__VERSION__ \
   VERSION_MAJOR ".26" VERSION_APPENDIX " - " MY_BUILD_SYS ", built at " __DATE__

#endif /* __011910__VERSION_INFO_H */
/************************* History ***************************\
| --- new since last version ---
|
| 2010-03-30: Version x.26
| - shortcuts for favourites added
| - missing shortcuts for font size added
| - feed libVLC with UTF-8 strings
| - pause shortcut added
| - pause, jump forward / backward only works on archive play
|
| 2010-03-25: Version x.25
| - fake shortcut keypress from player class
| - Hotkey Alt+{x} now also works on fullscreen
| - bugfix for APPDATA unicode problem
| - "--vout-event=3" handling on win32 only
| - jump 2 minutes forward on hotkey "->" and
|   2 minutes backward on hotkey "<-"
|   (will only work in archive view)
|
| 2010-03-11: Version x.23
| - bugfix for segfault on program exit if player still is
|   playing
|
| 2010-03-10: Version x.22
| - patched d3d dll so libVLC will not catch keypress events
| - hotkeys for aspect ratio and full screen (all for 2.xx)
| - better state machine to enable / disable buttons as needed
| - new state display
| - remove ugli player state from player widget
|
| 2010-03-05: Version x.20 BETA
| - try to integrate vlc player into gui using libVlc
| - log entries from libvlc
| - decide in project file to build with / without libvlc
| - new mod file to support libvlc
|
| 2010-02-26: Version 1.19
| - add favourites
| - fix problem with new archiv protocol
| - add forced mux option to be more flexible in mod files
| - on translit make whole filename uppercase
| - add special AVI vlc module
|
| 2010-02-18: Version 1.18
| - store font size, splitter position and maximized
|   option in ini file and load settings at next start
|
| 2010-02-15: Version 1.17
| - add splitter between channels and epg
| - own function to create time from string because
|   localized string is needed in QDateTime::fromString()
| - store time value as uint also in channel list
| - textsize changes now also in channel list and group box
|
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

