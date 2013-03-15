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

#include "clogfile.h"

namespace Iptv
{
   enum eRespType {
      E_RESP_STRING,
      E_RESP_BINARY,
      E_RESP_UNKNOWN
   };
}

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

   virtual QNetworkReply* post(int iReqId, const QString& url, const QString& content, Iptv::eRespType eResp = Iptv::E_RESP_STRING);
   virtual QNetworkReply*  get(int iReqId, const QString& url, Iptv::eRespType eResp = Iptv::E_RESP_STRING);

private:
   QVariant cookies;

protected:
   QNetworkRequest& prepareRequest(QNetworkRequest& req, const QString &url);
   QNetworkReply*   prepareReply(QNetworkReply* rep, int iReqId, Iptv::eRespType eResp);

signals:
   void sigStringResponse (int reqId, QString strResp);
   void sigBinResponse (int reqId, QByteArray binResp);
   void sigErr (QString sErr, int iErr);

public slots:

private slots:
   void slotResponse(QNetworkReply* reply);
};

#endif // __20130315_QIPTVCTRLCLIENT_H
