/*=============================================================================\
| $HeadURL$
|
| Author: Joerg Neubert
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
| Author:      Joerg Neubert
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: constructs a CKartinaClnt object to communicate with
|              kartina.tv
| Parameters:  host, username, password, erotic allowed (true / false)
|
\-----------------------------------------------------------------------------*/
CKartinaClnt::CKartinaClnt(const QString &host, const QString &usr, const QString &pw, bool bAllowErotic) :QHttp(host)
{
   sUsr           = usr;
   sPw            = pw;
   iReq           = -1;
   bEros          = bAllowErotic;
   sCookie        = "";
   sHost          = host;
   eReq           = Kartina::REQ_UNKNOWN;
   bRenewCookie   = true;

   bufReq.open(QIODevice::WriteOnly);

   connect(this, SIGNAL(requestFinished(int, bool)), this, SLOT(handleEndRequest(int, bool)));
   connect(this, SIGNAL(responseHeaderReceived(QHttpResponseHeader)), this, SLOT(getResponseHeader(QHttpResponseHeader)));
}

/*-----------------------------------------------------------------------------\
| Function:    CKartinaClnt / constructor
|
| Author:      Joerg Neubert
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
   iReq           = -1;
   bEros          = false;
   sCookie        = "";
   sHost          = "";
   eReq           = Kartina::REQ_UNKNOWN;
   bRenewCookie   = true;

   bufReq.open(QIODevice::WriteOnly);

   connect(this, SIGNAL(requestFinished(int, bool)), this, SLOT(handleEndRequest(int, bool)));
   connect(this, SIGNAL(responseHeaderReceived(QHttpResponseHeader)), this, SLOT(getResponseHeader(QHttpResponseHeader)));
}

/*-----------------------------------------------------------------------------\
| Function:    ~CKartinaClnt / destructor
|
| Author:      Joerg Neubert
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
| Author:      Joerg Neubert
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: set communication parameter to communicate with
|              kartina.tv
| Parameters:  host, username, password, erotic allowed (true / false)
|
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetData(const QString &host, const QString &usr, const QString &pw, bool bAllowErotic)
{
   sUsr           = usr;
   sPw            = pw;
   bEros          = bAllowErotic;
   sHost          = host;
   sCookie        = "";
   eReq           = Kartina::REQ_UNKNOWN;
   bRenewCookie   = true;

   setHost(host);
}

/*-----------------------------------------------------------------------------\
| Function:    PostRequest
|
| Author:      Joerg Neubert
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: send a post request to kartina.tv
|
| Parameters:  request type, path on server, request string
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::PostRequest (Kartina::EReq req, const QString &path, const QString &content)
{
   eReq = req;
   QHttpRequestHeader header("POST", path.toAscii());
   header.addValue("Host", sHost);
   header.setContentType("application/x-www-form-urlencoded");
   header.addValue("Date", CHttpTime::GetHttpTime());
   header.addValue("User-Agent", "Mozilla/5.0");
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

   mInfo(tr("Request #%1 postet.").arg(iReq));
}

/*-----------------------------------------------------------------------------\
| Function:    GetCookie
|
| Author:      Joerg Neubert
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

   bRenewCookie = true;

   PostRequest(Kartina::REQ_COOKIE, "/",
               QString("act=login&code_login=%1&code_pass=%2").arg(sUsr).arg(sPw));
}

/*-----------------------------------------------------------------------------\
| Function:    GetChannelList
|
| Author:      Joerg Neubert
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

   PostRequest(Kartina::REQ_CHANNELLIST, "/", "m=channels&act=get_list_xml");
}

/*-----------------------------------------------------------------------------\
| Function:    SetTimeShift
|
| Author:      Joerg Neubert
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

   PostRequest(Kartina::REQ_TIMESHIFT, "/",
               QString("m=clients&act=x_set_timeshift&ts=%1").arg(iHours));
}

/*-----------------------------------------------------------------------------\
| Function:    GetStreamURL
|
| Author:      Joerg Neubert
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

   QString req = QString("m=channels&act=get_stream_url&cid=%1").arg(iChanID);

   if (bEros)
   {
      req += QString("&protect_code=%1").arg(sPw);
   }

   PostRequest((bTimerRec) ? Kartina::REQ_TIMERREC : Kartina::REQ_STREAM, "/", req);
}

/*-----------------------------------------------------------------------------\
| Function:    SetServer
|
| Author:      Joerg Neubert
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: set streaming server
|
| Parameters:  server number (1 or 3)
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::SetServer(int iSrv)
{
   mInfo(tr("Set Streaming Server to No %1 ...").arg(iSrv));

   PostRequest(Kartina::REQ_SERVER, "/",
               QString("m=clients&act=x_set_sserv&ssrv=%1").arg(iSrv));
}

/*-----------------------------------------------------------------------------\
| Function:    SetHttpBuffer
|
| Author:      Joerg Neubert
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

   PostRequest(Kartina::REQ_SERVER, "/",
               QString("m=clients&act=x_set_http_cache&htc=%1").arg(iTime));
}

/*-----------------------------------------------------------------------------\
| Function:    GetEPG
|
| Author:      Joerg Neubert
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

   PostRequest(Kartina::REQ_EPG, "/",
               QString("m=epg&act=show_day_xml&day=%1&cid=%2")
               .arg(now.toString("ddMMyy")).arg(iChanID));
}

/*-----------------------------------------------------------------------------\
| Function:    GetArchivURL
|
| Author:      Joerg Neubert
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
   mInfo(tr("Request Archiv URL:\n  --> %1").arg(prepared));

   QString req = prepared;

   if (bEros)
   {
      req += QString("&protect_code=%1").arg(sPw);
   }

   PostRequest(Kartina::REQ_ARCHIV, "/", req);
}

/*-----------------------------------------------------------------------------\
| Function:    getResponseHeader (slot)
|
| Author:      Joerg Neubert
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: catch authentication cookie from response header
|
| Parameters:  ref to header
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CKartinaClnt::getResponseHeader (const QHttpResponseHeader &resp)
{
   if (bRenewCookie)
   {
      mInfo(tr("We've got Response for Authentification!"));

      sCookie      = resp.value("Set-Cookie");

      if (sCookie != "")
      {
         mInfo(tr("Got cookie:\n  --> %1!").arg(sCookie));

         // we have catched the cookie ...
         emit sigGotCookie();
      }
      else
      {
         mErr(tr("Error: Can't authenticate!"));

         emit sigError(tr("Can't authenticate you at %1! Please check username and password!")
                       .arg(COMPANY_NAME));
      }

      bRenewCookie = false;
   }
}

/*-----------------------------------------------------------------------------\
| Function:    handleEndRequest (slot)
|
| Author:      Joerg Neubert
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
            // signal already emitted when cookie was catched from response header ...
            break;
         case Kartina::REQ_CHANNELLIST:
            emit sigGotChannelList(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_TIMESHIFT:
            emit sigTimeShiftSet();
            break;
         case Kartina::REQ_EPG:
            emit sigGotEPG(QString::fromUtf8(baPageContent.constData()));
            break;
         case Kartina::REQ_SERVER:
            emit sigServerChanged();
            break;
         case Kartina::REQ_HTTPBUFF:
            emit sigBufferSet();
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
|  Author: Joerg Neubert
|  Description: is api busy ?
|
|  Parameters: --
|
|  Returns: true ==> busy
|          false ==> available
\----------------------------------------------------------------- */
bool CKartinaClnt::busy ()
{
   return ((eReq != Kartina::REQ_UNKNOWN) || bRenewCookie) ? true : false;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/
