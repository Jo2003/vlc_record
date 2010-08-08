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
#include "cchanlogo.h"

// for folders ...
extern CDirStuff *pFolders;

/* -----------------------------------------------------------------\
|  Method: CChanLogo / constructor
|  Begin: 18.01.2010 / 16:17:51
|  Author: Jo2003
|  Description: init values
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CChanLogo::CChanLogo()
{
   bRun  = false;
   iReq  = -1;

   // check, if dir exists ...
   QDir LogoDir(pFolders->getLogoDir());

   if (!LogoDir.exists())
   {
      LogoDir.mkpath(pFolders->getLogoDir());
   }

   // set hostname ...
   setHost(KARTINA_HOST);
   connect(this, SIGNAL(requestFinished(int,bool)), this, SLOT(slotCheckResp(int,bool)));
}

/* -----------------------------------------------------------------\
|  Method: ~CChanLogo / destructor
|  Begin: 18.01.2010 / 16:18:13
|  Author: Jo2003
|  Description: clean on destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CChanLogo::~CChanLogo()
{
   abort();
}

/* -----------------------------------------------------------------\
|  Method: SetChanList
|  Begin: 18.01.2010 / 16:18:33
|  Author: Jo2003
|  Description: set channel list, start download
|
|  Parameters: channel list
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanLogo::SetChanList(const QVector<cparser::SChan> &list)
{
   // set channel list only if download process
   // isn't already running ...
   if (!bRun)
   {
      chanList = list;

      cit      = chanList.constBegin();

      if (cit != chanList.constEnd())
      {
         bRun     = true;
         StartDownLoad();
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: StartDownLoad
|  Begin: 18.01.2010 / 16:19:04
|  Author: Jo2003
|  Description: download logo if not exists
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanLogo::StartDownLoad()
{
   if (!(*cit).bIsGroup)
   {
      if (!QFile::exists(QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg((*cit).iId)))
      {
         dataBuffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
         iReq = get((*cit).sIcon, &dataBuffer);
      }
      else
      {
         // file exists -> trigger next request ...
         iReq = -1;
         emit requestFinished (iReq, true);
      }
   }
   else
   {
      // no needed item -> trigger next request ...
      iReq = -2;
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
void CChanLogo::slotCheckResp(int iReqID, bool err)
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
            // create gif file ...
            QFile gif(QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg((*cit).iId));

            if (gif.open(QIODevice::WriteOnly))
            {
               // write content ...
               gif.write(ba);
               gif.close();
            }
         }
      }

      // check if we can get next item ...
      if (++ cit != chanList.constEnd())
      {
         // download next ...
         StartDownLoad();
      }
      else
      {
         // last item ... download finished ...
         bRun = false;

         // send signal that we're done ...
         emit sigLogosReady();
      }
   }
}

/************************* History ***************************\
| $Log$
\*************************************************************/

