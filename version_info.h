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

#define VERSION_MINOR "51"
#define BETA_EXT      "B2"

#define __MY__VERSION__ \
   VERSION_MAJOR "." VERSION_MINOR BETA_EXT VERSION_APPENDIX " - " MY_BUILD_SYS ", built at " __DATE__

#endif /* __011910__VERSION_INFO_H */
/************************* History ***************************\
| --- new since last version ---
| - add hardware acceleration switch in settings
|
| 2011-12-01: Version 2.50
| - add new bitrate "mobile"
| - update shortinfo / slider / progress on archive play
|   on show change
| - use older libvlc 1.1.7 which still can remux mp4 to ts
| - use timeshift module of libVLC to buffer stream on
|   pause (if it works, it makes life much easier)
|
| 2011-10-12: Version x.49B3
| - updated translation
| - add update check at startup
| - don't show systray icon when not minimizing to systray
| - when searching without searchstring, show videos normal
| - searching shows up too 100 results
|
| 2011-09-29: Version x.49B2
| - add first time settings dialog
| - store pre-defined values into config to make
|   a good first start
| - rework on VOD to have max. 20 entries on one site
| - add filter "newest" and "best"
| - VOD search request goes now out to kartina.tv
| - reworked settings dialog
| - EPG access only 2 weeks back / 1 week ahead
| - in short info window show only what's playing now
| - rework for progress bar to show progress of playing show
| - memory function for last seen channel / day
| - rework on wait trigger to add command queue
| - major GUI rework to make VOD easier accessable
| - many updated translations
| - updated installation
|
| ------ branch to make an official Kartina.TV app ------
|
| - add app bundle support when getting VLCs file name
| - own translation of error codes sent by kartina.tv
| - more common error message box
| - default player modules 1_vlc-player.mod and 5_libvlc.mod
|
| 2011-07-10: Version x.45
| - use new libVLC 1.1.10
| - updated CleanShowName() to remove any forbidden filename
|   character
|
| 2011-04-15: Version x.44
| - no changes to last beta
|
| 2011-04-13: Version x.44 Beta 1
| - delete old db timerrec jobs from database
| - new shortcut table
| - take care of unique shortcuts
|
| 2011-03-30: Version x.43
| - make sure shortcut table is only re-translated
|   if there is something to translate
| - add shortcut reset button
| - updated translation
|
| 2011-03-25: Version x.42B3
| - fix fullscreen shortcut problems
| - shortcuts for volume + / - / mute
| - re-translate shortcut table on language change
| - use libVLC 1.1.8  (Windows only)
| - display version of libVLC in about dialog
|
| 2011-03-23: Version x.42B2
| - remove channel list widget
| - add new channel list view
| - remove channel list item class
| - use item delegate to draw custom channel list
| - better style channel item
|
| 2011-03-18: Version x.42B1
| - advanced channel list (enable in settings)
| - add shortcut grabber class
| - add shortcut table in settings
| - use customizable shortcuts
| - remove static shortcuts from layouts
| - use libVLC 1.17 (Windows only)
|
| 2011-02-02: Version x.40B10
| - you now can change the API path in settings dialog using
|   shortcut "CTRL+ALT+A"
|
| 2011-01-25: Version x.40B9
| - display expire data in about dialog
| - new function "oneLevelParser" to make xml parsing
|   more common
|
| 2011-01-24: Version x.40B8
| - clean xml response to be more robust when kartina sends
| - fix channel list parser to support new entries
| - set timeshift in epgbrowser to 0 if no timeshift is
|   supported by channel
| - add bitrate switch stuff
|
| 2011-01-11: Version x.40B7
| - quick'n'dirty fix for new stream params in channel list
|
| 2011-01-10: Version x.40B6
| - use libvlc 1.15
| - don't check for vlc player path when using libvlc
|
| 2011-01-07: Version x.40B5
| - in archive play, pushing record will start record at
|   play position
|
| 2011-01-05: Version x.40B4
| - replace stream protocol info from url with http
|
| 2010-12-28: Version x.40B3
| - fast forward / backward in VOD possible
| - change archive slider stuff
|
| 2010-12-23: Version x.40B2
| - vod support
| - own downloader added
| - simple registration added
|
| 2010-10-01: Version x.39
| - use libvlc 1.14
| - fix 20 minutes archive bug
| - add test module for avi / xvid re-encode
|
| 2010-08-08: Version x.38
| - no changes from the last beta x.37B2
|
| 2010-08-02: Version x.37Beta2
| - play button changes to pause butto when supported
| - use different main functions for windows | others
| - new install script creates plugin cache
|
| 2010-07-30: Version x.37Beta
| - major code change to support kartinas Rest API
| - logout when closing app
|
| 2010-07-27: Version x.36Beta5
| - hide mouse pointer in fullscreen mode after 1 second
| - major code cleaning in player so libvlc is allocated
|   only once
| - set options for playback / record per media
| - therefore changes in modules
| - allocate memory for app and dialog dynamically
| - mark recorder dialog for delete later
|
| 2010-07-25: Version x.36Beta4
| - disable spooling while another spool request is active
|
| 2010-07-23: Version x.36Beta3
| - use official build of libVLC 1.1.1
|
| 2010-07-22: Version x.26Beta2
| - uses libVLC 1.11
| - fullscreen is located on screen where player widget is
|   located
|
| 2010-07-13: Version x.33Beta3
| - lock logging stuff with mutex
| - trigger next log check only when leaving actual check
| - use optimized window flags when reparenting
| - when jumping forward / backward make sure we're
|   not entering another show
|
| 2010-06-25: Version x.33Beta2
| - fix a bug where aspect or crop wasn't updated
|
| 2010-06-23: Version x.33Beta
| - more buffering values to support bad connections
| - when opening timer record window, active channel
|   is the default one
| - time jump with variable time value
| - new position slider for archive play (works exactly)
| - stores aspect ratio + crop ratio for every channel
| - uses SQLite to store data
| - uses libVLC 1.10 (with patch for event handling)
| - new layout optimized for widescreen!!!
|
| 2010-04-18: Version x.29
| - bug: if show title contains slashes or backslashes
|   record will not work because vlc will not create the
|   pseudo directory --> fixed
|
| 2010-04-09: Version x.28
| - add some more info to LCD display
| - fix unpause problem in archive
|
| 2010-04-09: Version x.27
| - for time jump take care of pause
| - encapsulate playtime / pause stuff into CTimerEx class
| - fix for non working second+ time jump
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

