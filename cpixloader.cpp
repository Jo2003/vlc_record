/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:17:24
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cpixloader.h"
#include "defdef.h"
#include "qcustparser.h"

// global customization class ...
extern QCustParser *pCustomization;

/* -----------------------------------------------------------------\
|  Method: CPixLoader / constructor
|  Begin: 18.01.2010 / 16:17:51
|  Author: Jo2003
|  Description: init values
|
|  Parameters: pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
CPixLoader::CPixLoader(QObject *parent) : QObject(parent)
{
   bRun  = false;
}

/* -----------------------------------------------------------------\
|  Method: ~CPixLoader / destructor
|  Begin: 18.01.2010 / 16:18:13
|  Author: Jo2003
|  Description: clean on destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CPixLoader::~CPixLoader()
{
}

/* -----------------------------------------------------------------\
|  Method: startDownLoad
|  Begin: 18.01.2010 / 16:19:04
|  Author: Jo2003
|  Description: download picture if not exists
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPixLoader::startDownLoad()
{
   int iCount = 0;
   PixCache::SPixDesc desc;

   // get first most entry ...
   mtxCacheQueue.lock();
   desc = cacheQueue[0];
   mtxCacheQueue.unlock();

   QFileInfo info(desc.sRemote);

   if (!QFile::exists(QString("%1/%2").arg(desc.sLocal).arg(info.fileName())))
   {
      // request download ...
      if (desc.sRemote.contains("http"))
      {
         // full url ...
         emit sigLoadImage(desc.sRemote);
      }
      else
      {
         emit sigLoadImage(QString("http://%1%2").arg(pCustomization->strVal("API_SERVER")).arg(desc.sRemote));
      }
   }
   else
   {
      // file exists, remove requested item
      // and download next ...
      mtxCacheQueue.lock();
      cacheQueue.remove(0);
      iCount = cacheQueue.count();
      mtxCacheQueue.unlock();

      if (iCount > 0)
      {
         // recursive call ...
         startDownLoad();
      }
      else
      {
         // empty queue ...
         bRun = false;
         emit allDone();
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotImage (slot)
|  Begin: 18.03.2013
|  Author: Jo2003
|  Description: check http response (answer)
|
|  Parameters: image as byte array
|
|  Returns: --
\----------------------------------------------------------------- */
void CPixLoader::slotImage(const QByteArray &ba)
{
   int iCount = 0;
   PixCache::SPixDesc desc;

   // is this a gif file ... ?
   if (ba.size() > 0)
   {
      mtxCacheQueue.lock();
      desc = cacheQueue[0];
      mtxCacheQueue.unlock();

      QFileInfo info(desc.sRemote);
      QFile     pic(QString("%1/%2").arg(desc.sLocal).arg(info.fileName()));

      if (pic.open(QIODevice::WriteOnly))
      {
         // write content ...
         pic.write(ba);
         pic.close();
      }
   }

   // remove requested item ...
   mtxCacheQueue.lock();
   cacheQueue.remove(0);
   iCount = cacheQueue.count();
   mtxCacheQueue.unlock();

   if (iCount > 0)
   {
      // download next ...
      startDownLoad();
   }
   else
   {
      // last item ... download finished ...
      bRun = false;
      emit allDone();
   }
}

/* -----------------------------------------------------------------\
|  Method: enqueuePic
|  Begin: 31.05.2012
|  Author: Jo2003
|  Description: queue in picture to load
|
|  Parameters: remote url, local folder
|
|  Returns: --
\----------------------------------------------------------------- */
void CPixLoader::enqueuePic(const QString &sRemote, const QString &sLocal)
{
   PixCache::SPixDesc desc;
   desc.sLocal  = sLocal;
   desc.sRemote = sRemote;

   mtxCacheQueue.lock();
   cacheQueue.append(desc);
   mtxCacheQueue.unlock();

   if (!bRun)
   {
      bRun = true;
      startDownLoad();
   }
}

/* -----------------------------------------------------------------\
|  Method: busy
|  Begin: 31.05.2012
|  Author: Jo2003
|  Description: is any download in progress
|
|  Parameters: --
|
|  Returns: true --> yes
|          false --> no
\----------------------------------------------------------------- */
bool CPixLoader::busy()
{
   return bRun;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
