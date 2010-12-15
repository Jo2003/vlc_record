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
#include "cstreamloader.h"

// log file functions ...
extern CLogFile VlcLog;

/*-----------------------------------------------------------------------------\
| Function:    CStreamLoader / constructor
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: construct stream downloader class, init values
|
| Parameters:  --
|
\-----------------------------------------------------------------------------*/
CStreamLoader::CStreamLoader() :QHttp()
{
   iReq   = -1;
   iCache = -1;
   sHost  = "";

   // set timer interval for file check ...
   tFileCheck.setInterval(1000); // 1 sec ...

   connect(this, SIGNAL(requestFinished(int, bool)), this, SLOT(handleEndRequest(int, bool)));
   connect(&tFileCheck, SIGNAL(timeout()), this, SLOT(slotStreamDataAvailable()));
}

/*-----------------------------------------------------------------------------\
| Function:    ~CStreamLoader / destructor
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: constructs a CKartinaClnt object to communicate with
|              kartina.tv
| Parameters:  host, username, password, erotic allowed (true / false)
|
\-----------------------------------------------------------------------------*/
CStreamLoader::~CStreamLoader()
{
   abort();

   if (fStream.isOpen ())
   {
      fStream.close ();
   }
}

/* -----------------------------------------------------------------\
|  Method: downloadStream [slot]
|  Begin: 13.12.2010 / 16:32:33
|  Author: Jo2003
|  Description: download stream to disk
|
|  Parameters: stream url, file name, optional cache time
|
|  Returns: --
\----------------------------------------------------------------- */
void CStreamLoader::downloadStream (const QString &sUrl, const QString &sFileName, int iCacheTime)
{
   QString tmpUrl = sUrl;

   // patch stream prefix to allow the usage of QUrl ...
   tmpUrl.replace ("http/ts", "http");
   QUrl    url(tmpUrl);

   // save cache time ...
   iCache = iCacheTime;

   // set file name for download file ...
   fStream.setFileName (sFileName);

   // unbuffered to make sure that data will be written down to file as soon as possible ...
   if (fStream.open (QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
   {
      setHost (url.host(), QHttp::ConnectionModeHttp, url.port());

      iReq = get (sUrl, &fStream);

      mInfo(tr("Request #%1 sent ...").arg(iReq));

      // start file check timer ...
      tFileCheck.start();
   }
}

/* -----------------------------------------------------------------\
|  Method: stopDownload
|  Begin: 13.12.2010 / 16:32:33
|  Author: Jo2003
|  Description: stop stream download
|
|  Parameters: request id
|
|  Returns: --
\----------------------------------------------------------------- */
void CStreamLoader::stopDownload(int id)
{
   // we should abort only the download ...
   if (id == iReq)
   {
      // quit download ...
      abort();

      // close stream file ...
      fStream.close();
   }
}

/* -----------------------------------------------------------------\
|  Method: slotStreamDataAvailable
|  Begin: 14.12.2010 / 11:35
|  Author: Jo2003
|  Description: check if we have already downloaded a part of
|               the stream
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CStreamLoader::slotStreamDataAvailable()
{
   // wait until file is filled with cache size ...
   if (fStream.size() >= ((iCache / 1000) * 250000))
   {
      emit sigStreamDownload(iReq, fStream.fileName());
      tFileCheck.stop();
   }
}

/*-----------------------------------------------------------------------------\
| Function:    handleEndRequest (slot)
|
| Author:      Jo2003
|
| Begin:       Monday, January 04, 2010 16:14:52
|
| Description: sends messages if a request has finished ...
|
| Parameters:  request id (unused), error inidicator
|
| Returns:     --
\-----------------------------------------------------------------------------*/
void CStreamLoader::handleEndRequest(int id, bool err)
{
   // is this our request ... ?
   if (id == iReq)
   {
      if (!err)
      {
         mInfo(tr("Request #%1 done!").arg(id));

         // This shouldn't happen. The download should end only
         // on request. In case it ends, we will close the
         // file to which data were downloaded ...
         fStream.close();
      }
      else
      {
         mErr(tr("Error in Request: %1!").arg(errorString()));

         // send error signal ...
         emit sigError(errorString());
      }
   }
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 14.12.2010: - a download class for streams sent from kartina.tv
\=============================================================================*/
