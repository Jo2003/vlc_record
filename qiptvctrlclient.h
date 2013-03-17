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
      String,
      Binary,
      Stream,
      StringNoCookie,
      BinNoCookie,
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
   Q_OBJECT

public:
   explicit QIptvCtrlClient(QObject* parent = 0);
   virtual ~QIptvCtrlClient();

   virtual QNetworkReply* post(int iReqId, const QString& url, const QString& content, Iptv::eReqType t_req = Iptv::String);
   virtual QNetworkReply*  get(int iReqId, const QString& url, Iptv::eReqType t_req = Iptv::String);

private:
   QVariant cookies;
#ifdef __TRACE
   Iptv     iptv;
#endif

protected:
   QNetworkRequest& prepareRequest(QNetworkRequest& req, const QString &url);
   QNetworkReply*   prepareReply(QNetworkReply* rep, int iReqId, Iptv::eReqType t_req);

signals:
   void sigStringResponse (int reqId, QString strResp);
   void sigBinResponse (int reqId, QByteArray binResp);
   void sigErr (int reqId, QString sErr, int iErr);

public slots:

private slots:
   void slotResponse(QNetworkReply* reply);
};

#endif // __20130315_QIPTVCTRLCLIENT_H
