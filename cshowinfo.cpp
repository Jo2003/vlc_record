/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.02.2010 / 10:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cshowinfo.h"


/* -----------------------------------------------------------------\
|  Method: CShowInfo / constructor
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: init values
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CShowInfo::CShowInfo()
{
   iChannelId = -1;
   bIsArchive = false;
   ePlayState = IncPlay::PS_WTF;
   uiStart    = 0;
   uiEnd      = 0;
   iJumpTime  = 0;
}

/* -----------------------------------------------------------------\
|  Method: setShowName
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the show name
|
|  Parameters: ref. to name
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setShowName (const QString &name)
{
   sShowName = name;
}

/* -----------------------------------------------------------------\
|  Method: setChanName
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the channel name
|
|  Parameters: ref. to name
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setChanName (const QString &name)
{
   sChanName = name;
}

/* -----------------------------------------------------------------\
|  Method: setChanId
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the channel id
|
|  Parameters: id
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setChanId (int id)
{
   iChannelId = id;
}

/* -----------------------------------------------------------------\
|  Method: setArchive
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the archive marker
|
|  Parameters: archive marker
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setArchive (bool arch)
{
   bIsArchive = arch;
}

/* -----------------------------------------------------------------\
|  Method: setStartTime
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the start time
|
|  Parameters: time in seconds
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setStartTime (uint start)
{
   uiStart = start;
}

/* -----------------------------------------------------------------\
|  Method: setLastJumpTime
|  Begin: 22.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the last jump time
|
|  Parameters: time in seconds
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setLastJumpTime(int start)
{
   iJumpTime = start;
}

/* -----------------------------------------------------------------\
|  Method: setEndTime
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the end
|
|  Parameters: time in seconds
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setEndTime (uint end)
{
   uiEnd = end;
}

/* -----------------------------------------------------------------\
|  Method: setPlayState
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the play state
|
|  Parameters: play state
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setPlayState (IncPlay::ePlayStates state)
{
   ePlayState = state;
}

/* -----------------------------------------------------------------\
|  Method: showName
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the show name
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const QString &CShowInfo::showName()
{
   return sShowName;
}

/* -----------------------------------------------------------------\
|  Method: chanName
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the channel name
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const QString &CShowInfo::chanName()
{
   return sChanName;
}

/* -----------------------------------------------------------------\
|  Method: channelId
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the channel id
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const int &CShowInfo::channelId()
{
   return iChannelId;
}

/* -----------------------------------------------------------------\
|  Method: starts
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the start time
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const uint &CShowInfo::starts()
{
   return uiStart;
}

/* -----------------------------------------------------------------\
|  Method: lastJump
|  Begin: 22.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the last jump time
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const int &CShowInfo::lastJump()
{
   return iJumpTime;
}

/* -----------------------------------------------------------------\
|  Method: ends
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the end time
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const uint &CShowInfo::ends()
{
   return uiEnd;
}

/* -----------------------------------------------------------------\
|  Method: playState
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the play state
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const IncPlay::ePlayStates &CShowInfo::playState()
{
   return ePlayState;
}

/* -----------------------------------------------------------------\
|  Method: archive
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: get the archive marker
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const bool &CShowInfo::archive()
{
   return bIsArchive;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
