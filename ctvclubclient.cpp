/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     ctvclubclient.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#include "ctvclubclient.h"
#include "small_helpers.h"
#include "qcustparser.h"
#include "ctimeshift.h"
#include <QtJson>

// global customization class ...
extern QCustParser *pCustomization;

// log file functions ...
extern CLogFile VlcLog;

// global timeshift class ...
extern CTimeShift *pTs;

// device name
#define TVCLUB_DEV_NAME "&device=vlcrecords"

/*-----------------------------------------------------------------------------\
| Function:    CTVClubClient / constructor
|
| Author:      Jo2003
|
| Begin:       15.01.2017
|
| Description: constructs a CTVClubClient object to communicate with
|              iptv sender
| Parameters:  parent object
|
\-----------------------------------------------------------------------------*/
CTVClubClient::CTVClubClient(QObject *parent) :QIptvCtrlClient(parent)
{
   sUsr           = "";
   sPw            = "";
   sCookie        = "";
   sApiUrl        = "";
   sLang          = "";

   connect(this, SIGNAL(sigStringResponse(int,QString)), this, SLOT(slotStringResponse(int,QString)));
   connect(this, SIGNAL(sigBinResponse(int,QByteArray)), this, SLOT(slotBinResponse(int,QByteArray)));
   connect(this, SIGNAL(sigErr(int,QString,int)), this, SLOT(slotErr(int,QString,int)));
   // connect(&tPing, SIGNAL(timeout()), this, SLOT(slotPing()));

   setObjectName("CTVClubClient");
   // tPing.setInterval(60000);
   // tPing.start();
}

