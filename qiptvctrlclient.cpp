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
#include "ciptvdefs.h"
#include "version_info.h"
#include "defdef.h"
#include "externals_inc.h"
#include "small_helpers.h"

/// Gray my friend, is all theory ...
/// We can't count that the QNetworkConfigurationManager
/// will tell about all the network changes. E.g. we won't get
/// the information when a network interface goes down
/// due to energy control.
/// So all we can do is to check for all errors which may appear
/// on reply ... and we could check for timeout.
/// All this is dirty stuff and might not be very helpful at all.

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
   // we need this for online state ...
   _pNetConfMgr = new QNetworkConfigurationManager (this);
   bCSet        = false;
   bOnline      = false;
   ulReqNo      = 0;
   ulAckNo      = 0;

   tWatchdog.setSingleShot(true);
   tWatchdog.setInterval(HTTP_REQ_TIMEOUT);

   // connection check once a minute ...
   tConncheck.setSingleShot(true);

   connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotResponse(QNetworkReply*)));
   connect(this, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
           this, SLOT(slotAccessibilityChgd(QNetworkAccessManager::NetworkAccessibility)));

   connect(_pNetConfMgr, SIGNAL(configurationChanged(QNetworkConfiguration)), this, SLOT(configChgd(QNetworkConfiguration)));
   connect(&tWatchdog, SIGNAL(timeout()), this, SLOT(slotReqTmout()));
   connect(&tConncheck, SIGNAL(timeout()), this, SLOT(startConnectionCheck()));
   connect(this, SIGNAL(sigStateMessage(int,QString,int)), pStateMsg, SLOT(showMessage(int,QString,int)));
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
   uint ack     = reply->property(PROP_REQ_NO).toUInt();

   // make sure "old" responses will not break chain ...
   if (ack > ulAckNo)
   {
      ulAckNo = ack;
   }

   // a reply anyway ...
   tWatchdog.stop();

   // check for error ...
   if (reply->error() == QNetworkReply::NoError)
   {
      // error free reply means we're online anyway ...
      setOnline(true);

      // get data ...
      QByteArray ba = reply->readAll();

#ifdef __TRACE
      mInfo(tr("No.#%1, id=%2, type='%3', size=%4 bytes")
            .arg(ulAckNo).arg(iReqId)
            .arg(iptv.reqName((Iptv::eReqType)iReqType))
            .arg(ba.size()));
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

      case Iptv::chkconn:
      case Iptv::Stream:
         // nothing to do here ...
         break;

      default:
         emit sigApiErr(iReqId, tr("Error, unknown request type: %1!")
                     .arg(iReqType), -1);
         break;
      }
   }
   else
   {
      // Oops ... error!
      // We might be offline ...?!
      setOnline(stillOnlineOnError(reply->error()));

      if ((Iptv::eReqType)iReqType != Iptv::chkconn)
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

         QTimer::singleShot(1000, this, SLOT(startConnectionCheck()));

         // an error while downloading an image might appear ...
         if (((Iptv::eReqType)iReqType == Iptv::Binary)
            && ((CIptvDefs::EReq)iReqId == CIptvDefs::REQ_DOWN_IMG))
         {
            mInfo(tr("Keeping image download chain intact ..."));
            emit sigBinResponse(iReqId, QByteArray());
         }
      }
   }

   // mark for deletion ...
   reply->deleteLater();

   // check for new requests ...
   workOffQueue(__FUNCTION__);
}

