/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstalkerclient.h
 *
 *  @author   Jo2003
 *
 *  @date     06.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20150906_QSTALKERCLIENT_H
   #define __20150906_QSTALKERCLIENT_H

#include <QString>
#include <QDate>
#include <QRegExp>
#include <QtJson>

#include "qiptvctrlclient.h"
#include "clogfile.h"
#include "defdef.h"
#include "ciptvdefs.h"

//---------------------------------------------------------------------------
//! \class   QStalkerClient
//! \date    06.09.2015
//! \author  Jo2003
//! \brief   client for stalker API v2
//---------------------------------------------------------------------------
class QStalkerClient : public QIptvCtrlClient
{
    Q_OBJECT

    enum EInnerOps {
        IO_EPG_CUR,
        IO_TV_GENRES,
        IO_TV_CHANNELS,
        IO_DUNNO = 255
    };

public:
   explicit QStalkerClient(QObject *parent = 0);
   virtual ~QStalkerClient();

   void SetData(const QString &host, const QString &usr, const QString &pw, const QString& lang = "");
   int  queueRequest(CIptvDefs::EReq req, const QVariant& par_1 = QVariant(), const QVariant& par_2 = QVariant());
   void fillErrorMap();
   bool cookieSet();
   void SetCookie (const QString &cookie);
   void setUid(int id);


protected:
   virtual QNetworkRequest& prepareRequest(QNetworkRequest& req, const QString &url, int iSize = -1);
   void GetCookie ();
   void Logout ();
   void GetChannelList ();
   void SetTimeShift (int iHours);
   void GetTimeShift ();
   virtual void GetStreamURL (int iChanID, const QString &secCode, bool bTimerRec = false);
   virtual void GetArchivURL (const QString &prepared, const QString &secCode);
   virtual void GetVodUrl (int iVidId, const QString &secCode);
   void GetVodGenres ();
   void SetServer (const QString& sIp);
   void GetServer ();
   void SetBitRate (int iRate);
   void GetBitRate ();
   void SetHttpBuffer (int iTime);
   void GetEPG (int iChanID, int iOffset = 0);
   void GetVideos (const QString &sPrepared);
   void GetVideoInfo (int iVodID, const QString &secCode = QString());
   void setChanHide (const QString &cids, const QString &secCode);
   void setChanShow (const QString &cids, const QString &secCode);
   void getVodManager (const QString &secCode);
   void setVodManager (const QString &rules, const QString &secCode);
   void addVodFav (int iVidID, const QString &secCode);
   void remVodFav (int iVidID, const QString &secCode);
   void getVodFav ();
   void setParentCode (const QString& oldCode, const QString& newCode);
   void epgCurrent(const QString &cids);
   void updInfo (const QString& url);
   int  checkResponse (const QString &sResp, QString& sCleanResp);
   virtual void getVodLang();
   const QString& apiUrl();
   virtual void userData();
   void handleInnerOps(int reqId, const QString& resp);

private:
   QString   sUsr;
   QString   sPw;
   QString   sApiUrl;
   QString   sCookie;
   QErrorMap errMap;
   CIptvDefs karTrace;
   int       m_Uid;
   EInnerOps eIOps;
   int       mReqsToGo;
   QMap<int, QString>     mBufMap;
   QMap<QString, QString> mTvGenres;
   QVariantMap            mTvChannels;

public slots:
   void slotDownImg(const QString& url);

protected slots:
   virtual void slotStringResponse (int reqId, QString strResp);
   void slotBinResponse (int reqId, QByteArray binResp);
   void slotErr (int iReqId, QString sErr, int iErr);

signals:
   void sigError (QString str, int req, int err);
   void sigHttpResponse(QString str, int req);
   void sigImage(QByteArray ba);
};

#endif /* __20150906_QSTALKERCLIENT_H */


