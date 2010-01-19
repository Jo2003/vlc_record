/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/version_info.h $
| 
| Author: Joerg Neubert
|
| Begin: 19.01.2010 / 16:23:07
| 
| Last edited by: $Author: joergn $
| 
| $Id: version_info.h 176 2010-01-19 15:29:52Z joergn $
\*************************************************************/
#ifndef __011910__VERSION_INFO_H
   #define __011910__VERSION_INFO_H

#ifdef WIN32
   #define MY_BUILD_SYS "Windows"
#else
   #define MY_BUILD_SYS "Linux (x86)"
#endif

#define __MY__VERSION__ \
   "1.00 - " MY_BUILD_SYS ", built at " __DATE__

#endif /* __011910__VERSION_INFO_H */
/************************* History ***************************\
| 2010-01-19: Version 1.00 
| - missing comments added 
| - prepare for linux build 
| - reload epg only if we are at current day 
\*************************************************************/

