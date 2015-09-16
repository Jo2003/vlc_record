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
#include "externals_inc.h"

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
   iReq         = -1;
   iCache       = -1;
   sHost        = "";
   bUseTimerRec = false;
   m_pTmpFile   = NULL;
   m_iSize      = 0;
   m_bSpeedTest = false;

   // set timer interval for file check ...
   tFileCheck.setInterval(2000); // 2 sec ...

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
   tFileCheck.stop();

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
void CStreamLoader::downloadStream (const QString &sUrl, const QString &sFileName, int iCacheTime, bool bTimerRec)
{
   QString tmpUrl = sUrl;
   bUseTimerRec   = bTimerRec;

   mInfo(tr("Archive URL:\n --> %1").arg(sUrl));

   // patch stream prefix to allow the usage of QUrl ...
   tmpUrl.replace ("http/ts", "http");
   QUrl    url(tmpUrl);

   // print out host ...
   mInfo(tr("Download Stream from http://%1:%2\n --> to %3")
         .arg(url.host()).arg((url.port() == -1) ? 80 : url.port())
         .arg(sFileName));

   // save cache time ...
   iCache = iCacheTime;

   // set file name for download file ...
   fStream.setFileName (sFileName);

   // unbuffered to make sure that data will be written down to file as soon as possible ...
   if (fStream.open (QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered))
   {
      setHost (url.host(), QHttp::ConnectionModeHttp, (url.port() == -1) ? 80 : url.port());

      iReq = get (tmpUrl, &fStream);

      mInfo(tr("Request #%1 sent ...").arg(iReq));

      // start file check timer ...
      tFileCheck.start();

      emit sigStreamRequested(iReq);
   }
}

//---------------------------------------------------------------------------
//! \brief   start speed test
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \param   sUrl [in] (onst QString&) download url
//! \param   iSize [in] (int) size to download in MB (10 ... 100).
//---------------------------------------------------------------------------
void CStreamLoader::speedTest(const QString &sUrl, int iSize)
{
    QUrl url(sUrl);
    m_iSize      = (iSize <= 0) ? 10 : ((iSize > 100) ? 100 : iSize);
    m_pTmpFile   = new QTemporaryFile();
    m_bSpeedTest = true;

    if (m_pTmpFile)
    {
        if (m_pTmpFile->open())
        {
            tFileCheck.setInterval(333);

            setHost (url.host(), QHttp::ConnectionModeHttp, (url.port() == -1) ? 80 : url.port());

            iReq = get (url.toString(), m_pTmpFile);

            mInfo(tr("Request #%1 (%2) sent ...").arg(iReq).arg(sUrl));

            // start file check timer ...
            tFileCheck.start();

            m_tmDwn.start();
        }
    }
}

//---------------------------------------------------------------------------
//! \brief   end speed test
//
//! \author  Jo2003
//! \date    16.09.2015
//
//---------------------------------------------------------------------------
void CStreamLoader::endSpeedTest()
{
    abort();
    tFileCheck.stop();
    m_bSpeedTest = false;

    emit sigSpeedTestEnd();

    delete m_pTmpFile;
    m_pTmpFile = NULL;
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
      // make sure we don't check filesize anymore ...
      tFileCheck.stop();

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
    if (m_bSpeedTest)
    {
        if (m_pTmpFile)
        {
            emit sigDwnSpeed(m_tmDwn.elapsed(), m_pTmpFile->size());

            if (m_pTmpFile->size() >= (m_iSize * 1024 * 1024))
            {
                endSpeedTest();
            }
        }
    }
    else
    {
        // Make sure downloaded part of video is "big" enough
        // so libVLC will not reach the end when reading it.
        // Assume a good value up to 720p (HD Ready).

        // 4.5Mbit/s -> 562.5kB/s

        // wait until file is filled with cache size ...
        int iSize = (iCache / 1000) * 562500;

        iSize = (iSize < MIN_CACHE_SIZE) ? MIN_CACHE_SIZE : iSize;

        if (fStream.size() >= iSize)
        {
            // check no more needed ...
            tFileCheck.stop();

            if (!bUseTimerRec)
            {
                emit sigStreamDownload(iReq, fStream.fileName());
            }
            else
            {
                emit sigStreamDwnTimer(iReq, fStream.fileName());
            }
            emit sigBufferPercent(100);
        }
        else
        {
            emit sigBufferPercent((int)((100 * fStream.size()) / iSize));
        }
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
