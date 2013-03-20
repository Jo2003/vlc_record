/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: 19.03.2013
|
| $Id$
|
\=============================================================================*/
#include "crodnoeclient.h"
#include "small_helpers.h"

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

   connect(this, SIGNAL(sigStringResponse(int,QString)), this, SLOT(slotStringResponse(int,QString)));
   connect(this, SIGNAL(sigBinResponse(int,QByteArray)), this, SLOT(slotBinResponse(int,QByteArray)));
   connect(this, SIGNAL(sigErr(int,QString,int)), this, SLOT(slotErr(int,QString,int)));
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

#ifdef __TRACE
   mInfo(tr("Response for request '%1':\n ==8<==8<==8<==\n%2\n ==>8==>8==>8==")
         .arg(karTrace.reqValToKey((Kartina::EReq)reqId))
         .arg(strResp));
#endif // __TRACE

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
   switch((Kartina::EReq)reqId)
   {
   case Kartina::REQ_DOWN_IMG:
      emit sigImage(binResp);
      break;

   default:
      break;
   }
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
void CKartinaClnt::slotErr (int iReqId, QString sErr, int iErr)
{
   emit sigError(sErr, iReqId, iErr);
}

/////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------\
|  Method: queueRequest
|  Begin: 17.03.2013
|  Author: Jo2003
|  Description: queue in new request, check for cookie and abort
|
|  Parameters: req id, param1, param2
|
|  Returns: 0 -> ok, -1 -> unknown request
\----------------------------------------------------------------- */
int CKartinaClnt::queueRequest(Kartina::EReq req, const QVariant& par_1, const QVariant& par_2)
{
   int iRet = 0;

   // handle special cases ...
   if (sCookie == "")
   {
      // no ccokie set, only some requests allowed ...
      switch (req)
      {
      case Kartina::REQ_COOKIE:
      case Kartina::REQ_LOGOUT:
      case Kartina::REQ_UPDATE_CHECK:
         break;
      default:
         iRet = -1;
         break;
      }
   }

   if (iRet > -1)
   {
      // handle request ...
      switch (req)
      {
      case Kartina::REQ_CHANNELLIST:
         GetChannelList();
         break;
      case Kartina::REQ_COOKIE:
         GetCookie();
         break;
      case Kartina::REQ_EPG:
         GetEPG(par_1.toInt(), par_2.toInt());
         break;
      case Kartina::REQ_SERVER:
         SetServer(par_1.toString());
         break;
      case Kartina::REQ_HTTPBUFF:
         SetHttpBuffer(par_1.toInt());
         break;
      case Kartina::REQ_STREAM:
         GetStreamURL(par_1.toInt(), par_2.toString());
         break;
      case Kartina::REQ_TIMERREC:
         GetStreamURL(par_1.toInt(), par_2.toString(), true);
         break;
      case Kartina::REQ_ARCHIV:
         GetArchivURL(par_1.toString(), par_2.toString());
         break;
      case Kartina::REQ_TIMESHIFT:
         SetTimeShift(par_1.toInt());
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
         GetVideos(par_1.toString());
         break;
      case Kartina::REQ_GETVIDEOINFO:
         GetVideoInfo(par_1.toInt(), par_2.toString());
         break;
      case Kartina::REQ_GETVODURL:
         GetVodUrl(par_1.toInt(), par_2.toString());
         break;
      case Kartina::REQ_GETBITRATE:
         GetBitRate();
         break;
      case Kartina::REQ_SETBITRATE:
         SetBitRate(par_1.toInt());
         break;
      case Kartina::REQ_SETCHAN_HIDE:
         setChanHide(par_1.toString(), par_2.toString());
         break;
      case Kartina::REQ_SETCHAN_SHOW:
         setChanShow(par_1.toString(), par_2.toString());
         break;
      case Kartina::REQ_CHANLIST_ALL:
         GetChannelList(par_1.toString());
         break;
      case Kartina::REQ_GET_VOD_MANAGER:
         getVodManager(par_1.toString());
         break;
      case Kartina::REQ_SET_VOD_MANAGER:
         setVodManager(par_1.toString(), par_2.toString());
         break;
      case Kartina::REQ_ADD_VOD_FAV:
         addVodFav(par_1.toInt(), par_2.toString());
         break;
      case Kartina::REQ_REM_VOD_FAV:
         remVodFav(par_1.toInt(), par_2.toString());
         break;
      case Kartina::REQ_GET_VOD_FAV:
         getVodFav();
         break;
      case Kartina::REQ_SET_PCODE:
         setParentCode(par_1.toString(), par_2.toString());
         break;
      case Kartina::REQ_EPG_CURRENT:
         epgCurrent(par_1.toString());
         break;
      case Kartina::REQ_UPDATE_CHECK:
         updInfo(par_1.toString());
         break;
      default:
         iRet = -1;
         break;
      }
   }

   return iRet;
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
   get((int)Kartina::REQ_LOGOUT, sApiUrl + "logout", Iptv::Logout);
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
        QString("login=%1&pass=%2&with_acc=1&with_cfg=1")
        .arg(sUsr)
        .arg(CSmallHelpers::md5(CSmallHelpers::md5(sUsr) + CSmallHelpers::md5(sPw))),
        Iptv::Login);
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
   QString req = "with_epg=1";

   if (secCode != "")
   {
      // normal channel list request ...
      req = QString("&show=all&protect_code=%1").arg(secCode);
   }

   // request channel list or channel list for settings ...
   post((secCode == "") ? (int)Kartina::REQ_CHANNELLIST : (int)Kartina::REQ_CHANLIST_ALL,
        sApiUrl + "get_list_tv", req);
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

   post((int)Kartina::REQ_TIMESHIFT, sApiUrl + "set",
               QString("var=time_shift&val=%1").arg(iHours));
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
               sApiUrl + "get_url_tv", req);
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

   post((int)Kartina::REQ_SERVER, sApiUrl + "set",
               QString("var=media_server_id&val=%1").arg(sIp));
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

   QDateTime dt(QDate::currentDate().addDays(iOffset));

   post((int)Kartina::REQ_EPG, sApiUrl + "get_epg",
        QString("cid=%1&from_uts=%2&hours=24").arg(iChanID).arg(dt.toTime_t()));
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

   // adapt rodnoe ...
   req.replace("&gmt=", "&uts=");

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   post((int)Kartina::REQ_ARCHIV, sApiUrl + "get_url_tv", req);
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

   QString req = QString("var=parental_pass&val=%1&protect_code=%2")
         .arg(newCode).arg(oldCode);

   post((int)Kartina::REQ_SET_PCODE, sApiUrl + "set", req);
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

   post((int)Kartina::REQ_EPG_CURRENT, sApiUrl + "get_epg_current", QString("cid=%1").arg(cids));
}