//---------------------------------------------------------------------------
//
//! \brief   check response error, return if we're still online
//
//! \author  Jo2003
//! \date    07.08.2014
//
//! \param   err (QNetworkReply::NetworkError) response error
//
//! \return  true -> we're still online
//---------------------------------------------------------------------------
bool QIptvCtrlClient::stillOnlineOnError(QNetworkReply::NetworkError err)
{
   bool ret = false;

   switch (err)
   {
   case QNetworkReply::ContentOperationNotPermittedError:
   case QNetworkReply::ContentNotFoundError:
   case QNetworkReply::ContentAccessDenied:
   case QNetworkReply::AuthenticationRequiredError:
      ret = true;
      break;

   default:
      break;
   }

   return ret;
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
//! \param   json [in] (bool) is this json request
//
//! \return  ref. to QNetworkRequest as given as param
//---------------------------------------------------------------------------
QNetworkRequest &QIptvCtrlClient::prepareRequest(QNetworkRequest& req,
                                                 const QString& url,
                                                 int iSize, bool json)
{
   // set request url ...
   req.setUrl(QUrl(url));

   // set user agent (name + version of this app) ...
   req.setRawHeader("User-Agent", QString("%1 %2").arg(pCustomization->strVal("APP_NAME")).arg(__MY__VERSION__).toUtf8());

   // no persistent connections ...
   req.setRawHeader("Connection", "close");

   /*
   Kartina APIv2 needs this parameter not in header, but as query item in QUrl!

   if (sStbSerial.isEmpty())
   {
      generateStbSerial();
   }

   if (!sStbSerial.isEmpty())
   {
      // set stb serial ...
      req.setRawHeader("STB_SERIAL", sStbSerial.toUtf8());
   }
   */

   // set content type ...
   if (json)
   {
      req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
   }
   else
   {
      req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
   }

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
   rep->setProperty(PROP_TYPE,   (int)t_req);
   rep->setProperty(PROP_ID,     iReqId);
   rep->setProperty(PROP_REQ_NO, (uint)++ulReqNo);
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
//! \param   json [in] (bool) conent is json
//
//! \return  pointer to network reply
//---------------------------------------------------------------------------
QNetworkReply* QIptvCtrlClient::post(int iReqId, const QString& url,
                                     const QString& content,
                                     Iptv::eReqType t_req, bool json)
{
   QNetworkReply*  pReply;
   QNetworkRequest req;
   QUrl            data(content);

   // buffer request ...
   lastRequest.eHttpReqType = E_REQ_POST;
   lastRequest.eIptvReqType = t_req;
   lastRequest.iReqId       = iReqId;
   lastRequest.sContent     = content;
   lastRequest.sUrl         = url;
   lastRequest.json         = json;

   if ((t_req == Iptv::Login)
       && (lastRequest.iReqId != (int)CIptvDefs::REQ_LOGIN_ONLY))
   {
      lastLogin        = lastRequest;
      lastLogin.iReqId = (int)CIptvDefs::REQ_LOGIN_ONLY;
   }

   if (json)
   {
      prepareRequest(req, url, content.toUtf8().size(), json);
   }
   else
   {
      prepareRequest(req, url, data.toEncoded().size());
   }



#ifdef __TRACE
   mInfo(tr("id=%1, type='%2', url='%3', data='%4'")
         .arg(iReqId).arg(iptv.reqName(t_req)).arg(url).arg(content));
#endif // __TRACE

   if (json)
   {
      pReply = QNetworkAccessManager::post(req, content.toUtf8());
   }
   else
   {
      pReply = QNetworkAccessManager::post(req, data.toEncoded());
   }

   if (pReply != NULL)
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

   // buffer request ...
   lastRequest.eHttpReqType = E_REQ_GET;
   lastRequest.eIptvReqType = t_req;
   lastRequest.iReqId       = iReqId;
   lastRequest.sContent     = "";
   lastRequest.sUrl         = url;
   lastRequest.json         = false;

   if ((t_req == Iptv::Login)
      && (lastRequest.iReqId != (int)CIptvDefs::REQ_LOGIN_ONLY))
   {
      lastLogin        = lastRequest;
      lastLogin.iReqId = (int)CIptvDefs::REQ_LOGIN_ONLY;
   }

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
//! \param   isJson [in] (bool) is request a json string?
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::q_post(int iReqId, const QString& url, const QString& content, Iptv::eReqType t_req, bool isJson)
{
   SRequest reqObj;
   reqObj.eHttpReqType = E_REQ_POST;
   reqObj.eIptvReqType = t_req;
   reqObj.iReqId       = iReqId;
   reqObj.sContent     = content;
   reqObj.sUrl         = url;
   reqObj.uiTimeStamp  = QDateTime::currentDateTime().toTime_t();
   reqObj.json         = isJson;

   // add request to command queue ...
   mtxCmdQueue.lock();
   vCmdQueue.append(reqObj);
   mtxCmdQueue.unlock();

   // try to handle request ...
   workOffQueue(__FUNCTION__);
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
   reqObj.uiTimeStamp  = QDateTime::currentDateTime().toTime_t();
   reqObj.json         = false;

   // add request to command queue ...
   mtxCmdQueue.lock();
   vCmdQueue.append(reqObj);
   mtxCmdQueue.unlock();

   // try to handle request ...
   workOffQueue(__FUNCTION__);
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
void QIptvCtrlClient::workOffQueue(const QString& caller)
{
   SRequest reqObj;
   reqObj.eHttpReqType = E_REQ_UNKN;

#ifdef __TRACE
   if (!caller.isEmpty())
   {
      mInfo(tr("called from %1(), busy(): %2, isOnline(): %3")
            .arg(caller).arg(busy()).arg(isOnline()));
   }
#else
   Q_UNUSED(caller)
#endif // __TRACE

   // pending requests ... ?
   if (!busy() && isOnline())
   {
      // something to do ... ?
      mtxCmdQueue.lock();
      while (!vCmdQueue.isEmpty())
      {
         reqObj = vCmdQueue.first();
         vCmdQueue.remove(0);

         // how old is this request ... ?
         // all older than 2 minutes we can forget ...
         if ((reqObj.uiTimeStamp + 120) < QDateTime::currentDateTime().toTime_t())
         {
            mInfo(tr("Ignore old queued request ..."));

            // reset type to avoid a request ...
            reqObj.eHttpReqType = E_REQ_UNKN;
         }
         else
         {
            // entry is fresh enough ...
            break;
         }
      }

      if (reqObj.eHttpReqType == E_REQ_POST)
      {
         // handle queued post request; set busy state when call succeeds  ...
         post(reqObj.iReqId, reqObj.sUrl, reqObj.sContent, reqObj.eIptvReqType, reqObj.json);
      }
      else if (reqObj.eHttpReqType == E_REQ_GET)
      {
         // handle queued get request; set busy state when call succeeds  ...
         get(reqObj.iReqId, reqObj.sUrl, reqObj.eIptvReqType);
      }

      // check for timeout ...
      if (busy())
      {
         tWatchdog.start();
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
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::requeue(bool withLogin)
{
   if ((lastRequest.eHttpReqType != E_REQ_UNKN)
      && (lastRequest.iReqId != -1))
   {
      // lock queue ...
      mtxCmdQueue.lock();

      // delete all pending requests ...
      vCmdQueue.clear();

      // should we prepend something (e.g. login)?
      if (withLogin)
      {
         // add request to command queue ...
         lastLogin.uiTimeStamp = QDateTime::currentDateTime().toTime_t();

         mInfo(tr("Prepend login request ..."));

         vCmdQueue.append(lastLogin);
      }

      // add request to command queue ...
      lastRequest.uiTimeStamp = QDateTime::currentDateTime().toTime_t();

      mInfo(tr("Append last sent request (which triggered error) ..."));

      vCmdQueue.append(lastRequest);

      mtxCmdQueue.unlock();

      // try to handle request ...
      workOffQueue(__FUNCTION__);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   to be called if online state changes
//
//! \author  Jo2003
//! \date    03.06.2014
//
//! \param   o [in] (bool) online flag
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::setOnline(bool o)
{
   if (o != bOnline)
   {
      int     state = o ? (int)QStateMessage::S_INFO            : (int)QStateMessage::S_ERROR;
      QString msg   = o ? tr("Network connection established!") : tr("Error connecting to network!");
      int     tmout = o ? 1000                                  : 5000;
      mInfo(tr("Online state changed: %1 --> %2").arg(bOnline).arg(o));
      emit sigStateMessage(state, pHtml->htmlTag("b", msg), tmout);
      bOnline = o;

      if (bOnline)
      {
         // make sure request counter are synchronized ...
         mInfo(tr("Synchronize request counter and acknowledge: %1 <--> %2").arg(ulAckNo).arg(ulReqNo));
         ulAckNo = ulReqNo;
      }
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


//---------------------------------------------------------------------------
//
//! \brief   slot to be called if network config changes
//
//! \author  Jo2003
//! \date    04.06.2014
//
//! \param   config [in] (const QNetworkConfiguration&) changed config
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::configChgd(const QNetworkConfiguration& config)
{
    if (config == activeConfiguration())
    {
        mInfo(tr("network config '%1', id '%2' changed; state %3 -> check internet connection!")
              .arg(config.name()).arg(config.identifier())
              .arg(config.state()));

        if (!config.state().testFlag(QNetworkConfiguration::Active))
        {
            // one network was set down ...
            // make sure we send nothing as long we aren't sure
            // the connection to the internet is up and running ...
            setOnline(false);
        }

        // give a little time before sending request ...
        QTimer::singleShot(1000, this, SLOT(startConnectionCheck()));
    }
#ifdef __TRACE
    else
    {
        mInfo(tr("Unused(!) network config '%1', id '%2' changed to state %3")
              .arg(config.name()).arg(config.identifier())
              .arg(config.state()));
    }
#endif // __TRACE
}

//---------------------------------------------------------------------------
//
//! \brief   network accessibility changed -> set online state
//
//! \author  Jo2003
//! \date    04.06.2014
//
//! \param   acc [in] (QNetworkAccessManager::NetworkAccessibility) new state
//
//! \return  --
//---------------------------------------------------------------------------
void QIptvCtrlClient::slotAccessibilityChgd(QNetworkAccessManager::NetworkAccessibility acc)
{
   mInfo(tr("Network accessibility changed: %1").arg((int)acc));

   switch (acc)
   {
   case QNetworkAccessManager::NotAccessible:
      setOnline(false);
      break;

   case QNetworkAccessManager::Accessible:
      // give a little time before sending request ...
      QTimer::singleShot(1000, this, SLOT(startConnectionCheck()));
      break;

   default:
      // nothing to do ...
      break;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   return online state
//
//! \author  Jo2003
//! \date    03.06.2014
//
//! \return  true -> online, false -> offline
//---------------------------------------------------------------------------
bool QIptvCtrlClient::isOnline()
{
   return bOnline;
}

//---------------------------------------------------------------------------
//
//! \brief   return busy state
//
//! \author  Jo2003
//! \date    01.08.2014
//
//! \return  true -> busy, false -> available
//---------------------------------------------------------------------------
bool QIptvCtrlClient::busy()
{
   return (ulAckNo != ulReqNo);
}

//---------------------------------------------------------------------------
//
//! \brief   return stb serial
//
//! \author  Jo2003
//! \date    01.08.2014
//
//! \return  serial string
//---------------------------------------------------------------------------
const QString &QIptvCtrlClient::getStbSerial()
{
   if (sStbSerial.isEmpty())
   {
      generateStbSerial();
   }

   return sStbSerial;
}

//---------------------------------------------------------------------------
//
//! \brief   check if we're connected to the internet
//
//! \author  Jo2003
//! \date    04.06.2014
//
//---------------------------------------------------------------------------
void QIptvCtrlClient::startConnectionCheck()
{
   int tmOut;

   if (!isOnline())
   {
      QNetworkReply  *pRep;

      // use API URL for connection check ...
      QUrl url(QString("%1%2").arg(sApiUrl).arg("getinfo?info=servertime"));

      mInfo(tr("Start connection check: %1").arg(url.toString()));

      QNetworkRequest req(url);

      // no persistent connections ...
      req.setRawHeader("Connection", "close");

      // try to get site ...
      if ((pRep = QNetworkAccessManager::get(req)) != NULL)
      {
         // mark reply as connection check ...
         prepareReply(pRep, (int)CIptvDefs::REQ_UNKNOWN, Iptv::chkconn);
      }

      // if not connected check online state every 10 seconds ...
      tmOut = 10 * 1000;
   }
   else
   {
      // when connected every 60 seconds ...
      tmOut = 60 * 1000;
   }

   // (re-) trigger connection check ...
   tConncheck.start(tmOut);
}

//---------------------------------------------------------------------------
//
//! \brief   http request timed out, set online state to off
//
//! \author  Jo2003
//! \date    01.08.2014
//
//---------------------------------------------------------------------------
void QIptvCtrlClient::slotReqTmout()
{
   mInfo(tr("http request timed out!"));
   setOnline(false);
}
