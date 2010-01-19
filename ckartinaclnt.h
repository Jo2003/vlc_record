/*=============================================================================\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/ckartinaclnt.h $
|
| Author: Joerg Neubert
|
| last changed by: $Author: joergn $
|
| Begin: Monday, January 04, 2010 16:11:14
|
| $Id: ckartinaclnt.h 173 2010-01-18 15:43:19Z joergn $
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
#include <QMutex>

#include "clogfile.h"
#include "chttptime.h"
#include "defdef.h"


namespace Kartina {
   enum EReq
   {
      REQ_COOKIE,
      REQ_CHANNELLIST,
      REQ_STREAM,
      REQ_TIMESHIFT,
      REQ_EPG,
      REQ_SERVER,
      REQ_ARCHIV,
      REQ_UNKNOWN = 255
   };
}

/*=============================================================================\
| Class:       CKartinaClnt
|
| Author:      Joerg Neubert
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
   void GetStreamURL (int iChanID);
   void GetArchivURL (const QString &prepared);
   void SetServer (int iSrv);
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
   QMutex        mutexBuffer;

private slots:
   void handleEndRequest (/* int id, */bool err);
   void getResponseHeader (const QHttpResponseHeader &resp);

signals:
   void sigGotCookie ();
   void sigTimeShiftSet ();
   void sigGotChannelList (QString str);
   void sigGotEPG (QString str);
   void sigGotStreamURL (QString str);
   void sigGotArchivURL (QString str);
   void sigServerChanged ();
   void sigError (QString str);
};

#endif /* __201004161114_CKARTINACLNT_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/

