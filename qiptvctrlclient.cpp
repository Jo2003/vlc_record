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
   int iReqId   = reply->property(PROP_ID).toInt();
   int iReqType = reply->property(PROP_TYPE).toInt();
   QByteArray ba;

   // check for error ...
   if (reply->error() == QNetworkReply::NoError)
   {
      // get data ...
      ba = reply->readAll();

#ifdef __TRACE
      mInfo(tr("iReqID=%1, ReqType='%2', DataSize=%3B")
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
         emit sigErr(iReqId, tr("Error, unknown reqest type: %1!")
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
//
//! \return  ref. to QNetworkRequest as given as param
//---------------------------------------------------------------------------
QNetworkRequest &QIptvCtrlClient::prepareRequest(QNetworkRequest& req,
                                                 const QString& url)
{
   req.setUrl(QUrl(url));
   req.setRawHeader("User-Agent", APP_NAME " " __MY__VERSION__);

   // ...
   // Here is the possibility to add more header data as well ...
   // ...

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
   prepareRequest(req, url);

#ifdef __TRACE
   mInfo(tr("iReqID=%1, ReqType='%2', Url='%3', Content='%4'")
         .arg(iReqId).arg(iptv.reqName(t_req)).arg(url).arg(content));
#endif // __TRACE

   if ((pReply = QNetworkAccessManager::post(req, content.toAscii())) != NULL)
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
   mInfo(tr("iReqID=%1, ReqType='%2', Url='%3'")
         .arg(iReqId).arg(iptv.reqName(t_req)).arg(url));
#endif // __TRACE

   if ((pReply = QNetworkAccessManager::get(req)) != NULL)
   {
      prepareReply(pReply, iReqId, t_req);
   }

   return pReply;
}
