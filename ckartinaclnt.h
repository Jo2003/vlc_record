/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: Monday, January 04, 2010 16:11:14
|
| $Id$
|
\=============================================================================*/
#ifndef __201004161114_CKARTINACLNT_H
   #define __201004161114_CKARTINACLNT_H

#include <QString>
#include <QDate>
#include <QRegExp>

#include "qiptvctrlclient.h"
#include "defdef.h"
#include "ciptvdefs.h"

/*=============================================================================\
| Class:       CKartinaClnt
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:12:28
|
| Description: a class to communicate with kartina.tv services
|
\=============================================================================*/
class CKartinaClnt : public QIptvCtrlClient
{
   Q_OBJECT

public:
   explicit CKartinaClnt(QObject *parent = 0);
   virtual ~CKartinaClnt();

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
   virtual void GetStreamURL (int iChanID, const QString &secCode, bool bTimerRec = false);
   virtual void GetArchivURL (const QString &prepared, const QString &secCode);
   virtual void GetVodUrl (int iVidId, const QString &secCode);
   void GetVodGenres ();
   void SetServer (const QString& sIp);
   void GetServer ();
   void SetBitRate (int iRate);
   void GetBitRate ();
   void SetHttpBuffer (int iTime);
   void GetEPG (int iChanID, int iOffset = 0, bool bExtEPG = false);
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
   void statsService(const QString& stats);
   void statsOnly(const QString& stats);

private:
   QString   sUsr;
   QString   sPw;
   QString   sApiUrl;
   QString   sCookie;
   QErrorMap errMap;
   CIptvDefs karTrace;

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

#endif /* __201004161114_CKARTINACLNT_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/

