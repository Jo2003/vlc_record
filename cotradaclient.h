/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/COtradaClient.h $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: 19.03.2013
|
| $Id: COtradaClient.h 1495 2015-02-20 13:48:22Z Olenka.Joerg $
|
\=============================================================================*/
#ifndef __20130319_COtradaClient_H
   #define __20130319_COtradaClient_H

#include <QString>
#include <QDate>
#include <QRegExp>
#include <QTimer>

#include "qiptvctrlclient.h"
#include "clogfile.h"
#include "defdef.h"
#include "ciptvdefs.h"

/*=============================================================================\
| Class:       COtradaClient
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:12:28
|
| Description: a class to communicate with kartina.tv services
|
\=============================================================================*/
class COtradaClient : public QIptvCtrlClient
{
   Q_OBJECT

public:
   explicit COtradaClient(QObject *parent = 0);
   ~COtradaClient();

   void SetData(const QString &host, const QString &usr, const QString &pw, const QString& lang = "");

   int  queueRequest(CIptvDefs::EReq req, const QVariant& par_1 = QVariant(), const QVariant& par_2 = QVariant());

   void fillErrorMap();
   bool cookieSet();
   void SetCookie (const QString &cookie);


protected:
   void GetCookie ();
   void Logout ();
   void GetChannelList ();
   void GetProtChannelList (const QString &secCode);
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
   void getRadioList();
   void getRadioStream(int cid, bool bTimerRec = false);
   QString combineChannelLists(const QString& resp);
   void setInterfaceLang (const QString& langCode);
   int  checkResponse (const QString &sResp, QString& sCleanResp);
   void chanListLang(const QString& lang);
   void audioLang();

private:
   QString   sUsr;
   QString   sPw;
   QString   sApiUrl;
   QString   sCookie;
   QString   sLang;
   QErrorMap errMap;
   CIptvDefs karTrace;
   QString   sChanListBuffer;
   QString   sLangFilter;
   // QTimer    tPing;

public slots:
   void slotDownImg(const QString& url);
   void slotPing();

private slots:
   void slotStringResponse (int reqId, QString strResp);
   void slotBinResponse (int reqId, QByteArray binResp);
   void slotErr (int iReqId, QString sErr, int iErr);

signals:
   void sigError (QString str, int req, int err);
   void sigHttpResponse(QString str, int req);
   void sigImage(QByteArray ba);
};

#endif // __20130319_COtradaClient_H
