/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: Monday, January 04, 2010 16:13:58
|
| $Id$
|
\=============================================================================*/
#include "ckartinaclnt.h"

// log file functions ...
extern CLogFile VlcLog;

/*-----------------------------------------------------------------------------\
| Function:    CKartinaClnt / constructor
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: constructs a CKartinaClnt object to communicate with
|              kartina.tv
| Parameters:  --
|
\-----------------------------------------------------------------------------*/
CKartinaClnt::CKartinaClnt(QObject *parent) :QIptvCtrlClient(parent)
{
   sUsr           = "";
   sPw            = "";
   sCookie        = "";
   sApiUrl        = "";
   fillErrorMap();

   connect(this, SIGNAL(sigStringResponse(int,QString)), this, SLOT(slotStringResponse(int,QString)));
   connect(this, SIGNAL(sigBinResponse(int,QByteArray)), this, SLOT(slotBinResponse(int,QByteArray)));
   connect(this, SIGNAL(sigErr(QString,int)), this, SLOT(slotErr(QString,int)));
}

/*-----------------------------------------------------------------------------\
| Function:    ~CKartinaClnt / destructor
|
| Author:      Jo2003
|
| Begin:       Thursday, January 07, 2010 11:54:52
|
| Description: clean at destruction ...
|
| Parameters:  --
|
\-----------------------------------------------------------------------------*/
CKartinaClnt::~CKartinaClnt()
{
   // abort();
}

