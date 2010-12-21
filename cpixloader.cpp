/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/trunk/vlc-record/cchanlogo.cpp $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:17:24
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cchanlogo.cpp 269 2010-08-08 13:35:24Z Olenka.Joerg $
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
|  Method: setHostAndFolder
|  Begin:  21.12.2010 / 12:22
|  Author: Jo2003
|  Description: set hostname and local folder
|
|  Parameters: host name and local folder
|
|  Returns: --
\----------------------------------------------------------------- */
void CPixLoader::setHostAndFolder(const QString &host, const QString &folder)
{
   sLocalFolder = folder;
   sHost        = host;

   // check, if dir exists ...
   QDir PicDir(sLocalFolder);

   if (!PicDir.exists())
   {
      PicDir.mkpath(sLocalFolder);
   }

   // set hostname ...
   setHost(sHost);
}

/* -----------------------------------------------------------------\
|  Method: setPictureList
|  Begin: 18.01.2010 / 16:18:33
|  Author: Jo2003
|  Description: set picture list, start download
|
|  Parameters: picture list
|
|  Returns: --
\----------------------------------------------------------------- */
void CPixLoader::setPictureList(const QStringList &list)
{
   // set picture list only if download process
   // isn't already running ...
   if (!bRun)
   {
      lPicList = list;

      cit      = lPicList.constBegin();

      if (cit != lPicList.constEnd())
      {
         bRun     = true;
         startDownLoad();
      }
   }
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
   QFileInfo info(*cit);

   if (!QFile::exists(QString("%1/%2").arg(sLocalFolder).arg(info.fileName())))
   {
      dataBuffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
      iReq = get(*cit, &dataBuffer);
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
      if (iReqID > -1)
      {
         dataBuffer.open(QIODevice::ReadOnly);
         ba = dataBuffer.data();
         dataBuffer.close();
      }

      // if no error, save file ...
      if (!err)
      {
         // is this a gif file ... ?
         if (ba.size() > 0)
         {
            QFileInfo info(*cit);
            QFile     pic(QString("%1/%2").arg(sLocalFolder).arg(info.fileName()));

            if (pic.open(QIODevice::WriteOnly))
            {
               // write content ...
               pic.write(ba);
               pic.close();
            }
         }
      }

      // check if we can get next item ...
      if (++ cit != lPicList.constEnd())
      {
         // download next ...
         startDownLoad();
      }
      else
      {
         // last item ... download finished ...
         bRun = false;

         // send signal that we're done ...
         emit sigPixReady();
      }
   }
}

/************************* History ***************************\
| $Log$
\*************************************************************/

