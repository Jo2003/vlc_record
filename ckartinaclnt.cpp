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
| Parameters:  host, username, password, erotic allowed (true / false)
|
\-----------------------------------------------------------------------------*/
CKartinaClnt::CKartinaClnt(const QString &host, const QString &usr,
                           const QString &pw, const QString &sEPw,
                           bool bAllowErotic) :QHttp(host)
{
   sUsr           = usr;
   sPw            = pw;
   sErosPw        = sEPw;
   iReq           = -1;
   bEros          = bAllowErotic;
   sCookie        = "";
   sHost          = host;
   eReq           = Kartina::REQ_UNKNOWN;

   bufReq.open(QIODevice::WriteOnly);

   connect(this, SIGNAL(requestFinished(int, bool)), this, SLOT(handleEndRequest(int, bool)));
}

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
CKartinaClnt::CKartinaClnt() :QHttp()
{
   sUsr           = "";
   sPw            = "";
   sErosPw        = "";
   iReq           = -1;
   bEros          = false;
   sCookie        = "";
   sHost          = "";
   eReq           = Kartina::REQ_UNKNOWN;

   bufReq.open(QIODevice::WriteOnly);

   connect(this, SIGNAL(requestFinished(int, bool)), this, SLOT(handleEndRequest(int, bool)));
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
   abort();
}

