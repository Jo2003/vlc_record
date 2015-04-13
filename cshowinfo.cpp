/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cshowinfo.cpp $
|
| Author: Jo2003
|
| Begin: 24.02.2010 / 10:41:34
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cshowinfo.cpp 1267 2013-12-17 13:55:47Z Olenka.Joerg $
\*************************************************************/
#include "cshowinfo.h"
#include "templates.h"
#include "small_helpers.h"
#include "chtmlwriter.h"
#include "qchannelmap.h"

// global html writer ...
extern CHtmlWriter *pHtml;

// global channel map ..
extern QChannelMap *pChanMap;


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
CShowInfo::CShowInfo(QObject *parent) : QObject(parent)
{
   cleanShowInfo();
}

/* -----------------------------------------------------------------\
|  Method: cleanShowInfo
|  Begin: 11.05.2012
|  Author: Jo2003
|  Description: clean whole showinfo
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::cleanShowInfo()
{
   sShowName      = "";
   sChanName      = "";
   sDescr         = "";
   sAdUrl         = "";
   sPCode         = "";
   iChannelId     = -1;
   iVodId         = -1;
   ePlayState     = IncPlay::PS_STOP;
   eShowType      = ShowInfo::Live;
   uiStart        = 0;
   uiEnd          = 0;
   uiJumpTime     = 0;
   bStreamLoader  = false;
   bHls           = false;
   ulLastEpgUpd   = 0;
   iDefAStream    = 0;
   epgMap.clear();
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
|  Method: setStartTime
|  Begin: 15.06.2010 / 14:25:00
|  Author: Jo2003
|  Description: stores the start time
|
|  Parameters: time in seconds
|
|  Returns: --.
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
void CShowInfo::setLastJumpTime(uint start)
{
   uiJumpTime = start;
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
|  Method: setShowType
|  Begin: 22.09.2011
|  Author: Jo2003
|  Description: stores the show type
|
|  Parameters: show type
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setShowType(ShowInfo::eProgType type)
{
   eShowType = type;
}

/* -----------------------------------------------------------------\
|  Method: setVodId
|  Begin: 04.11.2011
|  Author: Jo2003
|  Description: stores the VOD id
|
|  Parameters: VOD id
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setVodId(int id)
{
   iVodId = id;
}

/* -----------------------------------------------------------------\
|  Method: setHtmlDescr
|  Begin: 22.09.2011
|  Author: Jo2003
|  Description: stores the html description of a show
|
|  Parameters: html string
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setHtmlDescr(const QString &descr)
{
   sDescr = descr;
}

/* -----------------------------------------------------------------\
|  Method: setAdUrl
|  Begin: 12.05.2012
|  Author: Jo2003
|  Description: stores the ad url of a video
|
|  Parameters: url string
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setAdUrl(const QString &adUrl)
{
   sAdUrl = adUrl;
}

/* -----------------------------------------------------------------\
|  Method: setPCode
|  Begin: 01.06.2012
|  Author: Jo2003
|  Description: stores the pcode of protected channel
|
|  Parameters: url string
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setPCode(const QString &code)
{
   sPCode = code;
}

/* -----------------------------------------------------------------\
|  Method: setEpgMap
|  Begin: 05.03.2012
|  Author: Jo2003
|  Description: stores epg map
|
|  Parameters: ref. to epg map
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setEpgMap(const t_EpgMap &map)
{
   epgMap = map;
}

/* -----------------------------------------------------------------\
|  Method: useStreamLoader
|  Begin: 31.08.2012
|  Author: Jo2003
|  Description: do we use the streamloader?
|
|  Parameters: true or false
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::useStreamLoader(bool bUse)
{
   bStreamLoader = bUse;
}

/* -----------------------------------------------------------------\
|  Method: useHls
|  Begin: 17.12.2013
|  Author: Jo2003
|  Description: do we use the hls?
|
|  Parameters: true or false
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::useHls(bool bUse)
{
   bHls = bUse;
}

/* -----------------------------------------------------------------\
|  Method: setEpgUpdTime
|  Begin: 04.12.2012
|  Author: Jo2003
|  Description: set last epg update timestamp
|
|  Parameters: unix timestamp
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::setEpgUpdTime(ulong ts)
{
   ulLastEpgUpd = ts;
}

/* -----------------------------------------------------------------\
|  Method: epgUpdTime
|  Begin: 04.12.2012
|  Author: Jo2003
|  Description: get last epg update timestamp
|
|  Parameters: --
|
|  Returns: unix timestamp
\----------------------------------------------------------------- */
ulong CShowInfo::epgUpdTime()
{
   return ulLastEpgUpd;
}

/* -----------------------------------------------------------------\
|  Method: streamLoader
|  Begin: 31.08.2012
|  Author: Jo2003
|  Description: get streamloader flag
|
|  Parameters: --
|
|  Returns: true or false
\----------------------------------------------------------------- */
bool CShowInfo::streamLoader()
{
   return bStreamLoader;
}

