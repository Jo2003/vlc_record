/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:16:25
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011810__PIXLOADER_H
   #define __011810__PIXLOADER_H

#include <QHttp>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QBuffer>
#include <QFileInfo>
#include <QVector>
#include <QMutex>

namespace PixCache {
   struct SPixDesc
   {
      QString sRemote;
      QString sLocal;
   };
}

typedef QVector<PixCache::SPixDesc> PixVector;

/********************************************************************\
|  Class: CPixLoader
|  Date:  21.12.2010 / 12:00
|  Author: Jo2003
|  Description: helper class to download pictures
|
\********************************************************************/
class CPixLoader : public QHttp
{
   Q_OBJECT

public:
   CPixLoader();
   virtual ~CPixLoader();
   void enqueuePic (const QString& sRemote, const QString &sLocal);
   bool busy();

protected:
   void startDownLoad ();

signals:
   void allDone();

private:
   QMutex      mtxCacheQueue;
   PixVector   cacheQueue;
   QBuffer     dataBuffer;
   bool        bRun;
   bool        bIsAnswer;
   int         iReq;

private slots:
   void slotCheckResp (int iReqID, bool err);
};

#endif // __011810__PIXLOADER_H
/************************* History ***************************\
| $Log$
\*************************************************************/

