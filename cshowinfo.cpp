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
#include "templates.h"
#include "small_helpers.h"


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
   sShowName     = "";
   sChanName     = "";
   sDescr        = "";
   sAdUrl        = "";
   sPCode        = "";
   iChannelId    = -1;
   iVodId        = -1;
   ePlayState    = IncPlay::PS_STOP;
   eShowType     = ShowInfo::Live;
   uiStart       = 0;
   uiEnd         = 0;
   uiJumpTime    = 0;
   bStreamLoader = false;
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
   if ((eShowType == ShowInfo::Archive)
      || (eShowType == ShowInfo::VOD))
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
   int iRV = -1;
   t_EpgMap::const_iterator cit;

   for (cit = epgMap.constBegin(); cit != epgMap.constEnd(); cit++)
   {
      if (CSmallHelpers::inBetween((*cit).uiStart, (*cit).uiEnd, uiTime))
      {
         sShowName  = (*cit).sShowName;
         uiStart    = (*cit).uiStart;
         uiEnd      = (*cit).uiEnd;
         uiJumpTime = (uiTime > (*cit).uiStart) ? uiTime : 0; // take care of relative time jumps!
         sDescr     = QString(TMPL_BACKCOLOR)
                         .arg("rgb(255, 254, 212)")
                         .arg(createTooltip(tr("%1 (Archive)").arg(sChanName),
                                 QString("%1 %2").arg((*cit).sShowName).arg((*cit).sShowDescr),
                                 uiStart, uiEnd));
         iRV = 0;
         break;
      }
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: createTooltip [static]
|  Begin: 22.03.2010 / 18:15
|  Author: Jo2003
|  Description: create a tooltip for given data
|
|  Parameters: channel name, program description,
|              start time, end time
|
|  Returns: tool tip string
\----------------------------------------------------------------- */
QString CShowInfo::createTooltip (const QString & name, const QString & prog, uint start, uint end)
{
   // create tool tip with programm info ...
   QString sToolTip = PROG_INFO_TOOL_TIP;
   sToolTip.replace(TMPL_PROG, tr("Program:"));
   sToolTip.replace(TMPL_START, tr("Start:"));
   sToolTip.replace(TMPL_END, tr("End:"));
   sToolTip.replace(TMPL_TIME, tr("Length:"));

   sToolTip = sToolTip.arg(name).arg(prog)
               .arg(QDateTime::fromTime_t(start).toString(DEF_TIME_FORMAT))
               .arg(end ? QDateTime::fromTime_t(end).toString(DEF_TIME_FORMAT) : "")
               .arg(end ? tr("%1 min.").arg((end - start) / 60)                : "");

   return sToolTip;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