/* -----------------------------------------------------------------\
|  Method: isHls
|  Begin: 17.12.2013
|  Author: Jo2003
|  Description: is hls used
|
|  Parameters: --
|
|  Returns: true or false
\----------------------------------------------------------------- */
bool CShowInfo::isHls()
{
   return bHls;
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
const uint &CShowInfo::lastJump()
{
   return uiJumpTime;
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
|  Method: showType
|  Begin: 22.09.2011
|  Author: Jo2003
|  Description: get the show type
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const ShowInfo::eProgType &CShowInfo::showType()
{
   return eShowType;
}

/* -----------------------------------------------------------------\
|  Method: vodId
|  Begin: 04.11.2011
|  Author: Jo2003
|  Description: get the VOD id
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const int &CShowInfo::vodId()
{
   return iVodId;
}

/* -----------------------------------------------------------------\
|  Method: canCtrlStream
|  Begin: 22.09.2011
|  Author: Jo2003
|  Description: is stream controlable?
|
|  Parameters: --
|
|  Returns: true --> yes
|          false --> no
\----------------------------------------------------------------- */
bool CShowInfo::canCtrlStream()
{
   if (((eShowType == ShowInfo::Archive) || (eShowType == ShowInfo::VOD))
       && ((ePlayState == IncPlay::PS_PLAY) || (ePlayState == IncPlay::PS_PAUSE)))
   {
      return true;
   }
   else
   {
      return false;
   }
}

/* -----------------------------------------------------------------\
|  Method: htmlDescr
|  Begin: 22.09.2011
|  Author: Jo2003
|  Description: get the show description
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const QString& CShowInfo::htmlDescr()
{
   return sDescr;
}

/* -----------------------------------------------------------------\
|  Method: adUrl
|  Begin: 12.05.2012
|  Author: Jo2003
|  Description: get the ad url
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const QString& CShowInfo::adUrl()
{
   return sAdUrl;
}

/* -----------------------------------------------------------------\
|  Method: pCode
|  Begin: 01.06.2012
|  Author: Jo2003
|  Description: get the pcode
|
|  Parameters: --
|
|  Returns: ref. to value
\----------------------------------------------------------------- */
const QString& CShowInfo::pCode()
{
   return sPCode;
}

/* -----------------------------------------------------------------\
|  Method: autoUpdate
|  Begin: 05.03.2012
|  Author: Jo2003
|  Description: auto update showInfo with values from epg
|               matching to given time
|  Parameters:  time to update showinfo to
|
|  Returns: 0 --> ok
|          -1 --> no entry matching to time found
\----------------------------------------------------------------- */
int CShowInfo::autoUpdate(uint uiTime)
{
   int     iRV = -1;
   int     iTs =  0;
   QString sChanNameTmpl;
   t_EpgMap::const_iterator cit;

   for (cit = epgMap.constBegin(); cit != epgMap.constEnd(); cit++)
   {
      if (CSmallHelpers::inBetween((*cit).uiStart, (*cit).uiEnd, uiTime))
      {
         iTs = pChanMap->value(iChannelId, true).iTs;

         if (eShowType == ShowInfo::Archive)
         {
            sChanNameTmpl = tr("%1 (Archive)").arg(sChanName);
         }
         else
         {
            sChanNameTmpl = sChanName;
         }

         sShowName  = (*cit).sShowName;
         uiStart    = (*cit).uiStart;
         uiEnd      = (*cit).uiEnd;
         uiJumpTime = (uiTime > (*cit).uiStart) ? uiTime : 0; // take care of relative time jumps!
         sDescr     = pHtml->createTooltip(sChanNameTmpl, QString("%1\n%2").arg((*cit).sShowName).arg((*cit).sShowDescr), uiStart, uiEnd, iTs);
         iRV = 0;
         break;
      }
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: updWithChanEntry
|  Begin: 30.01.2013
|  Author: Jo2003
|  Description: update show info with channel entry
|
|  Parameters:  time to update, channel entry
|
|  Returns: --
\----------------------------------------------------------------- */
void CShowInfo::updWithChanEntry (ulong ulTime, const cparser::SChan &entry)
{
   QString sChanNameTmpl;

   if (eShowType == ShowInfo::Archive)
   {
      sChanNameTmpl = tr("%1 (Archive)").arg(sChanName);
   }
   else
   {
      sChanNameTmpl = sChanName;
   }

   sShowName    = entry.sProgramm;
   uiStart      = entry.uiStart;
   uiEnd        = entry.uiEnd;
   uiJumpTime   = (ulTime > entry.uiStart) ? ulTime : 0; // take care of relative time jumps!
   ulLastEpgUpd = 0; // updated show info -> be open for new updates ...
   sDescr       = pHtml->createTooltip(sChanNameTmpl, entry.sProgramm, entry.uiStart, entry.uiEnd, entry.iTs);
}

//---------------------------------------------------------------------------
//
//! \brief   store default audio track index
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   idx (int) audio track index
//
//! \return  --
//---------------------------------------------------------------------------
void CShowInfo::setDefAStream(int idx)
{
   iDefAStream = idx;
}

//---------------------------------------------------------------------------
//
//! \brief   get default audio track index
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   --
//
//! \return  audio track index
//---------------------------------------------------------------------------
const int &CShowInfo::defAStream()
{
   return iDefAStream;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
