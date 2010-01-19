/*=============================================================================\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/ckartinaclnt.cpp $
|
| Author: Joerg Neubert
|
| last changed by: $Author: joergn $
|
| Begin: Monday, January 04, 2010 16:13:58
|
| $Id: ckartinaclnt.cpp 173 2010-01-18 15:43:19Z joergn $
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
   bEros          = bAllowErotic;
   sCookie        = "";
   sHost          = host;
   eReq           = Kartina::REQ_UNKNOWN;
   bRenewCookie   = true;

   bufReq.open(QIODevice::WriteOnly);

   connect(this, SIGNAL(done(bool)), this, SLOT(handleEndRequest(bool)));
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
   bEros          = false;
   sCookie        = "";
   sHost          = "";
   eReq           = Kartina::REQ_UNKNOWN;
   bRenewCookie   = true;

   bufReq.open(QIODevice::WriteOnly);

   connect(this, SIGNAL(done(bool)), this, SLOT(handleEndRequest(bool)));
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
   mutexBuffer.lock();
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
   request(header, content.toAscii(), &bufReq);
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
   VlcLog.LogInfo(tr("%1 / %2():%3 Request Authentication\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
   VlcLog.LogInfo(tr("%1 / %2():%3 Request Channel List\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));
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
   VlcLog.LogInfo(tr("%1 / %2():%3 Set TimeShift to: %4 hours\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(iHours));
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
void CKartinaClnt::GetStreamURL(int iChanID)
{
   VlcLog.LogInfo(tr("%1 / %2():%3 Request URL for channel %4\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(iChanID));

   QString req = QString("m=channels&act=get_stream_url&cid=%1").arg(iChanID);

   if (bEros)
   {
      req += QString("&protect_code=%1").arg(sPw);
   }

   PostRequest(Kartina::REQ_STREAM, "/", req);
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
   VlcLog.LogInfo(tr("%1 / %2():%3 Set Streaming Server to No %4\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(iSrv));
   PostRequest(Kartina::REQ_SERVER, "/",
               QString("m=clients&act=x_set_sserv&ssrv=%1").arg(iSrv));
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
   VlcLog.LogInfo(tr("%1 / %2():%3 Request EPG for Channel %4\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(iChanID));

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
   VlcLog.LogInfo(tr("%1 / %2():%3 Request Archiv URL:\n%4\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                  .arg(prepared));

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
      VlcLog.LogInfo(tr("%1 / %2():%3 We've got Response for Authentification!\n")
                     .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

      sCookie      = resp.value("Set-Cookie");

      if (sCookie != "")
      {
         VlcLog.LogInfo(tr("%1 / %2():%3 Got cookie: %4!\n")
                        .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(sCookie));

         // we have catched the cookie ...
         emit sigGotCookie();
      }
      else
      {
         VlcLog.LogErr(tr("%1 / %2():%3 Error: Can't authenticate!\n")
                        .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));
         emit sigError(tr("Can't authenticate you at kartina.tv! Please check username and password!"));
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
void CKartinaClnt::handleEndRequest(bool err)
{
   // close buffer device and open for read only...
   bufReq.close();
   bufReq.open(QIODevice::ReadOnly);

   // read all content ...
   baPageContent = bufReq.readAll();

   // close buffer device and open for write only...
   bufReq.close();
   bufReq.open(QIODevice::WriteOnly | QIODevice::Truncate);
   mutexBuffer.unlock();

   if (!err)
   {
      VlcLog.LogInfo(tr("%1 / %2():%3 Request done!\n")
                     .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
      case Kartina::REQ_STREAM:
         emit sigGotStreamURL(QString::fromUtf8(baPageContent.constData()));
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
      VlcLog.LogErr(tr("%1 / %2():%3 Error in Request: %4!\n")
                     .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(errorString()));

      // send error signal ...
      emit sigError(errorString());
   }

   // mark request as ended so the API is "free for use" again ...
   eReq = Kartina::REQ_UNKNOWN;
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