/*-----------------------------------------------------------------------------\
| Function:    SetData
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: set communication parameter to communicate with
|              kartina.tv
| Parameters:  host, username, password, erotic allowed (true / false)
|
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetData(const QString &host, const QString &usr,
                           const QString &pw, const QString &sEPw,
                           bool bAllowErotic)
{
   sUsr           = usr;
   sPw            = pw;
   sErosPw        = sEPw;
   bEros          = bAllowErotic;
   sHost          = host;
   sCookie        = "";
   eReq           = Kartina::REQ_UNKNOWN;

   setHost(host);
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
   sCookie      = cookie;
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

   GetRequest(Kartina::REQ_LOGOUT, KARTINA_API_PATH "logout");
}

/*-----------------------------------------------------------------------------\
| Function:    PostRequest
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: send a post request to kartina.tv
|
| Parameters:  request type, path on server, request string
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::PostRequest (Kartina::EReq req,
                                const QString &path,
                                const QString &content,
                                const QString &sBrowser)
{
   eReq = req;
   QHttpRequestHeader header("POST", path.toAscii());
   header.addValue("Host", sHost);
   header.setContentType("application/x-www-form-urlencoded");
   header.addValue("User-Agent", sBrowser);
   header.addValue("Connection", "close");
   if (sCookie != "")
   {
      header.addValue("Cookie", sCookie);
   }
   header.setContentLength(content.toAscii().size());

   // open and clean data buffer ...
   bufReq.open(QIODevice::WriteOnly | QIODevice::Truncate);

   // post request ...
   iReq = request(header, content.toAscii(), &bufReq);

   mInfo(tr("Request #%1 sent ...").arg(iReq));
}

/*-----------------------------------------------------------------------------\
| Function:    GetRequest
|
| Author:      Jo2003
|
| Begin:       28.07.2010 / 16:14:52
|
| Description: send a get request to kartina.tv
|
| Parameters:  request type, request
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetRequest (Kartina::EReq req,
                               const QString &sRequest,
                               const QString &sBrowser)
{
   eReq = req;
   QHttpRequestHeader header("GET", sRequest.toAscii());
   header.addValue("Host", sHost);
   header.setContentType("application/x-www-form-urlencoded");
   header.addValue("User-Agent", sBrowser);
   header.addValue("Connection", "close");
   if (sCookie != "")
   {
      header.addValue("Cookie", sCookie);
   }
   header.setContentLength(0);

   // open and clean data buffer ...
   bufReq.open(QIODevice::WriteOnly | QIODevice::Truncate);

   // post request ...
   iReq = request(header, QByteArray(), &bufReq);

   mInfo(tr("Request #%1 sent ...").arg(iReq));
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

   PostRequest(Kartina::REQ_COOKIE, KARTINA_API_PATH "login",
               QString("login=%1&pass=%2").arg(sUsr).arg(sPw));
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
void CKartinaClnt::GetChannelList ()
{
   mInfo(tr("Request Channel List ..."));

   GetRequest(Kartina::REQ_CHANNELLIST, KARTINA_API_PATH "channel_list");
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

   GetRequest(Kartina::REQ_GET_SERVER, KARTINA_API_PATH "settings?var=stream_server");
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

   GetRequest(Kartina::REQ_GETTIMESHIFT, KARTINA_API_PATH "settings?var=timeshift");
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

   PostRequest(Kartina::REQ_TIMESHIFT, KARTINA_API_PATH "settings_set",
               QString("var=timeshift&val=%1").arg(iHours));
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
| Parameters:  channel index
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetStreamURL(int iChanID, bool bTimerRec)
{
   mInfo(tr("Request URL for channel %1 ...").arg(iChanID));

   QString req = QString("cid=%1").arg(iChanID);

   if (bEros)
   {
      req += QString("&protect_code=%1").arg(sErosPw);
   }

   PostRequest((bTimerRec) ? Kartina::REQ_TIMERREC : Kartina::REQ_STREAM,
               KARTINA_API_PATH "get_url", req);
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

   PostRequest(Kartina::REQ_SERVER, KARTINA_API_PATH "settings_set",
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

   PostRequest(Kartina::REQ_SERVER, KARTINA_API_PATH "settings_set",
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

   GetRequest(Kartina::REQ_EPG,
              QString("%1epg?cid=%2&day=%3").arg(KARTINA_API_PATH)
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
void CKartinaClnt::GetArchivURL (const QString &prepared)
{
   mInfo(tr("Request Archiv URL ..."));

   QString req = prepared;

   if (bEros)
   {
      req += QString("&protect_code=%1").arg(sErosPw);
   }

   PostRequest(Kartina::REQ_ARCHIV, KARTINA_API_PATH "get_url", req);
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

   GetRequest(Kartina::REQ_GETVODGENRES, "/?m=vod&act=home", "User-Agent: Mozilla/5.0");
}

/*-----------------------------------------------------------------------------\
| Function:    GetVideos
|
| Author:      Jo2003
|
| Begin:       09.12.2010 / 13:18
|
| Description: get vidoes matching genre id (VOD)
|
| Parameters:  genre id
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::GetVideos(int iGenreID)
{
   mInfo(tr("Request Vidoes for Genres %1...").arg(iGenreID));

   QString sReq = QString("%1vod_list?type=last").arg(KARTINA_API_PATH);

   if (iGenreID != -1)
   {
      sReq += QString("&genre=%1").arg(iGenreID);
   }

   GetRequest(Kartina::REQ_GETVIDEOS, sReq);
}

/*-----------------------------------------------------------------------------\
| Function:    handleEndRequest (slot)
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: sends messages if a request has finished ...
|
| Parameters:  request id (unused), error inidicator
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::handleEndRequest(int id, bool err)
{
   // is this our request ... ?
   if (id == iReq)
   {
      // close buffer device and open for read only...
      bufReq.close();
      bufReq.open(QIODevice::ReadOnly);

      // read all content ...
      baPageContent = bufReq.readAll();

      // close buffer device ...
      bufReq.close();

      if (!err)
      {
         mInfo(tr("Request #%1 done!").arg(id));

         // send signals dependet on ended request ...
         switch (eReq)
         {
         case Kartina::REQ_COOKIE:
            emit sigGotCookie(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_CHANNELLIST:
            emit sigGotChannelList(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_TIMESHIFT:
            emit sigTimeShiftSet(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_EPG:
            emit sigGotEPG(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_SERVER:
            emit sigServerChanged(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_HTTPBUFF:
            emit sigBufferSet(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_STREAM:
            emit sigGotStreamURL(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_TIMERREC:
            emit sigGotTimerStreamURL (QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_ARCHIV:
            emit sigGotArchivURL(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_GET_SERVER:
            emit sigSrvForm (QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_LOGOUT:
            sCookie = "";
            emit sigLogout (QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_GETTIMESHIFT:
            emit sigGotTimeShift(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_GETVODGENRES:
            emit sigGotVodGenres(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_GETVIDEOS:
            emit sigGotVideos(QString::fromUtf8(baPageContent.constData()));
            break;
         default:
            break;
         }
      }
      else
      {
         mErr(tr("Error in Request: %1!").arg(errorString()));

         // send error signal ...
         emit sigError(errorString());
      }

      // mark request as ended so the API is "free for use" again ...
      eReq = Kartina::REQ_UNKNOWN;
   }
}

/* -----------------------------------------------------------------\
|  Method: busy
|  Begin: 18.01.2010 / 16:32:33
|  Author: Jo2003
|  Description: is api busy ?
|
|  Parameters: --
|
|  Returns: true ==> busy
|          false ==> available
\----------------------------------------------------------------- */
bool CKartinaClnt::busy ()
{
   return ((eReq != Kartina::REQ_UNKNOWN) || (sCookie == "")) ? true : false;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/
