/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qiptvctrlclient.cpp $
 *
 *  @file     qiptvctrlclient.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.03.2013
 *
 *  $Id: qiptvctrlclient.cpp 1495 2015-02-20 13:48:22Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qiptvctrlclient.h"
#include "version_info.h"
#include "defdef.h"
#include "qcustparser.h"
#include "ciptvdefs.h"
#include "small_helpers.h"

// global customization class ...
extern QCustParser *pCustomization;

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
   bBusy = false;
   connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotResponse(QNetworkReply*)));

   mLastRequest.iReqId       = -1;
   mLastRequest.eHttpReqType = E_REQ_UNKN;
   mLastLogin                = mLastRequest;
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
      // check for redirect ...
      if (reply->hasRawHeader("Location"))
      {
          QString redir = QString(reply->rawHeader("Location"));
          mInfo(tr("Redirect to: '%1'").arg(redir));

          get(iReqId, redir, (Iptv::eReqType)iReqType);

          // mark for deletion ...
          reply->deleteLater();

          // leave function ...
          return;
      }

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

      case Iptv::m3u:
         emit sigM3u (iReqId, QString::fromUtf8(ba.constData()));
         break;

      case Iptv::hls:
         emit sigHls (iReqId, ba);
         break;

      case Iptv::noop:
#ifdef __TRACE
         mInfo(tr("Response:\n ==8<==8<==8<==\n%1\n ==>8==>8==>8==")
            .arg(QString(ba)));
#endif // __TRACE
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
      QString sErr = reply->errorString();
#ifdef _IS_OEM
      // in case of OEM we should remove the API server string from
      // error messages ...
      QUrl    sUrl = reply->request().url();

      sErr.remove(sUrl.host());
      sErr = sErr.simplified();
#endif // _IS_OEM

      mInfo(tr("Network response error #%1:\n  --> %2").arg((int)reply->error()).arg(sErr));
      emit sigErr(iReqId, sErr, (int)reply->error());
   }

   // mark for deletion ...
   reply->deleteLater();

   // request handled ...
   bBusy = false;

   // check for new requests ...
   workOffQueue();
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

   if (sStbSerial.isEmpty())
   {
       generateStbSerial();
   }

   // set user agent (name + version of this app) ...
   req.setRawHeader("User-Agent", QString("%1 %2-%3")
                                      .arg(pCustomization->strVal("APP_NAME"))
                                      .arg(__MY__VERSION__)
                                      .arg(sStbSerial).toUtf8());

   // no persistent connections ...
   req.setRawHeader("Connection", "close");

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

//---------------------------------------------------------------------------
//
//! \brief   queue in a post request
//
//! \author  Jo2003
//! \date    05.06.2013
//
//! \param   iReqId (int) request identifier
//! \param   url (const QString&) url used for the get request
//! \param   content (const QString&) data to be posted
//! \param   t_req (Iptv::eReqType) type of request
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::q_post(int iReqId, const QString& url, const QString& content, Iptv::eReqType t_req)
{
   SRequest reqObj;
   reqObj.eHttpReqType = E_REQ_POST;
   reqObj.eIptvReqType = t_req;
   reqObj.iReqId       = iReqId;
   reqObj.sContent     = content;
   reqObj.sUrl         = url;

   // add request to command queue ...
   mtxCmdQueue.lock();
   vCmdQueue.append(reqObj);
   mtxCmdQueue.unlock();

   // try to handle request ...
   workOffQueue();
}

//---------------------------------------------------------------------------
//
//! \brief   queue in a get request
//
//! \author  Jo2003
//! \date    05.06.2013
//
//! \param   iReqId (int) request identifier
//! \param   url (const QString&) url used for the get request
//! \param   t_req (Iptv::eReqType) type of request
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::q_get(int iReqId, const QString& url, Iptv::eReqType t_req)
{
   SRequest reqObj;
   reqObj.eHttpReqType = E_REQ_GET;
   reqObj.eIptvReqType = t_req;
   reqObj.iReqId       = iReqId;
   reqObj.sContent     = "";
   reqObj.sUrl         = url;

   // add request to command queue ...
   mtxCmdQueue.lock();
   vCmdQueue.append(reqObj);
   mtxCmdQueue.unlock();

   // try to handle request ...
   workOffQueue();
}