//---------------------------------------------------------------------------
//
//! \brief   check for program updates
//
//! \author  Jo2003
//! \date    17.03.2013
//
//! \param   url (QString) url with update information
//
//! \return  --
//---------------------------------------------------------------------------
void CKartinaClnt::updInfo (const QString& url)
{
   mInfo(tr("Check for available updates ..."));

   get((int)Kartina::REQ_UPDATE_CHECK, url);
}

//---------------------------------------------------------------------------
//
//! \brief   download image given by url
//
//! \author  Jo2003
//! \date    17.03.2013
//
//! \param   url (QString) url image data
//
//! \return  --
//---------------------------------------------------------------------------
void CKartinaClnt::slotDownImg(const QString &url)
{
   mInfo(tr("Download image ..."));

   get((int)Kartina::REQ_DOWN_IMG, url, Iptv::Binary);
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
   int iStartPos   = sResp.indexOf("<?xml");       // xml start tag
   int iEndPos     = sResp.lastIndexOf('>') + 1;   // end of last tag

   // store clean string in private variable ...
   sCleanResp      = sResp.mid(iStartPos, iEndPos - iStartPos);

   // quick'n'dirty error check ...
   if (sCleanResp.contains("<error>"))
   {
      iRV = -2;

      QString sCode, sMsg;
      QRegExp rx("<message>(.*)</message>");

      if (rx.indexIn(sCleanResp) > -1)
      {
         sMsg = rx.cap(1);
      }

      rx.setPattern("<code>(.*)</code>");

      if (rx.indexIn(sCleanResp) > -1)
      {
         sCode = rx.cap(1);
      }

      sCleanResp = QString("%1: %2").arg(sCode).arg(sMsg);
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
