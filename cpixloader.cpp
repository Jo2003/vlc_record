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

/* -----------------------------------------------------------------\
|  Method: CPixLoader / constructor
|  Begin: 18.01.2010 / 16:17:51
|  Author: Jo2003
|  Description: init values
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CPixLoader::CPixLoader()
{
   bRun  = false;
   iReq  = -1;

   connect(this, SIGNAL(requestFinished(int,bool)), this, SLOT(slotCheckResp(int,bool)));
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
   abort();
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
   QFileInfo info(cacheQueue[0].sRemote);

   if (!QFile::exists(QString("%1/%2").arg(cacheQueue[0].sLocal).arg(info.fileName())))
   {
      dataBuffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
      iReq = get(cacheQueue[0].sRemote, &dataBuffer);
   }
   else
   {
      // file exists -> trigger next request ...
      iReq = -1;
      emit requestFinished (iReq, true);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotCheckResp (slot)
|  Begin: 18.01.2010 / 16:19:35
|  Author: Jo2003
|  Description: check http response (answer)
|
|  Parameters: request id, error flag
|
|  Returns: --
\----------------------------------------------------------------- */
void CPixLoader::slotCheckResp(int iReqID, bool err)
{
   // is this requested response ... ?
   if (iReqID == iReq)
   {
      QByteArray ba;
      dataBuffer.close();

      // copy response into local buffer ...
      if ((iReqID > -1) && !err)
      {
         dataBuffer.open(QIODevice::ReadOnly);
         ba = dataBuffer.data();
         dataBuffer.close();

         // is this a gif file ... ?
         if (ba.size() > 0)
         {
            QFileInfo info(cacheQueue[0].sRemote);
            QFile     pic(QString("%1/%2").arg(cacheQueue[0].sLocal).arg(info.fileName()));

            if (pic.open(QIODevice::WriteOnly))
            {
               // write content ...
               pic.write(ba);
               pic.close();
            }
         }
      }

      // remove requested item ...
      mtxCacheQueue.lock();
      cacheQueue.remove(0);
      mtxCacheQueue.unlock();

      if (cacheQueue.count() > 0)
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

