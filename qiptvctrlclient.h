/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qiptvctrlclient.h
 *
 *  @author   Jo2003
 *
 *  @date     15.03.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130315_QIPTVCTRLCLIENT_H
   #define __20130315_QIPTVCTRLCLIENT_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QVariant>
#include <QMetaEnum>
#include <QUrl>
#include <QVector>
#include <QMutex>

#include "clogfile.h"

//---------------------------------------------------------------------------
//! \class   Iptv
//! \date    04.06.2012
//! \author  Jo2003
//! \brief   pseudo class for Qt's Meta Data Handling
//!          (replaces Iptv namespace)
//---------------------------------------------------------------------------
class Iptv : public QObject
{
   Q_OBJECT
   Q_ENUMS(eReqType)

public:

   enum eReqType {
      Login,   // always a string
      Logout,  // always a string
      String,
      Binary,
      Stream,
      m3u,
      hls,
      Unknown
   };

   //---------------------------------------------------------------------------
   //
   //! \brief   get ascii text for enum value
   //
   //! \author  Jo2003
   //! \date    05.06.2012
   //
   //! \param   e eReqType value
   //
   //! \return  ascii string for enum value
   //---------------------------------------------------------------------------
   const char* reqName (eReqType e)
   {
      QMetaEnum eReqEnum = metaObject()->enumerator(metaObject()->indexOfEnumerator("eReqType"));
      return eReqEnum.valueToKey(e);
   }
};

#define PROP_TYPE "type"
#define PROP_ID   "id"

//---------------------------------------------------------------------------
//! \class   QIptvCtrlClient
//! \date    15.03.2013
//! \author  Jo2003
//! \brief   a NetAccManager to handle http requests to the API server
//---------------------------------------------------------------------------
class QIptvCtrlClient : public QNetworkAccessManager
{
   /// Please note!
   /// We have to make sure that requests are
   /// executed in the incoming order. Since
   /// some commands depend on others we only should
   /// start next request when former request was done!
   /// Therefore the command queue and busy flag was
   /// added again. It's implemented into this class
   /// to hide it from the childs. So API client
   /// must not take care about that!

   Q_OBJECT

public:

   /// definitions for command queue ...
   enum EHttpReqType {
      E_REQ_GET,
      E_REQ_POST,
      E_REQ_UNKN = 255
   };

   struct SRequest {
      EHttpReqType   eHttpReqType;
      int            iReqId;
      QString        sUrl;
      QString        sContent;
      Iptv::eReqType eIptvReqType;
   };

   explicit QIptvCtrlClient(QObject* parent = 0);
   virtual ~QIptvCtrlClient();

   virtual void q_post(int iReqId, const QString& url, const QString& content, Iptv::eReqType t_req = Iptv::String);
   virtual void q_get(int iReqId, const QString& url, Iptv::eReqType t_req = Iptv::String);

   virtual QNetworkReply* post(int iReqId, const QString& url, const QString& content, Iptv::eReqType t_req);
   virtual QNetworkReply*  get(int iReqId, const QString& url, Iptv::eReqType t_req);

private:
   QVariant          cookies;
   bool              bCSet;
   bool              bBusy;
   QVector<SRequest> vCmdQueue;
   QMutex            mtxCmdQueue;
#ifdef __TRACE
   Iptv              iptv;
#endif

protected:
   QNetworkRequest& prepareRequest(QNetworkRequest& req, const QString &url, int iSize = -1);
   QNetworkReply*   prepareReply(QNetworkReply* rep, int iReqId, Iptv::eReqType t_req);
   void workOffQueue ();

signals:
   void sigStringResponse (int reqId, QString strResp);
   void sigBinResponse (int reqId, QByteArray binResp);
   void sigErr (int reqId, QString sErr, int iErr);

   void sigM3u (int reqId, QString s);
   void sigHls (int reqId, QByteArray bHls);

public slots:

private slots:
   void slotResponse(QNetworkReply* reply);
};

#endif // __20130315_QIPTVCTRLCLIENT_H