//---------------------------------------------------------------------------
//
//! \brief   handle string response from API server
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   reqId (int) request identifier
//! \param   strResp (QString) response string
//
//! \return  --
//---------------------------------------------------------------------------
void CKartinaClnt::slotStringResponse (int reqId, QString strResp)
{
   int     iErr = 0;
   QString sCleanResp;

   if (reqId == (int)Kartina::REQ_LOGOUT)
   {
       sCookie = "";

       // send response ...
       emit sigHttpResponse ("", reqId);
   }
   else
   {
      mInfo(tr("Request '%2' done!").arg(karTrace.reqValToKey((Kartina::EReq)reqId)));

      // check response ...
      if ((iErr = checkResponse(strResp, sCleanResp)) != 0)
      {
         emit sigError(sCleanResp, reqId, iErr);
      }
      else
      {
         // send response ...
         emit sigHttpResponse (sCleanResp, reqId);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   handle binary response from API server (maybe icons?)
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   reqId (int) request identifier
//! \param   binResp (QByteArray) binary data
//
//! \return  --
//---------------------------------------------------------------------------
void CKartinaClnt::slotBinResponse (int reqId, QByteArray binResp)
{
   Q_UNUSED(reqId)
   Q_UNUSED(binResp)
}

//---------------------------------------------------------------------------
//
//! \brief   handle error from API server
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   sErr (QString) error string
//! \param   iErr (int) error code
//
//! \return  --
//---------------------------------------------------------------------------
void CKartinaClnt::slotErr (QString sErr, int iErr)
{
   emit sigError(sErr, (int)Kartina::REQ_UNKNOWN, iErr);
}

/////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------\
|  Method: queueRequest
|  Begin: 19.01.2010 / 15:55:06
|  Author: Jo2003
|  Description: request kartina action
|
|  Parameters: action, action params
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaClnt::queueRequest(Kartina::EReq req, int iArg1, int iArg2)
{
   Kartina::SRequest cmd;

   cmd.req      = req;
   cmd.iOptArg1 = iArg1;
   cmd.iOptArg2 = iArg2;
   cmd.sOptArg1 = "";
   cmd.sOptArg2 = "";
   queueIn(cmd);
}

/* -----------------------------------------------------------------\
|  Method: queueRequest
|  Begin: 19.01.2010 / 15:55:06
|  Author: Jo2003
|  Description: request kartina action
|
|  Parameters: action, action params
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaClnt::queueRequest (Kartina::EReq req, const QString &sReq1, const QString &sReq2)
{
   Kartina::SRequest cmd;

   cmd.req      = req;
   cmd.iOptArg1 = -1;
   cmd.iOptArg2 = -1;
   cmd.sOptArg1 = sReq1;
   cmd.sOptArg2 = sReq2;
   queueIn(cmd);
}

/* -----------------------------------------------------------------\
|  Method: queueRequest
|  Begin: 30.05.2012
|  Author: Jo2003
|  Description: request kartina action
|
|  Parameters: action, action params
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaClnt::queueRequest (Kartina::EReq req, int iArg1, const QString &sArg1)
{
   Kartina::SRequest cmd;

   cmd.req      = req;
   cmd.iOptArg1 = iArg1;
   cmd.iOptArg2 = -1;
   cmd.sOptArg1 = sArg1;
   cmd.sOptArg2 = "";
   queueIn(cmd);
}

/* -----------------------------------------------------------------\
|  Method: queueIn
|  Begin: 28.09.2011
|  Author: Jo2003
|  Description: queue in new command, check for cookie and abort
|
|  Parameters: ref. to command
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaClnt::queueIn(const Kartina::SRequest &req)
{
   // handle request ...
   switch (req.req)
   {
   case Kartina::REQ_CHANNELLIST:
      GetChannelList();
      break;
   case Kartina::REQ_COOKIE:
      GetCookie();
      break;
   case Kartina::REQ_EPG:
      GetEPG(req.iOptArg1, req.iOptArg2);
      break;
   case Kartina::REQ_SERVER:
      SetServer(req.sOptArg1);
      break;
   case Kartina::REQ_HTTPBUFF:
      SetHttpBuffer(req.iOptArg1);
      break;
   case Kartina::REQ_STREAM:
      GetStreamURL(req.iOptArg1, req.sOptArg1);
      break;
   case Kartina::REQ_TIMERREC:
      GetStreamURL(req.iOptArg1, req.sOptArg1, true);
      break;
   case Kartina::REQ_ARCHIV:
      GetArchivURL(req.sOptArg1, req.sOptArg2);
      break;
   case Kartina::REQ_TIMESHIFT:
      SetTimeShift(req.iOptArg1);
      break;
   case Kartina::REQ_GETTIMESHIFT:
      GetTimeShift();
      break;
   case Kartina::REQ_GET_SERVER:
      GetServer();
      break;
   case Kartina::REQ_LOGOUT:
      Logout();
      break;
   case Kartina::REQ_GETVODGENRES:
      GetVodGenres();
      break;
   case Kartina::REQ_GETVIDEOS:
      GetVideos(req.sOptArg1);
      break;
   case Kartina::REQ_GETVIDEOINFO:
      GetVideoInfo(req.iOptArg1, req.sOptArg1);
      break;
   case Kartina::REQ_GETVODURL:
      GetVodUrl(req.iOptArg1, req.sOptArg1);
      break;
   case Kartina::REQ_GETBITRATE:
      GetBitRate();
      break;
   case Kartina::REQ_SETBITRATE:
      SetBitRate(req.iOptArg1);
      break;
   case Kartina::REQ_SETCHAN_HIDE:
      setChanHide(req.sOptArg1, req.sOptArg2);
      break;
   case Kartina::REQ_SETCHAN_SHOW:
      setChanShow(req.sOptArg1, req.sOptArg2);
      break;
   case Kartina::REQ_CHANLIST_ALL:
      GetChannelList(req.sOptArg1);
      break;
   case Kartina::REQ_GET_VOD_MANAGER:
      getVodManager(req.sOptArg1);
      break;
   case Kartina::REQ_SET_VOD_MANAGER:
      setVodManager(req.sOptArg1, req.sOptArg2);
      break;
   case Kartina::REQ_ADD_VOD_FAV:
      addVodFav(req.iOptArg1, req.sOptArg1);
      break;
   case Kartina::REQ_REM_VOD_FAV:
      remVodFav(req.iOptArg1, req.sOptArg1);
      break;
   case Kartina::REQ_GET_VOD_FAV:
      getVodFav();
      break;
   case Kartina::REQ_SET_PCODE:
      setParentCode(req.sOptArg1, req.sOptArg2);
      break;
   case Kartina::REQ_EPG_CURRENT:
      epgCurrent(req.sOptArg1);
      break;
   default:
      break;
   }
}

/////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------------------------\
| Function:    SetData
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: set communication parameter to communicate with
|              kartina.tv
| Parameters:  host, username, password
|
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetData(const QString &host, const QString &usr,
                           const QString &pw)
{
   sUsr           = usr;
   sPw            = pw;
   sApiUrl        = QString("http://%1%2").arg(host).arg(KARTINA_API_XML_PATH);
   sCookie        = "";
}

/*-----------------------------------------------------------------------------\
| Function:    SetCookie
|
| Author:      Jo2003
|
| Begin:       03.03.2010 / 12:18:00
|
| Description: set a former stored cookie
|
| Parameters:  ref. cookie string
|
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetCookie(const QString &cookie)
{
   mInfo(tr("We've got following Cookie: %1").arg(cookie));
   sCookie = cookie;
}

/*-----------------------------------------------------------------------------\
| Function:    Logout
|
| Author:      Jo2003
|
| Begin:       28.07.2010 / 12:18:00
|
| Description: logout from kartina
|
| Parameters:  --
|
\-----------------------------------------------------------------------------*/
void CKartinaClnt::Logout ()
{
   mInfo(tr("Logout ..."));
   get((int)Kartina::REQ_LOGOUT, sApiUrl + "logout");
}

/*-----------------------------------------------------------------------------\
| Function:    GetCookie
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: request authentication
|
| Parameters:  --
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetCookie ()
{
   mInfo(tr("Request Authentication ..."));

   post((int)Kartina::REQ_COOKIE, sApiUrl + "login",
        QString("login=%1&pass=%2&settings=all").arg(sUsr).arg(sPw));
}

/*-----------------------------------------------------------------------------\
| Function:    GetChannelList
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: request channel list from kartina.tv
|
| Parameters:  --
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetChannelList (const QString &secCode)
{
   mInfo(tr("Request Channel List ..."));
   QString req;

   if (secCode != "")
   {
      // normal channel list request ...
      req = QString("show=all&protect_code=%1").arg(secCode);
   }

   // request channel list or channel list for settings ...
   post((secCode == "") ? (int)Kartina::REQ_CHANNELLIST : (int)Kartina::REQ_CHANLIST_ALL,
        sApiUrl + "channel_list", req);
}

/*-----------------------------------------------------------------------------\
| Function:    GetServer
|
| Author:      Jo2003
|
| Begin:       17.02.2010, 18:30:52
|
| Description: request stream server list from kartina
|
| Parameters:  --
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetServer()
{
   mInfo(tr("Request Stream Server List ..."));

   get((int)Kartina::REQ_GET_SERVER, sApiUrl + "settings?var=stream_server");
}

/*-----------------------------------------------------------------------------\
| Function:    GetTimeShift
|
| Author:      Jo2003
|
| Begin:       29.07.2010, 13:50:52
|
| Description: request timeshift from kartina server
|
| Parameters:  --
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetTimeShift()
{
   mInfo(tr("Request Time Shift ..."));

   get((int)Kartina::REQ_GETTIMESHIFT, sApiUrl + "settings?var=timeshift");
}

/*-----------------------------------------------------------------------------\
| Function:    SetTimeShift
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: set timeshift
|
| Parameters:  int value in hours (allowed: 0, 1, 2, 3, 4, 8, 9, 10, 11)
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetTimeShift (int iHours)
{
   mInfo(tr("Set TimeShift to %1 hour(s) ...").arg(iHours));

   post((int)Kartina::REQ_TIMESHIFT, sApiUrl + "settings_set",
               QString("var=timeshift&val=%1").arg(iHours));
}

/*-----------------------------------------------------------------------------\
| Function:    GetBitRate
|
| Author:      Jo2003
|
| Begin:       14.01.2011, 13:50:52
|
| Description: request bitrate from kartina server
|
| Parameters:  --
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetBitRate()
{
   mInfo(tr("Request Bit Rate ..."));

   get((int)Kartina::REQ_GETBITRATE, sApiUrl + "settings?var=bitrate");
}

/*-----------------------------------------------------------------------------\
| Function:    SetBitRate
|
| Author:      Jo2003
|
| Begin:       14.01.2011 / 14:05
|
| Description: set bit rate
|
| Parameters:  int value kbit/sec. (allowed: 900, 1500)
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetBitRate(int iRate)
{
   mInfo(tr("Set BitRate to %1 kbit/s ...").arg(iRate));

   post((int)Kartina::REQ_SETBITRATE, sApiUrl + "settings_set",
               QString("var=bitrate&val=%1").arg(iRate));
}

/*-----------------------------------------------------------------------------\
| Function:    GetStreamURL
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: request stream URL for given channel id
|
| Parameters:  channel id, security code, [flag for timer record]
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetStreamURL(int iChanID, const QString &secCode, bool bTimerRec)
{
   mInfo(tr("Request URL for channel %1 ...").arg(iChanID));

   QString req = QString("cid=%1").arg(iChanID);

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   post((bTimerRec) ? (int)Kartina::REQ_TIMERREC : (int)Kartina::REQ_STREAM,
               sApiUrl + "get_url", req);
}

/*-----------------------------------------------------------------------------\
| Function:    SetServer
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: set streaming server
|
| Parameters:  server number (1 or 3)
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetServer (const QString &sIp)
{
   mInfo(tr("Set Streaming Server to %1 ...").arg(sIp));

   post((int)Kartina::REQ_SERVER, sApiUrl + "settings_set",
               QString("var=stream_server&val=%1").arg(sIp));
}

/*-----------------------------------------------------------------------------\
| Function:    SetHttpBuffer
|
| Author:      Jo2003
|
| Begin:       Thursday, January 21, 2010 11:49:52
|
| Description: set http buffer time
|
| Parameters:  time in msec. (1500, 3000, 5000, 8000, 15000)
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetHttpBuffer(int iTime)
{
   mInfo(tr("Set Http Buffer to %1 msec. ...").arg(iTime));

   post((int)Kartina::REQ_HTTPBUFF, sApiUrl + "settings_set",
               QString("var=http_caching&val=%1").arg(iTime));
}

/*-----------------------------------------------------------------------------\
| Function:    GetEPG
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: request todays EPG for given channel id
|
| Parameters:  channel index
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetEPG(int iChanID, int iOffset)
{
   mInfo(tr("Request EPG for Channel %1 ...").arg(iChanID));

   QDate now = QDate::currentDate().addDays(iOffset);

   get((int)Kartina::REQ_EPG, sApiUrl + QString("epg?cid=%1&day=%2")
       .arg(iChanID).arg(now.toString("ddMMyy")));
}

/*-----------------------------------------------------------------------------\
| Function:    GetArchivURL
|
| Author:      Jo2003
|
| Begin:       Monday, January 18, 2010 14:57:52
|
| Description: request archiv URL for prepared request
|
| Parameters:  prepared request
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetArchivURL (const QString &prepared, const QString &secCode)
{
   mInfo(tr("Request Archiv URL ..."));

   QString req = prepared;

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   post((int)Kartina::REQ_ARCHIV, sApiUrl + "get_url", req);
}

/*-----------------------------------------------------------------------------\
| Function:    GetVodGenres
|
| Author:      Jo2003
|
| Begin:       09.12.2010 / 13:18
|
| Description: request VOD genres (for now answer will be html code)
|
| Parameters:  --
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetVodGenres()
{
   mInfo(tr("Request VOD Genres ..."));

   get((int)Kartina::REQ_GETVODGENRES, sApiUrl + "vod_genres");
}

/*-----------------------------------------------------------------------------\
| Function:    GetVideos
|
| Author:      Jo2003
|
| Begin:       09.12.2010 / 13:18
|
| Description: get vidoes matching to prepared search string
|
| Parameters:  prepared search string
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetVideos(const QString &sPrepared)
{
   mInfo(tr("Request Videos ..."));

   get((int)Kartina::REQ_GETVIDEOS, sApiUrl + QString("vod_list?%1").arg(sPrepared));
}

/*-----------------------------------------------------------------------------\
| Function:    GetVideoInfo
|
| Author:      Jo2003
|
| Begin:       21.12.2010 / 16:18
|
| Description: get video info for video id (VOD)
|
| Parameters:  video id
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetVideoInfo(int iVodID, const QString &secCode)
{
   mInfo(tr("Request Video info for video %1...").arg(iVodID));

   QString req = QString("vod_info?id=%1").arg(iVodID);

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   get((int)Kartina::REQ_GETVIDEOINFO, sApiUrl + req);
}

/*-----------------------------------------------------------------------------\
| Function:    GetVodUrl
|
| Author:      Jo2003
|
| Begin:       22.12.2010 / 16:18
|
| Description: get video url for video id (VOD)
|
| Parameters:  video id
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetVodUrl(int iVidId, const QString &secCode)
{
   mInfo(tr("Request Video Url for video %1...").arg(iVidId));

   QString req = QString("vod_geturl?fileid=%1&ad=1")
         .arg(iVidId);

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   get((int)Kartina::REQ_GETVODURL, sApiUrl + req);
}

/*-----------------------------------------------------------------------------\
| Function:    setChanHide
|
| Author:      Jo2003
|
| Begin:       14.05.20012
|
| Description: hide channel from channel list
|
| Parameters:  channel id(s), security code
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::setChanHide(const QString &cids, const QString &secCode)
{
   mInfo(tr("Hide channel(s) %1 from channel list ...").arg(cids));

   QString req = QString("cmd=hide_channel&cid=%1&protect_code=%2")
           .arg(cids).arg(secCode);

   post((int)Kartina::REQ_SETCHAN_HIDE, sApiUrl + "rule", req);
}

/*-----------------------------------------------------------------------------\
| Function:    setChanShow
|
| Author:      Jo2003
|
| Begin:       14.05.20012
|
| Description: show channel in channel list
|
| Parameters:  channel id(s), security code
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::setChanShow(const QString &cids, const QString &secCode)
{
   mInfo(tr("Show channel(s) %1 in channel list ...").arg(cids));

   QString req = QString("cmd=show_channel&cid=%1&protect_code=%2")
           .arg(cids).arg(secCode);

   post((int)Kartina::REQ_SETCHAN_SHOW, sApiUrl + "rule", req);
}

/*-----------------------------------------------------------------------------\
| Function:    getVodManager
|
| Author:      Jo2003
|
| Begin:       23.05.2012
|
| Description: request VOD manager data
|
| Parameters:  security code
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::getVodManager(const QString &secCode)
{
   mInfo(tr("Request VOD manager data ..."));

   QString req = QString("cmd=get_user_rates&protect_code=%1")
           .arg(secCode);

   post((int)Kartina::REQ_GET_VOD_MANAGER, sApiUrl + "vod_manage", req);
}

/*-----------------------------------------------------------------------------\
| Function:    setVodManager
|
| Author:      Jo2003
|
| Begin:       23.05.2012
|
| Description: set VOD manager data
|
| Parameters:  rules in form: "violence=hide&porn=pass", security code
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::setVodManager(const QString &rules, const QString &secCode)
{
   mInfo(tr("Set VOD manager data (%1) ...").arg(rules));

   QString req = QString("cmd=set_user_rates%1&protect_code=%2")
           .arg(rules).arg(secCode);

   post((int)Kartina::REQ_SET_VOD_MANAGER, sApiUrl + "vod_manage", req);
}

/*-----------------------------------------------------------------------------\
| Function:    addVodFav
|
| Author:      Jo2003
|
| Begin:       29.05.2012
|
| Description: add one video to favourites
|
| Parameters:  video id
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::addVodFav(int iVidID, const QString &secCode)
{
   mInfo(tr("Add VOD favourite (%1) ...").arg(iVidID));

   QString req = QString("id=%1").arg(iVidID);

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   post((int)Kartina::REQ_ADD_VOD_FAV, sApiUrl + "vod_favadd", req);
}

/*-----------------------------------------------------------------------------\
| Function:    remVodFav
|
| Author:      Jo2003
|
| Begin:       29.05.2012
|
| Description: remove one video from favourites
|
| Parameters:  video id
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::remVodFav(int iVidID, const QString &secCode)
{
   mInfo(tr("Remove VOD favourite (%1) ...").arg(iVidID));

   QString req = QString("id=%1").arg(iVidID);

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   post((int)Kartina::REQ_REM_VOD_FAV, sApiUrl + "vod_favsub", req);
}

/*-----------------------------------------------------------------------------\
| Function:    getVodFav
|
| Author:      Jo2003
|
| Begin:       29.05.2012
|
| Description: request vod favourites
|
| Parameters:  --
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::getVodFav()
{
   mInfo(tr("Get VOD favourites (%1) ..."));
   get((int)Kartina::REQ_GET_VOD_FAV, sApiUrl + "vod_favlist");
}

/*-----------------------------------------------------------------------------\
| Function:    setParentCode
|
| Author:      Jo2003
|
| Begin:       31.05.2012
|
| Description: set new parent code
|
| Parameters:  old code, new code
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::setParentCode(const QString &oldCode, const QString &newCode)
{
   mInfo(tr("Change parent code ..."));

   QString req = QString("var=pcode&old_code=%1&new_code=%2&confirm_code=%2")
         .arg(oldCode).arg(newCode);

   post((int)Kartina::REQ_SET_PCODE, sApiUrl + "settings_set", req);
}

/*-----------------------------------------------------------------------------\
| Function:    epgCurrent
|
| Author:      Jo2003
|
| Begin:       06.12.2012
|
| Description: request current epg for given channels
|
| Parameters:  comma separated list of channel ids
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::epgCurrent(const QString &cids)
{
   mInfo(tr("EPG current for Channels: %1 ...").arg(cids));

   get((int)Kartina::REQ_EPG_CURRENT, sApiUrl + QString("epg_current?cids=%1&epg=3")
       .arg(cids));
}

/* -----------------------------------------------------------------\
|  Method: cookieSet
|  Begin: 24.01.2011 / 10:40
|  Author: Jo2003
|  Description: is cookie set?
|
|  Parameters: --
|
|  Returns: true ==> set
|          false ==> not set
\----------------------------------------------------------------- */
bool CKartinaClnt::cookieSet()
{
   return (sCookie != "") ? true : false;
}

/* -----------------------------------------------------------------\
|  Method: checkResponse
|  Begin: 28.07.2010 / 18:42:54
|  Author: Jo2003
|  Description: format kartina error string
|
|  Parameters: sResp -> string to heck
|              sCleanReasp -> cleaned response
|
|  Returns: error code
\----------------------------------------------------------------- */
int CKartinaClnt::checkResponse (const QString &sResp, QString &sCleanResp)
{
   int iRV = 0;

   // clean response ... (delete content which may come
   // after / before the xml code ...
   QString sEndTag = "</response>";
   int iStartPos   = sResp.indexOf("<?xml");
   int iEndPos     = sResp.indexOf(sEndTag) + sEndTag.length();

   // store clean string in private variable ...
   sCleanResp      = sResp.mid(iStartPos, iEndPos - iStartPos);

   QRegExp rx("<message>(.*)</message>[ \t\n\r]*"
              "<code>(.*)</code>");

   // quick'n'dirty error check ...
   if (sCleanResp.contains("<error>"))
   {
      if (rx.indexIn(sCleanResp) > -1)
      {
         iRV = rx.cap(2).toInt();

         sCleanResp = errMap.contains((Kartina::EErr)iRV) ? errMap[(Kartina::EErr)iRV] : rx.cap(1);
      }
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: fillErrorMap
|  Begin: 21.07.2011 / 12:30
|  Author: Jo2003
|  Description: fill error translation map
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaClnt::fillErrorMap()
{
   errMap.clear();
   errMap.insert(Kartina::ERR_UNKNOWN,                 tr("Unknown error"));
   errMap.insert(Kartina::ERR_INCORRECT_REQUEST,       tr("Incorrect request"));
   errMap.insert(Kartina::ERR_WRONG_LOGIN_DATA,        tr("Wrong login or password"));
   errMap.insert(Kartina::ERR_ACCESS_DENIED,           tr("Access denied"));
   errMap.insert(Kartina::ERR_LOGIN_INCORRECT,         tr("Login incorrect"));
   errMap.insert(Kartina::ERR_CONTRACT_INACTIVE,       tr("Your contract is inactive"));
   errMap.insert(Kartina::ERR_CONTRACT_PAUSED,         tr("Your contract is paused"));
   errMap.insert(Kartina::ERR_CHANNEL_NOT_FOUND,       tr("Channel not found or not allowed"));
   errMap.insert(Kartina::ERR_BAD_PARAM,               tr("Error in request: Bad parameters"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_DAY,       tr("Missing parameter (day) in format <DDMMYY>"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_CID,       tr("Missing parameter (cid)"));
   errMap.insert(Kartina::ERR_MULTIPLE_ACCOUNT_USE,    tr("Another client with your data logged in"));
   errMap.insert(Kartina::ERR_AUTHENTICATION,          tr("Authentication error"));
   errMap.insert(Kartina::ERR_PACKAGE_EXPIRED,         tr("Your package expired"));
   errMap.insert(Kartina::ERR_UNKNOWN_API_FUNCTION,    tr("Unknown API function"));
   errMap.insert(Kartina::ERR_ARCHIVE_NOT_AVAIL,       tr("Archive not available"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_PLACE,     tr("Missing parameter (place)"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_NAME,      tr("Missing parameter (name)"));
   errMap.insert(Kartina::ERR_CONFIRMATION_CODE,       tr("Incorrect confirmation code"));
   errMap.insert(Kartina::ERR_WRONG_PCODE,             tr("Current code is wrong"));
   errMap.insert(Kartina::ERR_NEW_CODE,                tr("New code is wrong"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_VAL,       tr("Missing parameter (val)"));
   errMap.insert(Kartina::ERR_VALUE_NOT_ALLOWED,       tr("Value not allowed"));
   errMap.insert(Kartina::ERR_MISSING_PARAM,           tr("Missing parameter"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_ID,        tr("Missing parameter (id)"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_FILEID,    tr("Missing parameter (fileid)"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_TYPE,      tr("Missing parameter (type)"));
   errMap.insert(Kartina::ERR_MISSING_PARAM_QUERY,     tr("Missing parameter (query)"));
   errMap.insert(Kartina::ERR_BITRATE_NOT_AVAIL,       tr("Bitrate not available"));
   errMap.insert(Kartina::ERR_SERVICE_NOT_AVAIL,       tr("Service not available"));
   errMap.insert(Kartina::ERR_QUERY_LIMIT_EXCEEDED,    tr("Query limit exceeded"));
   errMap.insert(Kartina::ERR_RULE_ALREADY_EXISTS,     tr("Rule already exists"));
   errMap.insert(Kartina::ERR_RULE_NEED_CMD,           tr("Missing parameter (cmd)"));
   errMap.insert(Kartina::ERR_MANAGE_NEED_CMD,         tr("Missing parameter (cmd)"));
   errMap.insert(Kartina::ERR_MANAGE_BAD_VALUE,        tr("Bad value (rate)"));
   errMap.insert(Kartina::ERR_MANAGE_FILM_NOT_FOUND,   tr("Can't find film"));
   errMap.insert(Kartina::ERR_MANAGE_ALREADY_ADDED,    tr("Film already added"));
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/
