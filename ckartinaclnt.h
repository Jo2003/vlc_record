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
#include <QFile>
#include <QTimer>

#include "clogfile.h"
#include "defdef.h"
#include "customization.h"
#include "version_info.h"


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
      REQ_GET_SERVER,
      REQ_LOGOUT,
      REQ_GETTIMESHIFT,
      REQ_GETVODGENRES,
      REQ_GETVIDEOS,
      REQ_GETVIDEOINFO,
      REQ_ABORT,
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
   CKartinaClnt(const QString &host, const QString &usr, const QString &pw,
                const QString &sEPw = QString(), bool bAllowErotic = false);

   CKartinaClnt();
   ~CKartinaClnt();

   void SetData(const QString &host, const QString &usr, const QString &pw,
                const QString &sEPw = QString(), bool bAllowErotic = false);

   void GetCookie ();
   void Logout ();
   void GetChannelList ();
   void SetTimeShift (int iHours);
   void GetTimeShift ();
   void GetStreamURL (int iChanID, bool bTimerRec = false);
   void GetArchivURL (const QString &prepared);
   void GetVodGenres ();
   void SetServer (const QString& sIp);
   void GetServer ();
   void SetHttpBuffer (int iTime);
   void GetEPG (int iChanID, int iOffset = 0);
   void GetVideos (int iGenreID);
   void GetVideoInfo (int iVodID);
   void SetCookie (const QString &cookie);
   bool busy ();

protected:
   void PostRequest (Kartina::EReq req, const QString &path, const QString &content,
                     const QString &sBrowser = "VLC-Record " __MY__VERSION__);
   void GetRequest (Kartina::EReq req, const QString &sRequest,
                    const QString &sBrowser = "VLC-Record " __MY__VERSION__);

private:
   Kartina::EReq eReq;
   QString       sUsr;
   QString       sPw;
   QString       sHost;
   QString       sErosPw;
   bool          bEros;
   QString       sCookie;
   QByteArray    baPageContent;
   QBuffer       bufReq;
   int           iReq;

private slots:
   void handleEndRequest (int id, bool err);

signals:
   void sigGotCookie (QString str);
   void sigGotTimeShift (QString str);
   void sigTimeShiftSet (QString str);
   void sigGotChannelList (QString str);
   void sigGotEPG (QString str);
   void sigGotVodGenres (QString str);
   void sigGotStreamURL (QString str);
   void sigGotTimerStreamURL (QString str);
   void sigGotArchivURL (QString str);
   void sigServerChanged (QString str);
   void sigBufferSet (QString str);
   void sigSrvForm (QString str);
   void sigError (QString str);
   void sigLogout (QString str);
   void sigGotVideos (QString str);
   void sigGotVideoInfo (QString str);
};

#endif /* __201004161114_CKARTINACLNT_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 04.Jan.2010 - communication API for kartina.tv (inspired by conros)
\=============================================================================*/

