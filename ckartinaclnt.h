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

#include <QHttp>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QBuffer>
#include <QString>
#include <QMessageBox>
#include <QtNetwork>
#include <QDate>

#include "clogfile.h"
#include "chttptime.h"
#include "defdef.h"
#include "customization.h"


namespace Kartina {
   enum EReq
   {
      REQ_COOKIE,
      REQ_CHANNELLIST,
      REQ_STREAM,
      REQ_TIMESHIFT,
      REQ_EPG,
      REQ_SERVER,
      REQ_HTTPBUFF,
      REQ_ARCHIV,
      REQ_TIMERREC,
      REQ_UNKNOWN = 255
   };
}

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
class CKartinaClnt : public QHttp
{
   Q_OBJECT

public:
   CKartinaClnt(const QString &host, const QString &usr = QString(),
                const QString &pw = QString(), bool bAllowErotic = false);
   CKartinaClnt();
   ~CKartinaClnt();

   void SetData(const QString &host, const QString &usr = QString(),
                const QString &pw = QString(), bool bAllowErotic = false);
   void GetCookie ();
   void GetChannelList ();
   void SetTimeShift (int iHours);
   void GetStreamURL (int iChanID, bool bTimerRec = false);
   void GetArchivURL (const QString &prepared);
   void SetServer (int iSrv);
   void SetHttpBuffer (int iTime);
   void GetEPG (int iChanID, int iOffset = 0);
   bool busy ();

protected:
   void PostRequest (Kartina::EReq req, const QString &path, const QString &content);

private:
   Kartina::EReq eReq;
   QString       sUsr;
   QString       sPw;
   QString       sHost;
   bool          bEros;
   QString       sCookie;
   QByteArray    baPageContent;
   QBuffer       bufReq;
   bool          bRenewCookie;
   int           iReq;

private slots:
   void handleEndRequest (int id, bool err);
   void getResponseHeader (const QHttpResponseHeader &resp);

signals:
   void sigGotCookie ();
   void sigTimeShiftSet ();
   void sigGotChannelList (QString str);
   void sigGotEPG (QString str);
   void sigGotStreamURL (QString str);
   void sigGotTimerStreamURL (QString str);
   void sigGotArchivURL (QString str);
   void sigServerChanged ();
   void sigBufferSet ();
   void sigError (QString str);
};

#endif /* __201004161114_CKARTINACLNT_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/

