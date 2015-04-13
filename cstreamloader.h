/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cstreamloader.h $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: 4.12.2010 / 18:55
|
| $Id: cstreamloader.h 871 2012-08-10 12:35:55Z Olenka.Joerg $
|
\=============================================================================*/
#ifndef __20101214185500_CSTREAMLOADER_H
   #define __20101214185500_CSTREAMLOADER_H

#include <QHttp>
#include <QString>
#include <QMessageBox>
#include <QtNetwork>
#include <QFile>
#include <QTimer>
#include <QUrl>

#include "clogfile.h"
#include "defdef.h"

/*=============================================================================\
| Class:       CStreamLoader
|
| Author:      Jo2003
|
| Begin:       14.12.2010 / 18:55
|
| Description: a class to download streams from streamserver
|
\=============================================================================*/
class CStreamLoader : public QHttp
{
   Q_OBJECT

public:
   CStreamLoader();
   virtual ~CStreamLoader();

private:
   QString       sHost;
   QFile         fStream;
   int           iReq;
   QTimer        tFileCheck;
   int           iCache;
   bool          bUseTimerRec;

public slots:
   void downloadStream (const QString &sUrl, const QString &sFileName, int iCacheTime, bool bTimerRec = false);
   void stopDownload (int id);

private slots:
   void handleEndRequest (int id, bool err);
   void slotStreamDataAvailable();

signals:
   void sigStreamRequested (int iReqId);
   void sigStreamDownload (int iReqId, QString sFileName);
   void sigStreamDwnTimer (int iReqId, QString sFileName);
   void sigError (QString str);
   void sigBufferPercent (int percent);
};

#endif // __20101214185500_CSTREAMLOADER_H
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 14.12.2010: - a download class for streams sent from kartina.tv
\=============================================================================*/