//---------------------------------------------------------------------------
//
//! \brief   handle next request stored in queue if not busy
//
//! \author  Jo2003
//! \date    05.06.2013
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::workOffQueue()
{
   SRequest reqObj;
   reqObj.eHttpReqType = E_REQ_UNKN;

   // pending requests ... ?
   if (!bBusy)
   {
      // something to do ... ?
      mtxCmdQueue.lock();
      if (!vCmdQueue.isEmpty())
      {
         reqObj = vCmdQueue.first();
         vCmdQueue.remove(0);
      }

      // store as last request ...
      if (mLastRequest != reqObj)
      {
          mLastRequest = reqObj;

          // buffer last login ...
          if ((mLastRequest.eIptvReqType == Iptv::Login)
             && (mLastRequest.iReqId != (int)CIptvDefs::REQ_LOGIN_ONLY))
          {
             mLastLogin        = mLastRequest;
             mLastLogin.iReqId = (int)CIptvDefs::REQ_LOGIN_ONLY;
          }
      }

      if (reqObj.eHttpReqType == E_REQ_POST)
      {
         // handle queued post request ...
         bBusy = true;
         post(reqObj.iReqId, reqObj.sUrl, reqObj.sContent, reqObj.eIptvReqType);
      }
      else if (reqObj.eHttpReqType == E_REQ_GET)
      {
         // handle queued get request ...
         bBusy = true;
         get(reqObj.iReqId, reqObj.sUrl, reqObj.eIptvReqType);
      }
      mtxCmdQueue.unlock();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   requeue last request
//
//! \author  Jo2003
//! \date    29.04.2014
//
//! \param   [in] clearQueue (bool) optioanl clear queue flag
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::requeue(bool clearQueue)
{
   if ((mLastRequest.eHttpReqType != E_REQ_UNKN)
      && (mLastRequest.iReqId != -1))
   {
      // lock queue ...
      mtxCmdQueue.lock();

      // delete all pending requests ...
      if (clearQueue)
      {
          vCmdQueue.clear();
      }

      mInfo(tr("Append last sent request (which triggered error) ..."));

      // add request to command queue ...
      vCmdQueue.append(mLastRequest);

      mtxCmdQueue.unlock();

      // try to handle request ...
      workOffQueue();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   requeue last request
//
//! \author  Jo2003
//! \date    29.04.2014
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::reLogin()
{
   if ((mLastLogin.eHttpReqType != E_REQ_UNKN)
      && (mLastLogin.iReqId != -1))
   {
      // lock queue ...
      mtxCmdQueue.lock();

      // delete all pending requests ...
      vCmdQueue.clear();

      mInfo(tr("Append re-Login request ..."));

      // add request to command queue ...
      vCmdQueue.append(mLastLogin);

      mtxCmdQueue.unlock();

      // try to handle request ...
      workOffQueue();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   create STB serial from MAC address of ethernet interfaces
//
//! \author  Jo2003
//! \date    04.06.2014
//
//---------------------------------------------------------------------------
void QIptvCtrlClient::generateStbSerial()
{
    QString     tmpKey;
    QStringList macList;
    sStbSerial.clear();

    /// 1. get all interfaces, but loopback
    /// 2. check if hardware address is a valid MAC address
    /// 3. add to macList
    /// 4. sort macList
    /// 5. join macList
    /// 6. create md5 hash from joined macList
    /// Also in case the network adapter order should change,
    /// the cli_serial should be the same.
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if (!interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            tmpKey = interface.hardwareAddress();

            if (tmpKey.split(":").count() == 6)
            {
                mInfo(tr("Interface: '%1'; MAC: %2")
                    .arg(interface.name())
                    .arg(interface.hardwareAddress()));

                macList.append(tmpKey);
            }
        }
    }

    if (!macList.isEmpty())
    {
        macList.sort();
        tmpKey = macList.join(":");
        sStbSerial = CSmallHelpers::md5(tmpKey);
        mInfo(tr("cli_serial '%2' from source '%1' created ...").arg(tmpKey).arg(sStbSerial));
    }
}