/*-----------------------------------------------------------------------------\
| Function:    ~CTVClubClient / destructor
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
CTVClubClient::~CTVClubClient()
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
void CTVClubClient::slotStringResponse (int reqId, QString strResp)
{
    int iErr = 0;

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
        if ((iErr = checkResponse(strResp)) != 0)
        {
            emit sigError(strResp, reqId, iErr);
        }
        else
        {
            emit sigHttpResponse (strResp, reqId);
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
void CTVClubClient::slotBinResponse (int reqId, QByteArray binResp)
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
void CTVClubClient::slotErr (int iReqId, QString sErr, int iErr)
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
int CTVClubClient::queueRequest(CIptvDefs::EReq req, const QVariant& par_1, const QVariant& par_2)
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
      case CIptvDefs::REQ_CHANNELGROUPS:
         getChanGroups();
         break;
      case CIptvDefs::REQ_CHANNELLIST:
         GetChannelList(par_1.toInt());
         break;
      case CIptvDefs::REQ_COOKIE:
         GetCookie();
         break;
      case CIptvDefs::REQ_EPG:
         GetEPG(par_1.toInt(), par_2.toInt());
         break;
      case CIptvDefs::REQ_SERVER:
         SetServer(par_1.toString());
         break;
      case CIptvDefs::REQ_HTTPBUFF:
         SetHttpBuffer(par_1.toInt());
         break;
      case CIptvDefs::REQ_STREAM:
         GetStreamURL(par_1.toInt(), par_2.toString());
         break;
      case CIptvDefs::REQ_TIMERREC:
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
         GetProtChannelList(par_1.toString());
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
      case CIptvDefs::REQ_SET_LANGUAGE:
         setInterfaceLang(par_1.toString());
         break;
      case CIptvDefs::REQ_CL_LANG:
         chanListLang(par_1.toString());
         break;
      case CIptvDefs::REQ_GET_ALANG:
         audioLang();
         break;
      case CIptvDefs::REQ_SETTINGS:
          getSettings();
          break;
      case CIptvDefs::REQ_FAVS_GET:
          getFavs();
          break;
      case CIptvDefs::REQ_FAVS_SET:
          setFavs(par_1.toStringList());
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
void CTVClubClient::SetData(const QString &host, const QString &usr,
                           const QString &pw, const QString &lang)
{
   sUsr           = usr;
   sPw            = pw;
   sLang          = lang;
   sApiUrl        = QString("http://%1%2").arg(host).arg(pCustomization->strVal("API_JSON_PATH"));
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
void CTVClubClient::SetCookie(const QString &cookie)
{
   mInfo(tr("We've got following Cookie: %1").arg(cookie));
   sCookie = cookie + TVCLUB_DEV_NAME;
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
void CTVClubClient::Logout ()
{
   mInfo(tr("Logout ..."));
   q_get((int)CIptvDefs::REQ_LOGOUT, sApiUrl + "logout?" + sCookie, Iptv::Logout);
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
void CTVClubClient::GetCookie ()
{
   mInfo(tr("Request Authentication ..."));

   q_post((int)CIptvDefs::REQ_COOKIE, sApiUrl + "auth",
        QString("hash=%1%2")
            .arg(CSmallHelpers::md5(sUsr + CSmallHelpers::md5(sPw)))
            .arg(TVCLUB_DEV_NAME),
        Iptv::Login);
}

/*-----------------------------------------------------------------------------\
| Function:    getChanGroups
|
| Author:      Jo2003
|
| Begin:       15.01.2017
|
| Description: request channel groups
|
\-----------------------------------------------------------------------------*/
void CTVClubClient::getChanGroups ()
{
   mInfo(tr("Request Channel Groups ..."));

   // reset language filter ...
   sLangFilter = "";

   QString req = QString("groups?%1").arg(sCookie);

   // request channel groups
   q_get((int)CIptvDefs::REQ_CHANNELGROUPS, sApiUrl + req);
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
void CTVClubClient::GetChannelList (int gid)
{
   mInfo(tr("Request Channel List ..."));

   // reset language filter ...
   sLangFilter = "";

   QString req = QString("channels?gid=%1&limit=no&%2").arg(gid).arg(sCookie);

   // request channel list or channel list for settings ...
   q_get((int)CIptvDefs::REQ_CHANNELLIST, sApiUrl + req);
}

//---------------------------------------------------------------------------
/// \brief CTVClubClient::getFavs
//---------------------------------------------------------------------------
void CTVClubClient::getFavs()
{
   mInfo(tr("Request favourite channels ..."));

   QString req = QString("channels?gid=100&limit=no&%1").arg(sCookie);

   // request channel list or channel list for settings ...
   q_get((int)CIptvDefs::REQ_FAVS_GET, sApiUrl + req);
}

//---------------------------------------------------------------------------
/// \brief CTVClubClient::setFavs
/// \param favs favorites as string list
//---------------------------------------------------------------------------
void CTVClubClient::setFavs(const QStringList &favs)
{
    QString req;

    if ((favs.count() == 2) && (favs.at(0) == "0"))
    {
        mInfo(tr("Delete last favourite channel: %1 ...").arg(favs.at(1)));
        req = QString("cid=%1&pos=del&%2").arg(favs.at(1)).arg(sCookie);
    }
    else
    {
        mInfo(tr("Set favourite channels: %1 ...").arg(favs.join(",")));
        req = QString("set=%1&%2").arg(favs.join(",")).arg(sCookie);
    }

    q_post((int)CIptvDefs::REQ_FAVS_SET, sApiUrl + "set_favorites", req);
}

//---------------------------------------------------------------------------
//
//! \brief   request language filtered channel list
//
//! \author  Jo2003
//! \date    29.07.2013
//
//! \param   lang (const QString&) language code string
//
//! \return  --
//---------------------------------------------------------------------------
void CTVClubClient::chanListLang(const QString& lang)
{
   mInfo(tr("Request Channel List (language filtered) ..."));

   // store language filter ...
   sLangFilter = lang;

   QString req = QString("get_list_tv?with_epg=1&afilter=%1&time_shift=%2").arg(lang).arg(pTs->timeShift());

   // request language filtered channel list ...
   q_get((int)CIptvDefs::REQ_CHANNELLIST, sApiUrl + req);
}

//---------------------------------------------------------------------------
//
//! \brief   request available audio streams
//
//! \author  Jo2003
//! \date    29.07.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void CTVClubClient::audioLang()
{
   mInfo(tr("Request available audio streams ..."));

   // request language filtered channel list ...
   q_get((int)CIptvDefs::REQ_GET_ALANG, sApiUrl + "get_audio_lang");
}

//---------------------------------------------------------------------------
//! \brief   request all settings
//---------------------------------------------------------------------------
void CTVClubClient::getSettings()
{
    mInfo(tr("get settings ..."));
    q_get((int)CIptvDefs::REQ_SETTINGS, sApiUrl + "settings?" + sCookie, Iptv::String);
}

/*-----------------------------------------------------------------------------\
| Function:    GetProtChannelList
|
| Author:      Jo2003
|
| Begin:       22.03.2013
|
| Description: request channel list
|
| Parameters:  protect_code
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CTVClubClient::GetProtChannelList (const QString &secCode)
{
   mInfo(tr("Request Channel List ..."));

   QString req = QString("with_epg=1&show=all&protect_code=%1&time_shift=%2").arg(secCode).arg(pTs->timeShift());

   // request channel list or channel list for settings ...
   q_post((int)CIptvDefs::REQ_CHANLIST_ALL, sApiUrl + "get_list_tv", req);
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
void CTVClubClient::GetServer()
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
void CTVClubClient::GetTimeShift()
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
void CTVClubClient::SetTimeShift (int iHours)
{
    Q_UNUSED(iHours)
    mInfo(tr("Timeshift not supported ..."));
   /*
    mInfo(tr("Set TimeShift to %1 hour(s) ...").arg(iHours));

    q_post((int)CIptvDefs::REQ_TIMESHIFT, sApiUrl + "set",
               QString("var=time_shift&val=%1").arg(iHours));
    */
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
void CTVClubClient::GetBitRate()
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
void CTVClubClient::SetBitRate(int iRate)
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
void CTVClubClient::GetStreamURL(int iChanID, const QString &secCode, bool bTimerRec)
{
   mInfo(tr("Request URL for channel %1 ...").arg(iChanID));

   QString req = QString("cid=%1&%2").arg(iChanID).arg(sCookie);

   if (secCode != "")
   {
      req += QString("&protected=%1").arg(secCode);
   }

   q_post((bTimerRec) ? (int)CIptvDefs::REQ_TIMERREC : (int)CIptvDefs::REQ_STREAM,
               sApiUrl + "live", req);
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
void CTVClubClient::SetServer (const QString &sIp)
{
   mInfo(tr("Set Streaming Server to %1 ...").arg(sIp));

   q_post((int)CIptvDefs::REQ_SERVER, sApiUrl + "set",
               QString("server=%1&%2").arg(sIp).arg(sCookie));
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
void CTVClubClient::SetHttpBuffer(int iTime)
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
void CTVClubClient::GetEPG(int iChanID, int iOffset)
{
   mInfo(tr("Request EPG for Channel %1 ...").arg(iChanID));
   QDateTime dt(QDate::currentDate().addDays(iOffset));

   QString sReq = QString("epg?channels=%1&time=%2&period=24&limit=100&%3")
           .arg(iChanID)
           .arg(dt.toTime_t())
           .arg(sCookie);

   q_get((int)CIptvDefs::REQ_EPG, sApiUrl + sReq);
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
void CTVClubClient::GetArchivURL (const QString &prepared, const QString &secCode)
{
   mInfo(tr("Request Archiv URL ..."));

   QString req = QUrl::fromPercentEncoding(prepared.toUtf8());

   // adapt rodnoe ...
   req.replace("&gmt=", "&time=");


   req += QString("&%1").arg(sCookie);


   if (secCode != "")
   {
      req += QString("&protected=%1").arg(secCode);
   }

   q_post((int)CIptvDefs::REQ_ARCHIV, sApiUrl + "rec", req);
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
void CTVClubClient::GetVodGenres()
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
void CTVClubClient::GetVideos(const QString &sPrepared)
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
void CTVClubClient::GetVideoInfo(int iVodID, const QString &secCode)
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
void CTVClubClient::GetVodUrl(int iVidId, const QString &secCode)
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
void CTVClubClient::setChanHide(const QString &cids, const QString &secCode)
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
void CTVClubClient::setChanShow(const QString &cids, const QString &secCode)
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
void CTVClubClient::getVodManager(const QString &secCode)
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
void CTVClubClient::setVodManager(const QString &rules, const QString &secCode)
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
void CTVClubClient::addVodFav(int iVidID, const QString &secCode)
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
void CTVClubClient::remVodFav(int iVidID, const QString &secCode)
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
void CTVClubClient::getVodFav()
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
void CTVClubClient::setParentCode(const QString &oldCode, const QString &newCode)
{
   mInfo(tr("Change parent code ..."));

   QString req = QString("new_code=%1&old_code=%2&%3")
           .arg(newCode)
           .arg(oldCode)
           .arg(sCookie);

   q_post((int)CIptvDefs::REQ_SET_PCODE, sApiUrl + "set", req);
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
void CTVClubClient::epgCurrent(const QString &cids)
{
   mInfo(tr("EPG current for Channels: %1 ...").arg(cids));


   QString sReq = QString("epg?channels=%1&c_to=3&limit=300&%2")
           .arg(cids)
           .arg(sCookie);

   q_get((int)CIptvDefs::REQ_EPG_CURRENT, sApiUrl + sReq);
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
void CTVClubClient::updInfo (const QString& url)
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
void CTVClubClient::slotDownImg(const QString &url)
{
   mInfo(tr("Download image ..."));

   q_get((int)CIptvDefs::REQ_DOWN_IMG, url, Iptv::Binary);
}

//---------------------------------------------------------------------------
//
//! \brief   send PING to API server
//
//! \author  Jo2003
//! \date    20.02.2015
//
//---------------------------------------------------------------------------
void CTVClubClient::slotPing()
{
   if (sCookie != "")
   {
      QString req = QString("%1noop?%2").arg(sApiUrl).arg(sCookie);
      get(CIptvDefs::REQ_NOOP, req, Iptv::noop);
      mInfo(tr("Noop Ping"));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get channel list for radio
//
//! \author  Jo2003
//! \date    25.03.2013
//
//
//! \return  --
//---------------------------------------------------------------------------
void CTVClubClient::getRadioList()
{
   mInfo(tr("Download radio list ..."));

   QString req = QString("get_list_radio?time_shift=%1").arg(pTs->timeShift());

   if(!sLangFilter.isEmpty())
   {
      req += "&afilter=" + sLangFilter;
   }

   q_get((int)CIptvDefs::REQ_CHANLIST_RADIO, sApiUrl + req);
}

//---------------------------------------------------------------------------
//
//! \brief   set interface language
//
//! \author  Jo2003
//! \date    25.03.2013
//
//! \param   langCode (const QString&) new language code
//
//! \return  --
//---------------------------------------------------------------------------
void CTVClubClient::setInterfaceLang (const QString& langCode)
{
   mInfo(tr("Set interface language ..."));

   q_post((int)CIptvDefs::REQ_SET_LANGUAGE, sApiUrl + "set",
          QString("var=interface_lng&val=%1").arg(langCode));
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
bool CTVClubClient::cookieSet()
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
|
|  Returns: error code
\----------------------------------------------------------------- */
int CTVClubClient::checkResponse (QString &sResp)
{
    bool ok;
    int  iRet = 0;

    // {"error":{"code":9,"msg":"Channel not found or not allowed"}}

    QtJson::JsonObject data = QtJson::parse(sResp, ok).toMap();

    if (ok)
    {
        if (data.contains("error"))
        {
            data  = data.value("error").toMap();
            iRet  = data.value("code").toInt();
            sResp = data.value("msg").toString();
        }
    }

    return iRet;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/
