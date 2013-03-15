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
   // check for error ...
   if (reply->error() == QNetworkReply::NoError)
   {
      // no error ...
      cookies = reply->header(QNetworkRequest::SetCookieHeader);

      // get request id ...
      int iReqId    = reply->property(PROP_ID).toInt();
      int iRespType = reply->property(PROP_TYPE).toInt();
      QByteArray ba = reply->readAll();

#ifdef __TRACE
   mInfo(tr("iReqID=%1, RespType=%2, DataSize=%3B")
         .arg(iReqId).arg(iRespType).arg(ba.size()));
#endif // __TRACE

      // What kind of reply is this?
      switch((Iptv::eRespType)iRespType)
      {
      case Iptv::E_RESP_STRING:
         emit sigStringResponse(iReqId, QString::fromUtf8(ba.constData()));
         break;

      case Iptv::E_RESP_BINARY:
         emit sigBinResponse(iReqId, ba);
         break;

      default:
         emit sigErr(tr("Error, unknown response type: %1!")
                     .arg(iRespType), -1);
         break;
      }
   }
   else
   {
      emit sigErr(reply->errorString(), (int)reply->error());
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
   req.setHeader(QNetworkRequest::CookieHeader, cookies);

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
//! \param   eResp (Iptv::eRespType) assumed response type
//
//! \return  pointer to QNetworkReply as given as param
//---------------------------------------------------------------------------
QNetworkReply* QIptvCtrlClient::prepareReply(QNetworkReply* rep, int iReqId, Iptv::eRespType eResp)
{
   rep->setProperty(PROP_TYPE, (int)eResp);
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
//! \param   eResp (Iptv::eRespType) assumed response type
//
//! \return  pointer to network reply
//---------------------------------------------------------------------------
QNetworkReply* QIptvCtrlClient::post(int iReqId, const QString& url,
                                     const QString& content,
                                     Iptv::eRespType eResp)
{
   QNetworkReply*  pReply;
   QNetworkRequest req;
   prepareRequest(req, url);

#ifdef __TRACE
   mInfo(tr("iReqID=%1, RespType=%2, Url='%3', Content='%4'")
         .arg(iReqId).arg((int)eResp).arg(url).arg(content));
#endif // __TRACE

   if ((pReply = QNetworkAccessManager::post(req, content.toAscii())) != NULL)
   {
      prepareReply(pReply, iReqId, eResp);
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
//! \param   eResp (Iptv::eRespType) assumed response type
//
//! \return  pointer to network reply
//---------------------------------------------------------------------------
QNetworkReply* QIptvCtrlClient::get(int iReqId, const QString& url,
                                    Iptv::eRespType eResp)
{
   QNetworkReply*  pReply;
   QNetworkRequest req;
   prepareRequest(req, url);

#ifdef __TRACE
   mInfo(tr("iReqID=%1, RespType=%2, Url='%3'")
         .arg(iReqId).arg((int)eResp).arg(url));
#endif // __TRACE

   if ((pReply = QNetworkAccessManager::get(req)) != NULL)
   {
      prepareReply(pReply, iReqId, eResp);
   }

   return pReply;
}
