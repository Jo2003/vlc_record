/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: 27.03.2014
|
| $Id$
|
\=============================================================================*/
#ifndef __20140327_CTELEPROMCLIENT_H
   #define __20140327_CTELEPROMCLIENT_H

#include <QString>
#include <QDate>
#include <QRegExp>

#include "qiptvctrlclient.h"
#include "defdef.h"
#include "ciptvdefs.h"

/*=============================================================================\
| Class:       CTelepromClient
|
| Author:      Jo2003
|
| Begin:       27.03.2014
|
| Description: a class to communicate with telepromt services
|
\=============================================================================*/
class CTelepromClient : public QIptvCtrlClient
{
   Q_OBJECT

public:
   explicit CTelepromClient(QObject *parent = 0);
   ~CTelepromClient();

   void SetData(const QString &host, const QString &usr, const QString &pw, const QString& lang = "");

   int  queueRequest(CIptvDefs::EReq req, const QVariant& par_1 = QVariant(), const QVariant& par_2 = QVariant());

   void fillErrorMap();
   bool cookieSet();
   void SetCookie (const QString &cookie);


protected:
   void GetCookie ();
   void Logout ();
   void GetChannelList (const QString &secCode = QString());
   void SetTimeShift (int iHours);
   void GetTimeShift ();
   void GetStreamURL (int iChanID, const QString &secCode, bool bTimerRec = false);
   void GetArchivURL (const QString &prepared, const QString &secCode);
   void GetVodUrl (int iVidId, const QString &secCode);
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

private:
   QString   sUsr;
   QString   sPw;
   QString   sCookie;
   QErrorMap errMap;
   CIptvDefs karTrace;

public slots:
   void slotDownImg(const QString& url);

private slots:
   void slotStringResponse (int reqId, QString strResp);
   void slotBinResponse (int reqId, QByteArray binResp);
   void slotErr (int iReqId, QString sErr, int iErr);

signals:
   void sigError (QString str, int req, int err);
   void sigHttpResponse(QString str, int req);
   void sigImage(QByteArray ba);
};

#endif // __20140327_CTELEPROMCLIENT_H

