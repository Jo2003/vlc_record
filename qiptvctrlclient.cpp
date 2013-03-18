/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qiptvctrlclient.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.03.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qiptvctrlclient.h"
#include "version_info.h"
#include "defdef.h"

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   constructs QIptvCtrlClient object
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   parent pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
QIptvCtrlClient::QIptvCtrlClient(QObject* parent) :
   QNetworkAccessManager(parent)
{
   bCSet = false;
   connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotResponse(QNetworkReply*)));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QIptvCtrlClient object
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QIptvCtrlClient::~QIptvCtrlClient()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   handle network response
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   reply (QNetworkReply*) pointer to received reply
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::slotResponse(QNetworkReply* reply)
{
   // get id and type from reply ...
   int iReqId   = reply->property(PROP_ID).toInt();
   int iReqType = reply->property(PROP_TYPE).toInt();

   // check for error ...
   if (reply->error() == QNetworkReply::NoError)
   {
      // get data ...
      QByteArray ba = reply->readAll();

#ifdef __TRACE
      mInfo(tr("id=%1, type='%2', size=%3 bytes")
         .arg(iReqId).arg(iptv.reqName((Iptv::eReqType)iReqType)).arg(ba.size()));
#endif // __TRACE

      // What kind of reply is this?
      switch((Iptv::eReqType)iReqType)
      {
      case Iptv::Login:
      case Iptv::Logout:
      case Iptv::String:

         // decide if we want to clear or set session cookie ...
         if ((Iptv::eReqType)iReqType == Iptv::Login)
         {
            bCSet   = true;
            cookies = reply->header(QNetworkRequest::SetCookieHeader);
         }
         else if ((Iptv::eReqType)iReqType == Iptv::Logout)
         {
            bCSet   = false;
            cookies = QVariant();
         }

         emit sigStringResponse(iReqId, QString::fromUtf8(ba.constData()));
         break;

      case Iptv::Binary:
         emit sigBinResponse(iReqId, ba);
         break;

      case Iptv::Stream:
         // nothing to do here ...
         break;

      default:
         emit sigErr(iReqId, tr("Error, unknown request type: %1!")
                     .arg(iReqType), -1);
         break;
      }
   }
   else
   {
      emit sigErr(iReqId, reply->errorString(), (int)reply->error());
   }

   // mark for deletion ...
   reply->deleteLater();
}

//---------------------------------------------------------------------------
//
//! \brief   prepare network request
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   req (QNetworkRequest&) ref. to request
//! \param   url (const QString&) url used for the post request
//! \param   iSize (int) optional, content size in bytes (when using post)
//
//! \return  ref. to QNetworkRequest as given as param
//---------------------------------------------------------------------------
QNetworkRequest &QIptvCtrlClient::prepareRequest(QNetworkRequest& req,
                                                 const QString& url,
                                                 int iSize)
{
   // set request url ...
   req.setUrl(QUrl(url));

   // set user agent (name + version of this app) ...
   req.setRawHeader("User-Agent"  , APP_NAME " " __MY__VERSION__);

   // set content type ...
   req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

   // set content length if given (used on post requests only) ...
   if (iSize != -1)
   {
      req.setHeader(QNetworkRequest::ContentLengthHeader, iSize);
   }

   // ...
   // Here is the place to add more header data ...
   // ...

   // set cookie if already catched ...
   if (bCSet)
   {
      req.setHeader(QNetworkRequest::CookieHeader, cookies);
   }

   return req;
}

//---------------------------------------------------------------------------
//
//! \brief   prepare network reply: set properties so we recognize response
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   rep (QNetworkReply*) ref. to reply
//! \param   iReqId (int) request identifier
//! \param   t_req (Iptv::eReqType) type of request
//
//! \return  pointer to QNetworkReply as given as param
//---------------------------------------------------------------------------
QNetworkReply* QIptvCtrlClient::prepareReply(QNetworkReply* rep, int iReqId, Iptv::eReqType t_req)
{
   rep->setProperty(PROP_TYPE, (int)t_req);
   rep->setProperty(PROP_ID,  iReqId);
   return rep;
}

//---------------------------------------------------------------------------
//
//! \brief   send a post request
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   iReqId (int) request identifier
//! \param   url (const QString&) url used for the post request
//! \param   content (const QString&) data to be posted
//! \param   t_req (Iptv::eReqType) type of request
//
//! \return  pointer to network reply
//---------------------------------------------------------------------------
QNetworkReply* QIptvCtrlClient::post(int iReqId, const QString& url,
                                     const QString& content,
                                     Iptv::eReqType t_req)
{
   QNetworkReply*  pReply;
   QNetworkRequest req;
   QUrl            data(content);

   prepareRequest(req, url, data.toEncoded().size());

#ifdef __TRACE
   mInfo(tr("id=%1, type='%2', url='%3', data='%4'")
         .arg(iReqId).arg(iptv.reqName(t_req)).arg(url).arg(content));
#endif // __TRACE

   if ((pReply = QNetworkAccessManager::post(req, data.toEncoded())) != NULL)
   {
      prepareReply(pReply, iReqId, t_req);
   }

   return pReply;
}

//---------------------------------------------------------------------------
//
//! \brief   send a get request
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   iReqId (int) request identifier
//! \param   url (const QString&) url used for the get request
//! \param   t_req (Iptv::eReqType) type of request
//
//! \return  pointer to network reply
//---------------------------------------------------------------------------
QNetworkReply* QIptvCtrlClient::get(int iReqId, const QString& url,
                                    Iptv::eReqType t_req)
{
   QNetworkReply*  pReply;
   QNetworkRequest req;

   prepareRequest(req, url);

#ifdef __TRACE
   mInfo(tr("id=%1, type='%2', url='%3'")
         .arg(iReqId).arg(iptv.reqName(t_req)).arg(url));
#endif // __TRACE

   if ((pReply = QNetworkAccessManager::get(req)) != NULL)
   {
      prepareReply(pReply, iReqId, t_req);
   }

   return pReply;
}
