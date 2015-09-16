/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: 4.12.2010 / 18:55
|
| $Id$
|
\=============================================================================*/
#ifndef __20101214185500_CSTREAMLOADER_H
   #define __20101214185500_CSTREAMLOADER_H

#include <QHttp>
#include <QString>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QtNetwork>
#include <QFile>
#include <QTimer>
#include <QTime>
#include <QUrl>

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
   QString         sHost;
   QFile           fStream;
   int             iReq;
   QTimer          tFileCheck;
   QTime           m_tmDwn;
   int             iCache;
   bool            bUseTimerRec;
   bool            m_bSpeedTest;
   QTemporaryFile *m_pTmpFile;
   int             m_iSize;

public slots:
   void downloadStream (const QString &sUrl, const QString &sFileName, int iCacheTime, bool bTimerRec = false);
   void speedTest (const QString &sUrl, int iSize);
   void endSpeedTest();
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
   void sigDwnSpeed (int ms, int bytes);
   void sigSpeedTestEnd ();
};

#endif // __20101214185500_CSTREAMLOADER_H
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 14.12.2010: - a download class for streams sent from kartina.tv
\=============================================================================*/

