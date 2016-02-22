/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/COtradaClient.cpp $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: 19.03.2013
|
| $Id: COtradaClient.cpp 1495 2015-02-20 13:48:22Z Olenka.Joerg $
|
\=============================================================================*/
#include "cotradaclient.h"
#include "small_helpers.h"
#include "qcustparser.h"
#include "ctimeshift.h"

// global customization class ...
extern QCustParser *pCustomization;

// log file functions ...
extern CLogFile VlcLog;

// global timeshift class ...
extern CTimeShift *pTs;

/*-----------------------------------------------------------------------------\
| Function:    COtradaClient / constructor
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: constructs a COtradaClient object to communicate with
|              iptv sender
| Parameters:  --
|
\-----------------------------------------------------------------------------*/
COtradaClient::COtradaClient(QObject *parent) :QIptvCtrlClient(parent)
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

   setObjectName("COtradaClient");
   // tPing.setInterval(60000);
   // tPing.start();

   eIOps = IO_DUNNO;
}

/*-----------------------------------------------------------------------------\
| Function:    ~COtradaClient / destructor
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
COtradaClient::~COtradaClient()
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
void COtradaClient::slotStringResponse (int reqId, QString strResp)
{
    int     iErr = 0;
    QString sCleanResp;
    bool    bSendResp = true;

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
            /// ugly radio not there hack ...
            if ((CIptvDefs::EReq)reqId == CIptvDefs::REQ_CHANLIST_RADIO)
            {
                // in case the account doesn't support
                // radio this error might happen -> ignore and send
                // TV channel list instead!
                reqId      = (int)CIptvDefs::REQ_CHANNELLIST;
                sCleanResp = combineChannelLists("");

                emit sigHttpResponse (sCleanResp, reqId);
            }
            else
            {
                emit sigError(sCleanResp, reqId, iErr);
            }
        }
        else
        {
            // modify response as well as id if needed ...
            switch ((CIptvDefs::EReq)reqId)
            {
                // radio stream is a normal stream ...
                case CIptvDefs::REQ_RADIO_STREAM:
                    reqId = (int)CIptvDefs::REQ_STREAM;
                    break;

                // radio timer record should be handled as timer record ...
                case CIptvDefs::REQ_RADIO_TIMERREC:
                    reqId = (int)CIptvDefs::REQ_TIMERREC;
                    break;

                // special handling to concat TV channels with radio channels ...
                case CIptvDefs::REQ_CHANNELLIST:
                    sChanListBuffer = sCleanResp;
                    getRadioList();
                    bSendResp = false;
                    break;

                case CIptvDefs::REQ_CHANLIST_RADIO:
                    sCleanResp = combineChannelLists(sCleanResp);
                    reqId      = (int)CIptvDefs::REQ_CHANNELLIST;
                    break;

                case CIptvDefs::REQ_INNER_OPS:
                    handleInnerOps(sCleanResp);
                    bSendResp = false;
                    break;

                default:
                    break;
            }

            // send response ... ?
            if (bSendResp)
            {
                emit sigHttpResponse (sCleanResp, reqId);
            }
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
void COtradaClient::slotBinResponse (int reqId, QByteArray binResp)
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
void COtradaClient::slotErr (int iReqId, QString sErr, int iErr)
{
   emit sigError(sErr, iReqId, iErr);
}

//---------------------------------------------------------------------------
//
//! \brief   combine tv and radio channel list
//
//! \author  Jo2003
//! \date    25.03.2013
//
//! \param   resp (QString) radio channel list
//
//! \return  ref. to combined string
//---------------------------------------------------------------------------
QString COtradaClient::combineChannelLists(const QString& resp)
{
    QRegExp rx;
    QString sFound;

    // create whole new channel list ...
    QString chanListTmpl = (QString)
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
    + "<response>"
    + "<icons><tv>[%%TV%%]</tv><radio>[%%RADIO%%]</radio></icons>"
    + "<groups_tv>[%%GROUPS_TV%%]</groups_tv>"
    + "<groups_radio>[%%GROUPS_RADIO%%]</groups_radio>"
    + "<servertime>[%%SERVERTIME%%]</servertime>"
    + "</response>";

    // servertime from TV channels ...
    rx.setPattern("<servertime>(.*)</servertime>");
    sFound = (rx.indexIn(sChanListBuffer) > -1) ? rx.cap(1) : "";
    chanListTmpl.replace("[%%SERVERTIME%%]", sFound);

    // TV channels icon ...
    rx.setPattern("<default>(.*)</default>");
    sFound = (rx.indexIn(sChanListBuffer) > -1) ? rx.cap(1) : "";
    chanListTmpl.replace("[%%TV%%]", sFound);

    // Radio channel icons ...
    sFound = (rx.indexIn(resp) > -1) ? rx.cap(1) : "";
    chanListTmpl.replace("[%%RADIO%%]", sFound);

    // TV groups and channels ...
    rx.setPattern("<groups>(.*)</groups>");
    sFound = (rx.indexIn(sChanListBuffer) > -1) ? rx.cap(1) : "";
    chanListTmpl.replace("[%%GROUPS_TV%%]", sFound);

    // radio groups and channels ...
    sFound = (rx.indexIn(resp) > -1) ? rx.cap(1) : "";
    chanListTmpl.replace("[%%GROUPS_RADIO%%]", sFound);

#ifdef __TRACE
    mInfo(tr("Patched channel list:\n ==8<==8<==8<==\n%2\n ==>8==>8==>8==")
          .arg(chanListTmpl));
#endif // __TRACE

    return chanListTmpl;
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
int COtradaClient::queueRequest(CIptvDefs::EReq req, const QVariant& par_1, const QVariant& par_2)
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
      case CIptvDefs::REQ_CHANNELLIST:
         GetChannelList();
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
      case CIptvDefs::REQ_RADIO_STREAM:
         getRadioStream(par_1.toInt());
         break;
      case CIptvDefs::REQ_RADIO_TIMERREC:
         getRadioStream(par_1.toInt(), true);
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
void COtradaClient::SetData(const QString &host, const QString &usr,
                           const QString &pw, const QString &lang)
{
   sUsr           = usr;
   sPw            = pw;
   sLang          = lang;
   sApiUrl        = QString("http://%1%2").arg(host).arg(pCustomization->strVal("API_XML_PATH"));
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
void COtradaClient::SetCookie(const QString &cookie)
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
void COtradaClient::Logout ()
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
void COtradaClient::GetCookie ()
{
   mInfo(tr("Request Authentication ..."));

   q_post((int)CIptvDefs::REQ_COOKIE, sApiUrl + "login",
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
void COtradaClient::GetChannelList ()
{
   mInfo(tr("Request Channel List ..."));

   // reset language filter ...
   sLangFilter = "";

   QString req = QString("get_list_tv?with_epg=1&time_shift=%1&mode=1").arg(pTs->timeShift());

   // request channel list or channel list for settings ...
   q_get((int)CIptvDefs::REQ_CHANNELLIST, sApiUrl + req);
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
void COtradaClient::chanListLang(const QString& lang)
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
void COtradaClient::audioLang()
{
   mInfo(tr("Request available audio streams ..."));

   // request language filtered channel list ...
   q_get((int)CIptvDefs::REQ_GET_ALANG, sApiUrl + "get_audio_lang");
}

//---------------------------------------------------------------------------
//
//! \brief   special handling inside client
//
//! \author  Jo2003
//! \date    22.02.2016
//
//! \param   resp response string
//
//! \return  --
//---------------------------------------------------------------------------
void COtradaClient::handleInnerOps(const QString &resp)
{
    QRegExp rx;
    QString req;

    if ((eIOps == IO_ADD_FAV) || (eIOps == IO_DEL_FAV) || (eIOps == IO_FAV_LIST))
    {
        rx.setPattern("<favorites>(.*)</favorites>");

        if (rx.indexIn(resp) > -1)
        {
            QStringList sl;

            switch (eIOps)
            {
            case IO_ADD_FAV:
                sl = rx.cap(1).split(",", QString::SkipEmptyParts);
                sl << QString::number(vaIOdata.toInt());
                req = QString("val=%1").arg(sl.join(","));
                q_post((int)CIptvDefs::REQ_ADD_VOD_FAV, sApiUrl + "set_favorites_movie", req);
                break;

            case IO_DEL_FAV:
                sl = rx.cap(1).split(",", QString::SkipEmptyParts);
                sl.removeOne(QString::number(vaIOdata.toInt()));
                req = QString("val=%1").arg(sl.join(","));
                q_post((int)CIptvDefs::REQ_REM_VOD_FAV, sApiUrl + "set_favorites_movie", req);
                break;

            case IO_FAV_LIST:
                if (rx.cap(1).isEmpty())
                {
                    emit sigHttpResponse ("<xml/>", CIptvDefs::REQ_GETVIDEOS);
                }
                else
                {
                    req = QString("idlist=%1").arg(rx.cap(1));
                    q_post((int)CIptvDefs::REQ_GETVIDEOS, sApiUrl + "get_list_movie", req);
                }
                break;

            default:
                break;
            }
        }
    }
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
void COtradaClient::GetProtChannelList (const QString &secCode)
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
void COtradaClient::GetServer()
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
void COtradaClient::GetTimeShift()
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
void COtradaClient::SetTimeShift (int iHours)
{
   mInfo(tr("Set TimeShift to %1 hour(s) ...").arg(iHours));

   q_post((int)CIptvDefs::REQ_TIMESHIFT, sApiUrl + "set",
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
void COtradaClient::GetBitRate()
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
void COtradaClient::SetBitRate(int iRate)
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
void COtradaClient::GetStreamURL(int iChanID, const QString &secCode, bool bTimerRec)
{
   mInfo(tr("Request URL for channel %1 ...").arg(iChanID));

   QString req = QString("cid=%1&time_shift=%2").arg(iChanID).arg(pTs->timeShift());

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_post((bTimerRec) ? (int)CIptvDefs::REQ_TIMERREC : (int)CIptvDefs::REQ_STREAM,
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
void COtradaClient::SetServer (const QString &sIp)
{
   mInfo(tr("Set Streaming Server to %1 ...").arg(sIp));

   q_post((int)CIptvDefs::REQ_SERVER, sApiUrl + "set",
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
void COtradaClient::SetHttpBuffer(int iTime)
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
void COtradaClient::GetEPG(int iChanID, int iOffset)
{
   mInfo(tr("Request EPG for Channel %1 ...").arg(iChanID));

   QDateTime dt(QDate::currentDate().addDays(iOffset));

   q_post((int)CIptvDefs::REQ_EPG, sApiUrl + "get_epg",
          QString("cid=%1&from_uts=%2&hours=24&time_shift=%3").arg(iChanID).arg(dt.toTime_t()).arg(pTs->timeShift()));
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
void COtradaClient::GetArchivURL (const QString &prepared, const QString &secCode)
{
   mInfo(tr("Request Archiv URL ..."));

   QString req = QUrl::fromPercentEncoding(prepared.toUtf8());

   // adapt rodnoe ...
   req.replace("&gmt=", "&uts=");

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_post((int)CIptvDefs::REQ_ARCHIV, sApiUrl + "get_url_tv", req);
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
void COtradaClient::GetVodGenres()
{
   mInfo(tr("Request VOD Genres ..."));

   q_get((int)CIptvDefs::REQ_GETVODGENRES, sApiUrl + "get_genre_movie");
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
void COtradaClient::GetVideos(const QString &sPrepared)
{
   mInfo(tr("Request Videos ..."));

   QString sHelp = sPrepared;
   sHelp.replace("nums", "limit");

   if (!sHelp.contains("limit"))
   {
       sHelp += "&limit=20";
   }

   q_get((int)CIptvDefs::REQ_GETVIDEOS, sApiUrl + "get_list_movie?" + QUrl::fromPercentEncoding(sHelp.toUtf8()));
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
void COtradaClient::GetVideoInfo(int iVodID, const QString &secCode)
{
   mInfo(tr("Request Video info for video %1...").arg(iVodID));

   QString req = QString("get_list_movie?extended=1&idlist=%1").arg(iVodID);

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
void COtradaClient::GetVodUrl(int iVidId, const QString &secCode)
{
   mInfo(tr("Request Video Url for video %1...").arg(iVidId));

   QString req = QString("get_url_movie?cid=%1")
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
void COtradaClient::setChanHide(const QString &cids, const QString &secCode)
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
void COtradaClient::setChanShow(const QString &cids, const QString &secCode)
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
void COtradaClient::getVodManager(const QString &secCode)
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
void COtradaClient::setVodManager(const QString &rules, const QString &secCode)
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
void COtradaClient::addVodFav(int iVidID, const QString &secCode)
{
    Q_UNUSED(secCode)
    mInfo(tr("Add VOD favourite (%1) ...").arg(iVidID));
    eIOps    = IO_ADD_FAV;
    vaIOdata = iVidID;
    q_get((int)CIptvDefs::REQ_INNER_OPS, sApiUrl + "get_favorites_movie");
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
void COtradaClient::remVodFav(int iVidID, const QString &secCode)
{
    Q_UNUSED(secCode)
    mInfo(tr("Remove VOD favourite (%1) ...").arg(iVidID));
    eIOps    = IO_DEL_FAV;
    vaIOdata = iVidID;
    q_get((int)CIptvDefs::REQ_INNER_OPS, sApiUrl + "get_favorites_movie");
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
void COtradaClient::getVodFav()
{
   mInfo(tr("Get VOD favourites (%1) ..."));
   eIOps = IO_FAV_LIST;
   q_get((int)CIptvDefs::REQ_INNER_OPS, sApiUrl + "get_favorites_movie");
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
void COtradaClient::setParentCode(const QString &oldCode, const QString &newCode)
{
   mInfo(tr("Change parent code ..."));

   QString req = QString("var=parental_pass&val=%1&protect_code=%2")
         .arg(newCode).arg(oldCode);

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
void COtradaClient::epgCurrent(const QString &cids)
{
   mInfo(tr("EPG current for Channels: %1 ...").arg(cids));

   q_post((int)CIptvDefs::REQ_EPG_CURRENT, sApiUrl + "get_epg_current", QString("cid=%1&time_shift=%2").arg(cids).arg(pTs->timeShift()));
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
void COtradaClient::updInfo (const QString& url)
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
void COtradaClient::slotDownImg(const QString &url)
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
void COtradaClient::slotPing()
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
void COtradaClient::getRadioList()
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
//! \brief   get stream url for radio
//
//! \author  Jo2003
//! \date    25.03.2013
//
//! \param   cid (int) channel id
//! \param   bTimerRec (bool) timer record flag
//
//! \return  --
//---------------------------------------------------------------------------
void COtradaClient::getRadioStream(int cid, bool bTimerRec)
{
   mInfo(tr("Get radio stream Url ..."));

   q_post(bTimerRec ? (int)CIptvDefs::REQ_RADIO_TIMERREC : (int)CIptvDefs::REQ_RADIO_STREAM,
          sApiUrl + "get_url_radio", QString("cid=%1&time_shift=%2").arg(cid & ~RADIO_OFFSET).arg(pTs->timeShift()));
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
void COtradaClient::setInterfaceLang (const QString& langCode)
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
bool COtradaClient::cookieSet()
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
int COtradaClient::checkResponse (const QString &sResp, QString &sCleanResp)
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
void COtradaClient::fillErrorMap()
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
