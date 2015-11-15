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

#ifdef __ARCH__I386
   #define __ARCH "i386"
#elif defined __ARCH__X86_64
   #define __ARCH "x86_64"
#else
   #define __ARCH ""
#endif


#if defined Q_OS_WIN32
   #define MY_BUILD_SYS "Windows"
#elif defined Q_OS_LINUX
   #define MY_BUILD_SYS "Linux " __ARCH
#elif defined Q_OS_MAC
   #define MY_BUILD_SYS "Mac"
#else
   #define MY_BUILD_SYS "unknown"
#endif

#define VERSION_MAJOR 2
#define VERSION_MINOR 72
#define VERSION_BUILD 6
#define BETA_EXT      "B"

#define __APPVERSION unsigned long((VERSION_MAJOR << 16) | (VERSION_MINOR << 8) | VERSION_BUILD)

#define STR_HELPER(x) #x
#define __APP_VSERSION_STRING(__major__, __minor__, __build__) \
   STR_HELPER(__major__) "." STR_HELPER(__minor__) "." STR_HELPER(__build__)


#define __MY__VERSION__ \
   __APP_VSERSION_STRING(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD) BETA_EXT " - " MY_BUILD_SYS ", built at " __DATE__

#endif /* __011910__VERSION_INFO_H */
/************************* History ***************************\
| --- new since last version ---
| 2015-11-15: Version 2.72.6B
| - after saving settings make re-login only if needed
| - re-request current stream after stream server change
|
| 2015-11-09: Version 2.72.5
| - add order information to player widget and info dialog
| - support API V2 functionality
| - vod favorites bugfix
| - speed test
| - supports 4 different stream standards
| - add udt patched VLC for Windows / Mac
|
| 2015-05-28: Version t.b.d.
| - add missing 3d format icon
|
| 2014-11-25: Version 2.72.0
| - add 3rd version number for bugfix releases
| - add icons for video quality in vod browser
| - bugfix: Position slider in VOD doesn't become active after
|   showing ad
|
| 2014-10-30: Version 2.71RC1
| - no changes since last beta
|
| 2014-10-26: Version 2.71B2
| - prepare a bugfix release with following changes:
|   + Make sure video context menu can be activated on Mac
|     -> make render widget transparent for mouse events -> fixed
|   + Click'n'Go slider stays still on record (VLC doesn't send
|     VOUT event on record) -> emulate event -> fixed
|   + No status messages on Linux due to missing X11 bypass
|     -> fixed
|   + No fullscreen on doubleclick on Linux when render widget
|     is transparent for mouse events -> catch
|     and count single clicks -> fixed
|
| 2014-10-16: Version 2.70xV3
| - add service and statistics for OEMs
|
| 2014-10-09: Version 2.70RC1
| - adapt build system for better debian packages
| - small changes in font size for click'n'go slider
|
| 2014-09-19: Version 2.69B20
| - add the possibility to use extended translation
|   using defines
|
| 2014-ß9-18: Version 2.69B19
| - bugfix: when pausing -> play timer wasn't started
|
| 2014-ß9-18: Version 2.69B18
| - show jump time as overlay
| - create pixmap from overlay text to get an outline
| - remove target time label from GUI
|
| 2014-09-17: Version 2.69B17
| - reduce blocking time when spooling is active (as possible)
| - in case of non existing archive let the user decide
|   if he wants to force new value or revert to last settings
| - spooling time can be re-triggered with time update
|
| 2014-09-05: Version 2.69B16
| - change icons for mute checkbox to make it more visible
| - archive not available is shown now for 7 seconds
|
| 2014-09-04: Version 2.69B15
| - bugfix for broken image download chain in case of missing
|   image
|
| 2014-09-01: Version 2.69B14
| - supports portable version (check for file .portable)
|
| 2014-08-28: Version 2.69B13
| - add extended EPG (might be disabled in advanced settings)
|
| 2014-08-08: Version 2.69B12
| - add advertising LCD page
| - add network state change message
| - use 90 min as ad block for seen videos
| - devide between network error and API error
|   (equal error numbers lead to strange behavior)
| - show network state message only when parent window exists
|
| 2014-08-06: Version 2.69B11
| - use externals_inc.h to include global stuff
| - VOD search now searches automated without the need
|   to click on search button
| - add clear button on channel filter and VOD search
| - advanced ad handling for VOD
| - make status message engine
| - add back link also on top
|
| 2014-08-05: Version 2.69B10
| - add status message system
| - when spooling in archive make sure new position is already
|   available in archive (if not, use status message)
| - fix a problem with click'n'go slider and time tooltip
|
| 2014-08-01: Version 2.69B9
| - better handle network errors
| - position slider shows timing information also when left
|   mouse button is pressed
| - shoe rating in video info
|
| 2014-06-04: Version 2.69B8
| - do not use isOnline stuff from QNetworkConfigurationManager
|   since it doesn't take care about virtual interfaces
| - when network config was changed a internet connection
|   check is done
|
| 2014-06-03: Version 2.69B7
| - many small changes in overlay control
| - click'n'go slider has a position tooltip
| - channel filter works while typing in
| - no API requests will be sent when network is offline
| - text size change now put into settings dialog
|
| 2014-05-20: Version 2.69B5
| - added various message boxes if:
|    + account soon ends
|    + load of channel list takes longer
|    + authentication problem
|    + multiple account usage
| - fix a glitch in vlc window context menu
| - animate overlay control to show / hide additional info / settings
| - add timing information to video slider
| - change skip button to fast forward / backward
| - add minutes label to time chooser
|
| 2014-01-26: Version 2.68RC1
| - updated help and language files
| - when exiting fullscreen restore previous display mode
| - can (again) enter mini mode using button on GUI
| - show control panel in mini mode as default
|
| 2013-12-19: Version 2.67B10
| - HLS now working for record and timer record as well
| - bugfix for not storing window position at close
| - bugfix for not showing overlay control panel on
|   2nd fullscreen toggle
| - can show control panel also in mini mode
|   (enable in context menu)
|
| 2013-12-17: Version 2.67B8
| - add workaround for HLS because libVLC can't handle
|   it correctly (needs testing!)
|
| 2013-12-03: Version 2.67B6
| - saves and restore choosen audio track
|
| 2013-11-29: Version 2.67B5
| - windowed mode is now minimal mode
| - minimal mode can be reached and leaved using player
|   context menu
| - picture size for VOD set to keep layout
| - mouse over video widget raises player window only in
|   fullscreen mode
| - add "stay on top" mode if minimal mode enabled
|
| 2013-11-26: Version 2.67B3
| - add buttons for windowed mode
| - add overlay to leave windowed mode
| - place windowed mode where player window was
| - create base class for fading widget
| - use fading widget for overlay controls
| - overlay controls fade in and out
|
| 2013-11-18: Version 2.67B2
| - use click'n'go slider for volume as well
| - support windowed mode (F11)
| - support shcreenshots (F12)
|
| 2013-09-19: Version 2.66
| - no changes since RC1
|
| 2013-09-06: Version 2.66RC1
| - updated help and translations
| - make sure "add to watch list button" is only displayed
|   when really needed
| - put an overlay on watch list button which shows the
|   number of entries in watch list
|
| 2013-08-20: Version 2.65B5
| - add new html writer class
| - use html writer in almost all cases html output is needed
| - re-style watch list and short info
| - correct url usage in recorder.cpp
| - about dialog uses html writer class
| - add watch list - a list to remember what
|   still to be watched
| - update add and remove button for watch list
| - bugfix for ORT widget where time stuck to 0:00
|
| 2013-07-30: Version 2.65B3
| - add channel list filter for channel name
| - add channel list filter for language (where available)
| - make api client and -parser global
| - make translators global
| - language change will take effect without reload
| - can get language from API server (where supported)
| - update some classes to be child of QObject
|
| 2013-07-17: Version 2.65B2
| - add a define which tells if vod manager is there
| - adapt translation
| - todo: adapt help
| - disable comboboxes for server, timeshift and bitrate if
|   there is no way to choose
|
| 2013-07-16: Version 2.65B1
| - add context menu to video widget
| - add deinterlace option to context menu and settings
| - can change audio track (selectable in context menu)
|
| 2013-07-12: Version 2.64
| - no changes to RC2
|
| 2013-07-08: Version 2.64 RC2
| - bugfix for nonworking search in VOD
|  (percent encoding was done twice)
| - make sure to don't use persistent http connections
|
| 2013-06-26: Version 2.64
| - RC1 for 2.64
| - merge changes for Novoe.TV into trunk
| - use QtJson instead of OJSON due to its smaller footprint
| - program update can be delayed for one version only
| - screenshots in help are unbranded
|
| 2013-06-05: Version 2.63B8
| - add missing parse for pass_protect and favorite
|   in JSON parser for video info
| - add command queue to iptv client so all requests
|   are completed in needed order
|
| 2013-04-24: Version 2.63B7
| - re-init libVlc after error event
|
| 2013-04-23: Version 2.63B6
| - show buffering info in time label
| - mouse wheel will change volume in fullscreen also when
|   positionend on control panel
|
| 2013-04-16: Version 2.63B5
| - use QJSON parser for VLC-Record
|
| 2013-04-08: Version 2.63
| - internal version used for customization tests
|
| 2013-03-26: Version 2.63B4
| - add support of external customization (first steps)
|   (search for *.cust file in resources folder)
|
| 2013-03-25: Version 2.63B3
| - make defines for api classes
| - make sure radio works on OEM
|
| 2013-03-18: Version 2.63B2
| - pixCache downloads are now done by network access manager
| - adapt header data to fix a problem on Linux
| - bugfix in channel list delegate (div / 0) on Mac
| - allow only requests when logged in, ignore anything else
| - add more request types so later iptv ctrl client can
|   the whole network access
|
| 2013-03-15: Version 2.63B1
| - add QIptvCtrlClient class based an QNetworkAccessManager
|   to handle API requests (QHttp was removed for API stuff)
| - todo: check behavior on error (logout, multi login)
|
| 2013-03-08: Version 2.62
| - no changes since last beta
|
| 2013-02-28: Version 2.61B6
| - support additional parameters for libVLC create
|   given in mod file (keyword: LVLC_OPTS)
|
| 2013-02-25: Version 2.61B5
| - make sure to request epg_current not to much
|   (workaround for channels with missing epg entries)
|
| 2013-02-22: Version 2.61B4
| - fix a bug where connection chain breaks due to
|   problems in API client (occurs on save settings)
|
| 2013-02-14: Version 2.61B3
| - intergrate wait trigger into kartina api client
| - remove waittrigger
| - make logout more stable
| - don't abort running http requests
|
| 2013-02-14: Version 2.61B2
| - volume now has a maximum value of 200%
|
| 2013-02-02: Version 2.61B1
| - bugfix: avoid to update EPG to often
|   (use epg_current instead)
|
| 2012-12-19: Version 2.60
| - replace CTimerEx (QTime based) with QTimerEx (QTimer based)
|
| 2012-12-17: Version 2.60RC1
| - will be final if all works well
|
| 2012-12-14: Version 2.60B8
| - little rework on record timer gui
| - finished docu + translation
|
| 2012-12-10: Version 2.60B7
| - add record timer
|
| 2012-12-06: Version 2.60B6
| - don't request whole channel list from API when some channels
|   need to be updated, but use /epg_current?cids=1,2,3&epg=3
| - bugfix: hide mouse in fullscreen only if video- or
|           control widget has focus
| - bugfix: pause in archiv doesn't work -> fixed
| - bugfix: cut show info to fit into info label
|
| 2012-12-04: Version 2.60B5
| - style comboboxes in overlay control panel
| - add show info update also for live stream
| - reload channel list only if needed
| - don't send API requests if not logged in or not
|   want to login
| - remove extended channel list settings
| - remove channel list refresh settings
| - add simple showinfo in overlay control panel
|
| 2012-12-03: Version 2.60B4
| - add aspect and crop combobox to overlay control
| - therefore a huge graphical redesign was needed
|   for that panel
| - pseudo caption now is used to move panel
|
| 2012-11-29: Version 2.60B3
| - overlay control is moveable
| - take care that videowidget gets focus so shortcuts work
| - add mute label to overlay control
| - queue libvlc events in QVector so we don't lose
|   any event
|
| 2012-11-27: Version 2.60B2
| - many small fixes
| - take care for mouse over
| - add a little eye candy: fade out overlay panel
|
| 2012-11-26: Version 2.60B1
| - add overlay control for fullscreen
| - add fusion control class to have all control elements
|   managed by one class
| - connect slots / signals to this class instead
|   of connecting to control elements itself
|
| 2012-09-27: Version 2.59
| - bugfix release for changed caching option in libVLC
|   (from http-caching --> network-caching)
|   Changes only in mod files!
| - take care of Qt transfer from Nokia to Digia
|
| 2012-09-11: Version 2.58
| - use newest Qt version
| - cleanup libVLC modules
| - fix for not showing ads
| - increase ipv4 timeout to 10 seconds
| - delete cookie on cases a logout would fail
| - use libVLC 2.0.1 on Mac (2.0.3 crashes)
| - use static const char* for aspect and crop formats
|   (hoping this will fix another mac crash problem)
|
| 2012-08-31: Version 2.57B11
| - pre-translit filename also if asking for file name
| - can omit player events
| - fix slider issue when using 9-th player modul
|
| 2012-08-30: Version 2.57B10
| - use icons for crop and aspect, this way the player window
|   can be smaller
| - add recDB version so we can defined update some stuff
| - add update functionality in recDB
| - downgrade will remove all database entries (settings)
|
| 2012-08-13: Version 2.57B8
| - use class own libvlc_media_list_t to avoid crashes on Mac
|
| 2012-08-11: Version 2.57B7
| - many tries to get no-idle working
| - again updated help due to translation changes
| - update translation
| - disable trayicon stuff on Mac - it only causes trouble
| - stop stream play if another one uses the account
| - add autoreleasepool on mac
| - try that libVLC handles no-idle stuff (needs testing)
| - fix a huge bug in CPlayer
|   + eventCallback causes big trouble when emiiting signals
|   + eventCallback now only stores event type
|   + a poller checks for state changes
|
| 2012-07-26: Version 2.57B4
| - use branded help
| - updated help
| - test for mac "no idle"
|
| 2012-07-10: Version 2.57B3
| - can drag / drop favourites into fav frame
| - add documentation in QtHelp format
| - add simple help view dialog
| - save / restore player position on fullscreen toggle
| - uses libVLC2.0.2 (Windows)
| - bugfix for non updating showInfo on show end
|
| 2012-06-13: Version 2.56
| - small changes in translation
| - delete parent code fields when wrong data was entered
|
| 2012-06-05: Version 2.55B10
| - put response error checking into kartina client class
| - build pseudo class "Kartina" to have access to Qt's
|   meta system (mainly for debug output)
| - clear buffered passwords on password error
|
| 2012-06-01: Version 2.55B7
| - new parental system ready (needs tests)
| - still missing translation for the new stuff
|
| 2012-05-30: Version 2.55B5
| - add password dialog
| - ask for password on password protected videos
| - show per icon that a video is protected
| - add favourite handling
|
| 2012-05-25: Version 2.55B1
| - add VOD Manager
| - add Channel Manager
| - first steps to add fovourites
|
| 2012-05-03: Version 2.54
| - fullscreen problem on Mac Snowleo fixed:
|   + use QStackedLayout to place the videoWidget inside
|     the main window at upper most layer
|   + after that make mainwindow fullscreen
| - as small minus the main window may flicker for a short
|   time when toggeling fullscreen
|
| 2012-04-26: Version 2.53B5
| - only show erotic channels in channel list if allowed
|   in settings
| - remove not working mac bugfix for fullscreen
| - try to make another change to raise renderView after
|   switching to fullscreen
| - erotic channels are now off by default again in
|   first time settings
| - on ubuntu add another package dependency to make sure
|   there are no GTK theme warnings at startup
| - libVLC is initiated to be quiet now (you can set
|   verbose level in settings [Ctrl+ALT+V])
|
| 2012-04-18: Version 2.53B1
| - add Qt translations so standard widgets will use right
|   translation
| - optimized global init flags for libVLC
| - many changes to the Mac port (mostly for App bundle)
|
| 2012-03-02: Version 2.52
| - take care on timejump for offset time in showinfo
| - many changes, tests and optimization on position slider
| - change style of position slider (untested)
| - remove progressbar from recorder because position slider
|   will make this job
| - add hardware acceleration switch in settings
| - add mac support (with some limitations)
| - change fullscreen stuff (simplification)
| - grab libVLC logging info from stderr
| - prepare for other projects (Kartina.TV, Afrobox, Polsky.TV)
| - VLC-Record project is back
| - many changes to simplify package creation
| - mouse wheel changes volume when in videoWidget
| - fix problems in timeshift stuff (pause handling)
| - fix mouse hiding in fullscreen mode
| - hide VOD if not supported by account
| - activate proxy server for libVLC
|
| ------ merge into trunk again ------
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
