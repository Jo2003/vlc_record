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
#include "qcustparser.h"
#include "externals_inc.h"

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
   connect(this, SIGNAL(sigApiErr(int,QString,int)), this, SLOT(slotErr(int,QString,int)));

   setObjectName("CKartinaClnt");
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
         .arg(karTrace.reqValToKey((CIptvDefs::EReq)reqId))
         .arg(strResp));
#endif // __TRACE

   if (reqId == (int)CIptvDefs::REQ_LOGOUT)
   {
       sCookie = "";

       // send response ...
       emit sigHttpResponse ("", reqId);
   }
   else
   {
      mInfo(tr("Request '%2' done!").arg(karTrace.reqValToKey((CIptvDefs::EReq)reqId)));

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
   switch((CIptvDefs::EReq)reqId)
   {
   case CIptvDefs::REQ_DOWN_IMG:
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
int CKartinaClnt::queueRequest(CIptvDefs::EReq req, const QVariant& par_1, const QVariant& par_2)
{
   int iRet = 0;

   // handle special cases ...
   if (sCookie == "")
   {
      // no ccokie set, only some requests allowed ...
      switch (req)
      {
      case CIptvDefs::REQ_COOKIE:
      case CIptvDefs::REQ_LOGOUT:
      case CIptvDefs::REQ_UPDATE_CHECK:
      case CIptvDefs::REQ_STATS_SERVICE:
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
      case CIptvDefs::REQ_CHANNELLIST:
         GetChannelList();
         break;
      case CIptvDefs::REQ_COOKIE:
         GetCookie();
         break;
      case CIptvDefs::REQ_EPG:
         GetEPG(par_1.toInt(), par_2.toInt());
         break;
      case CIptvDefs::REQ_EPG_EXT:
         GetEPG(par_1.toInt(), par_2.toInt(), true);
         break;
      case CIptvDefs::REQ_SERVER:
         SetServer(par_1.toString());
         break;
      case CIptvDefs::REQ_HTTPBUFF:
         SetHttpBuffer(par_1.toInt());
         break;
      case CIptvDefs::REQ_STREAM:
      case CIptvDefs::REQ_RADIO_STREAM:
         GetStreamURL(par_1.toInt(), par_2.toString());
         break;
      case CIptvDefs::REQ_TIMERREC:
      case CIptvDefs::REQ_RADIO_TIMERREC:
         GetStreamURL(par_1.toInt(), par_2.toString(), true);
         break;
      case CIptvDefs::REQ_ARCHIV:
         GetArchivURL(par_1.toString(), par_2.toString());
         break;
      case CIptvDefs::REQ_TIMESHIFT:
         SetTimeShift(par_1.toInt());
         break;
      case CIptvDefs::REQ_GETTIMESHIFT:
         GetTimeShift();
         break;
      case CIptvDefs::REQ_GET_SERVER:
         GetServer();
         break;
      case CIptvDefs::REQ_LOGOUT:
         Logout();
         break;
      case CIptvDefs::REQ_GETVODGENRES:
         GetVodGenres();
         break;
      case CIptvDefs::REQ_GETVIDEOS:
         GetVideos(par_1.toString());
         break;
      case CIptvDefs::REQ_GETVIDEOINFO:
         GetVideoInfo(par_1.toInt(), par_2.toString());
         break;
      case CIptvDefs::REQ_GETVODURL:
         GetVodUrl(par_1.toInt(), par_2.toString());
         break;
      case CIptvDefs::REQ_GETBITRATE:
         GetBitRate();
         break;
      case CIptvDefs::REQ_SETBITRATE:
         SetBitRate(par_1.toInt());
         break;
      case CIptvDefs::REQ_SETCHAN_HIDE:
         setChanHide(par_1.toString(), par_2.toString());
         break;
      case CIptvDefs::REQ_SETCHAN_SHOW:
         setChanShow(par_1.toString(), par_2.toString());
         break;
      case CIptvDefs::REQ_CHANLIST_ALL:
         GetChannelList(par_1.toString());
         break;
      case CIptvDefs::REQ_GET_VOD_MANAGER:
         getVodManager(par_1.toString());
         break;
      case CIptvDefs::REQ_SET_VOD_MANAGER:
         setVodManager(par_1.toString(), par_2.toString());
         break;
      case CIptvDefs::REQ_ADD_VOD_FAV:
         addVodFav(par_1.toInt(), par_2.toString());
         break;
      case CIptvDefs::REQ_REM_VOD_FAV:
         remVodFav(par_1.toInt(), par_2.toString());
         break;
      case CIptvDefs::REQ_GET_VOD_FAV:
         getVodFav();
         break;
      case CIptvDefs::REQ_SET_PCODE:
         setParentCode(par_1.toString(), par_2.toString());
         break;
      case CIptvDefs::REQ_EPG_CURRENT:
         epgCurrent(par_1.toString());
         break;
      case CIptvDefs::REQ_UPDATE_CHECK:
         updInfo(par_1.toString());
         break;
      case CIptvDefs::REQ_VOD_LANG:
         getVodLang();
         break;
      case CIptvDefs::REQ_STATS_SERVICE:
         statsService(par_1.toString());
         break;
      case CIptvDefs::REQ_STATS_ONLY:
         statsOnly(par_1.toString());
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
                           const QString &pw, const QString &lang)
{
   Q_UNUSED(lang)
   sUsr           = usr;
   sPw            = pw;
#ifdef _USE_QJSON
   sApiUrl        = QString("http://%1%2").arg(host).arg(pCustomization->strVal("API_JSON_PATH"));
#else
   sApiUrl        = QString("http://%1%2").arg(host).arg(pCustomization->strVal("API_XML_PATH"));
#endif // _USE_QJSON
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
   q_get((int)CIptvDefs::REQ_LOGOUT, sApiUrl + "logout", Iptv::Logout);
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

   q_post((int)CIptvDefs::REQ_COOKIE, sApiUrl + "login",
        QString("login=%1&pass=%2&settings=all&softid=%3%4-%5")
            .arg(sUsr).arg(sPw)
            .arg(pCustomization->strVal("APPLICATION_SHORTCUT"))
            .arg(OP_SYS).arg(SOFTID_DEVELOPER),
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
   QString req;

   if (secCode != "")
   {
      // normal channel list request ...
      req = QString("show=all&protect_code=%1").arg(secCode);
   }
#ifdef _TASTE_CHITRAM_TV
   else
   {
      req = "icon=1";
   }
#endif // _TASTE_CHITRAM_TV

   // request channel list or channel list for settings ...
   q_post((secCode == "") ? (int)CIptvDefs::REQ_CHANNELLIST : (int)CIptvDefs::REQ_CHANLIST_ALL,
        sApiUrl + "channel_list", req);
}

//---------------------------------------------------------------------------
//
//! \brief   get available VOD languages
//
//! \author  Jo2003
//! \date    28.01.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void CKartinaClnt::getVodLang()
{
   mInfo(tr("Request available VOD languages ..."));

   q_get((int)CIptvDefs::REQ_VOD_LANG, sApiUrl + "getinfo?info=langs");
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

   q_get((int)CIptvDefs::REQ_GET_SERVER, sApiUrl + "settings?var=stream_server");
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

   q_get((int)CIptvDefs::REQ_GETTIMESHIFT, sApiUrl + "settings?var=timeshift");
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

   q_post((int)CIptvDefs::REQ_TIMESHIFT, sApiUrl + "settings_set",
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

   q_get((int)CIptvDefs::REQ_GETBITRATE, sApiUrl + "settings?var=bitrate");
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

   q_post((int)CIptvDefs::REQ_SETBITRATE, sApiUrl + "settings_set",
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

   q_post((bTimerRec) ? (int)CIptvDefs::REQ_TIMERREC : (int)CIptvDefs::REQ_STREAM,
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

   q_post((int)CIptvDefs::REQ_SERVER, sApiUrl + "settings_set",
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

   q_post((int)CIptvDefs::REQ_HTTPBUFF, sApiUrl + "settings_set",
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
void CKartinaClnt::GetEPG(int iChanID, int iOffset, bool bExtEPG)
{
   mInfo(tr("Request EPG for Channel %1 ...").arg(iChanID));

   QDate now = QDate::currentDate().addDays(iOffset);

   q_get((int)(bExtEPG ? CIptvDefs::REQ_EPG_EXT : CIptvDefs::REQ_EPG), sApiUrl + QString("epg?cid=%1&day=%2")
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

   QString req = QUrl::fromPercentEncoding(prepared.toUtf8());

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_post((int)CIptvDefs::REQ_ARCHIV, sApiUrl + "get_url", req);
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

   q_get((int)CIptvDefs::REQ_GETVODGENRES, sApiUrl + "vod_genres");
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

   q_get((int)CIptvDefs::REQ_GETVIDEOS, sApiUrl + "vod_list?" + QUrl::fromPercentEncoding(sPrepared.toUtf8()));
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

   q_get((int)CIptvDefs::REQ_GETVIDEOINFO, sApiUrl + req);
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

   q_get((int)CIptvDefs::REQ_GETVODURL, sApiUrl + req);
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

   q_post((int)CIptvDefs::REQ_SETCHAN_HIDE, sApiUrl + "rule", req);
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

   q_post((int)CIptvDefs::REQ_SETCHAN_SHOW, sApiUrl + "rule", req);
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

   q_post((int)CIptvDefs::REQ_GET_VOD_MANAGER, sApiUrl + "vod_manage", req);
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

   q_post((int)CIptvDefs::REQ_SET_VOD_MANAGER, sApiUrl + "vod_manage", req);
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

   q_post((int)CIptvDefs::REQ_ADD_VOD_FAV, sApiUrl + "vod_favadd", req);
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

   q_post((int)CIptvDefs::REQ_REM_VOD_FAV, sApiUrl + "vod_favsub", req);
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
   q_get((int)CIptvDefs::REQ_GET_VOD_FAV, sApiUrl + "vod_favlist");
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

   q_post((int)CIptvDefs::REQ_SET_PCODE, sApiUrl + "settings_set", req);
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

   q_get((int)CIptvDefs::REQ_EPG_CURRENT, sApiUrl + QString("epg_current?cids=%1&epg=3")
       .arg(cids));
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

   q_get((int)CIptvDefs::REQ_UPDATE_CHECK, url);
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

   q_get((int)CIptvDefs::REQ_DOWN_IMG, url, Iptv::Binary);
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
#ifdef _USE_QJSON
   sCleanResp = sResp;

   if (sCleanResp.contains("\"error\""))
   {
      iRV = -1;
   }
#else
   // clean response ... (delete content which may come
   // after / before the xml code ...
   int iStartPos   = sResp.indexOf("<?xml");       // xml start tag
   int iEndPos     = sResp.lastIndexOf('>') + 1;   // end of last tag

   // store clean string in private variable ...
   sCleanResp      = sResp.mid(iStartPos, iEndPos - iStartPos);

   // quick'n'dirty error check ...
   if (sCleanResp.contains("<error>"))
   {
      QString msg;
      QRegExp rx;

      // for sure we have an error here ...
      iRV = -1;

      rx.setPattern("<code>(.*)</code>");

      if (rx.indexIn(sCleanResp) > -1)
      {
         iRV = rx.cap(1).toInt();
      }

      rx.setPattern("<message>(.*)</message>");

      if (rx.indexIn(sCleanResp) > -1)
      {
         msg = rx.cap(1);
      }

      sCleanResp = errMap.contains((CIptvDefs::EErr)iRV) ? errMap[(CIptvDefs::EErr)iRV] : msg;
   }
#endif // _USE_QJSON
   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: apiUrl
|  Begin: 12.12.2013
|  Author: Jo2003
|  Description: get API url
|
|  Parameters: --
|
|  Returns: api url
\----------------------------------------------------------------- */
const QString& CKartinaClnt::apiUrl()
{
   return sApiUrl;
}

//---------------------------------------------------------------------------
//
//! \brief   send / request statistics + service
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   stats [in] (const QString &) statistics as JSON string
//
//---------------------------------------------------------------------------
void CKartinaClnt::statsService(const QString &stats)
{
   QRegExp rx("^[^.]*\\.(.*)$");
   QString sHost = "http://service.polsky.tv/api/json/stats";

   if (rx.indexIn(QUrl(sApiUrl).host()) > -1)
   {
      sHost = QString("http://service.%1%2stats").arg(rx.cap(1)).arg(pCustomization->strVal("API_JSON_PATH"));
   }

   q_post(CIptvDefs::REQ_STATS_SERVICE,
          sHost,
          stats, Iptv::String, true);
}

//---------------------------------------------------------------------------
//
//! \brief   send / request statistics + service
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   stats [in] (const QString &) statistics as JSON string
//
//---------------------------------------------------------------------------
void CKartinaClnt::statsOnly(const QString &stats)
{
   QRegExp rx("^[^.]*\\.(.*)$");
   QString sHost = "http://service.polsky.tv/api/json/stats";

   if (rx.indexIn(QUrl(sApiUrl).host()) > -1)
   {
      sHost = QString("http://service.%1%2stats").arg(rx.cap(1)).arg(pCustomization->strVal("API_JSON_PATH"));
   }

   q_post(CIptvDefs::REQ_STATS_ONLY,
          sHost,
          stats, Iptv::String, true);
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
   errMap.insert(CIptvDefs::ERR_UNKNOWN,                 tr("Unknown error"));
   errMap.insert(CIptvDefs::ERR_INCORRECT_REQUEST,       tr("Incorrect request"));
   errMap.insert(CIptvDefs::ERR_WRONG_LOGIN_DATA,        tr("Wrong login or password"));
   errMap.insert(CIptvDefs::ERR_ACCESS_DENIED,           tr("Access denied"));
   errMap.insert(CIptvDefs::ERR_LOGIN_INCORRECT,         tr("Login incorrect"));
   errMap.insert(CIptvDefs::ERR_CONTRACT_INACTIVE,       tr("Your contract is inactive"));
   errMap.insert(CIptvDefs::ERR_CONTRACT_PAUSED,         tr("Your contract is paused"));
   errMap.insert(CIptvDefs::ERR_CHANNEL_NOT_FOUND,       tr("Channel not found or not allowed"));
   errMap.insert(CIptvDefs::ERR_BAD_PARAM,               tr("Error in request: Bad parameters"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_DAY,       tr("Missing parameter (day) in format <DDMMYY>"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_CID,       tr("Missing parameter (cid)"));
   errMap.insert(CIptvDefs::ERR_MULTIPLE_ACCOUNT_USE,    tr("Another client with your data logged in"));
   errMap.insert(CIptvDefs::ERR_AUTHENTICATION,          tr("Authentication error"));
   errMap.insert(CIptvDefs::ERR_PACKAGE_EXPIRED,         tr("Your package expired"));
   errMap.insert(CIptvDefs::ERR_UNKNOWN_API_FUNCTION,    tr("Unknown API function"));
   errMap.insert(CIptvDefs::ERR_ARCHIVE_NOT_AVAIL,       tr("Archive not available"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_PLACE,     tr("Missing parameter (place)"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_NAME,      tr("Missing parameter (name)"));
   errMap.insert(CIptvDefs::ERR_CONFIRMATION_CODE,       tr("Incorrect confirmation code"));
   errMap.insert(CIptvDefs::ERR_WRONG_PCODE,             tr("Current code is wrong"));
   errMap.insert(CIptvDefs::ERR_NEW_CODE,                tr("New code is wrong"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_VAL,       tr("Missing parameter (val)"));
   errMap.insert(CIptvDefs::ERR_VALUE_NOT_ALLOWED,       tr("Value not allowed"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM,           tr("Missing parameter"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_ID,        tr("Missing parameter (id)"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_FILEID,    tr("Missing parameter (fileid)"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_TYPE,      tr("Missing parameter (type)"));
   errMap.insert(CIptvDefs::ERR_MISSING_PARAM_QUERY,     tr("Missing parameter (query)"));
   errMap.insert(CIptvDefs::ERR_BITRATE_NOT_AVAIL,       tr("Bitrate not available"));
   errMap.insert(CIptvDefs::ERR_SERVICE_NOT_AVAIL,       tr("Service not available"));
   errMap.insert(CIptvDefs::ERR_QUERY_LIMIT_EXCEEDED,    tr("Query limit exceeded"));
   errMap.insert(CIptvDefs::ERR_RULE_ALREADY_EXISTS,     tr("Rule already exists"));
   errMap.insert(CIptvDefs::ERR_RULE_NEED_CMD,           tr("Missing parameter (cmd)"));
   errMap.insert(CIptvDefs::ERR_MANAGE_NEED_CMD,         tr("Missing parameter (cmd)"));
   errMap.insert(CIptvDefs::ERR_MANAGE_BAD_VALUE,        tr("Bad value (rate)"));
   errMap.insert(CIptvDefs::ERR_MANAGE_FILM_NOT_FOUND,   tr("Can't find film"));
   errMap.insert(CIptvDefs::ERR_MANAGE_ALREADY_ADDED,    tr("Film already added"));
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/
