/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:51:46
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cwaittrigger.h"

// log file functions ...
extern CLogFile VlcLog;

/* -----------------------------------------------------------------\
|  Method: CWaitTrigger / constructor
|  Begin: 19.01.2010 / 15:52:15
|  Author: Jo2003
|  Description: create object, init values
|
|  Parameters: parent pointer
|
|  Returns: --
\----------------------------------------------------------------- */
CWaitTrigger::CWaitTrigger(QObject * parent) : QThread(parent)
{
   pClient  = NULL;
   iGo      = 1;
}

/* -----------------------------------------------------------------\
|  Method: ~CWaitTrigger / destructor
|  Begin: 19.01.2010 / 15:52:51
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CWaitTrigger::~CWaitTrigger()
{
   stop();
}

/* -----------------------------------------------------------------\
|  Method: SetKartinaClient
|  Begin: 19.01.2010 / 15:53:14
|  Author: Jo2003
|  Description: set kartina client class pointer
|
|  Parameters: pointer to kartina connection
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::SetKartinaClient(CKartinaClnt *pKartinaClient)
{
   pClient = pKartinaClient;
}

/* -----------------------------------------------------------------\
|  Method: stop
|  Begin: 19.01.2010 / 15:53:52
|  Author: Jo2003
|  Description: stop thread loop
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::stop()
{
   iGo = 0;
   wait();
}

/* -----------------------------------------------------------------\
|  Method: run
|  Begin: 19.01.2010 / 15:54:11
|  Author: Jo2003
|  Description: thread loop, wait for requests and throw it
|               when client becomes ready
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::run()
{
   QVector<CommandQueue::SCmd>::iterator it;
   CommandQueue::SCmd                    cmd;
   while (iGo)
   {
      if (!commandQueue.isEmpty ())
      {
         // lock command queue ...
         mutex.lock();

         // copy first queue element ...
         it = commandQueue.begin();
         cmd.eReq     = (*it).eReq;
         cmd.iOptArg1 = (*it).iOptArg1;
         cmd.iOptArg2 = (*it).iOptArg2;
         cmd.sOptArg1 = (*it).sOptArg1;
         cmd.sOptArg2 = (*it).sOptArg2;

         // remove it from queue ...
         commandQueue.erase (it);

         // unlock command queue ...
         mutex.unlock();

         // we don't block cookie requests!
         if (cmd.eReq != Kartina::REQ_COOKIE)
         {
            // wait until api becomes available ...
            while (pClient->busy () && iGo)
            {
               msleep(10);
            }
         }

         if (iGo)
         {
            switch (cmd.eReq)
            {
            case Kartina::REQ_CHANNELLIST:
               pClient->GetChannelList();
               break;
            case Kartina::REQ_COOKIE:
               pClient->GetCookie();
               break;
            case Kartina::REQ_EPG:
               pClient->GetEPG(cmd.iOptArg1, cmd.iOptArg2);
               break;
            case Kartina::REQ_SERVER:
               pClient->SetServer(cmd.sOptArg1);
               break;
            case Kartina::REQ_HTTPBUFF:
               pClient->SetHttpBuffer(cmd.iOptArg1);
               break;
            case Kartina::REQ_STREAM:
               pClient->GetStreamURL(cmd.iOptArg1, cmd.sOptArg1);
               break;
            case Kartina::REQ_TIMERREC:
               pClient->GetStreamURL(cmd.iOptArg1, cmd.sOptArg1, true);
               break;
            case Kartina::REQ_ARCHIV:
               pClient->GetArchivURL(cmd.sOptArg1, cmd.sOptArg2);
               break;
            case Kartina::REQ_TIMESHIFT:
               pClient->SetTimeShift(cmd.iOptArg1);
               break;
            case Kartina::REQ_GETTIMESHIFT:
               pClient->GetTimeShift();
               break;
            case Kartina::REQ_GET_SERVER:
               pClient->GetServer();
               break;
            case Kartina::REQ_LOGOUT:
               pClient->Logout();
               break;
            case Kartina::REQ_GETVODGENRES:
               pClient->GetVodGenres();
               break;
            case Kartina::REQ_GETVIDEOS:
               pClient->GetVideos(cmd.sOptArg1);
               break;
            case Kartina::REQ_GETVIDEOINFO:
               pClient->GetVideoInfo(cmd.iOptArg1, cmd.sOptArg1);
               break;
            case Kartina::REQ_GETVODURL:
               pClient->GetVodUrl(cmd.iOptArg1, cmd.sOptArg1);
               break;
            case Kartina::REQ_GETBITRATE:
               pClient->GetBitRate();
               break;
            case Kartina::REQ_SETBITRATE:
               pClient->SetBitRate(cmd.iOptArg1);
               break;
            case Kartina::REQ_SETCHAN_HIDE:
               pClient->setChanHide(cmd.sOptArg1, cmd.sOptArg2);
               break;
            case Kartina::REQ_SETCHAN_SHOW:
               pClient->setChanShow(cmd.sOptArg1, cmd.sOptArg2);
               break;
            case Kartina::REQ_CHANLIST_ALL:
               pClient->GetChannelList(cmd.sOptArg1);
               break;
            case Kartina::REQ_GET_VOD_MANAGER:
               pClient->getVodManager(cmd.sOptArg1);
               break;
            case Kartina::REQ_SET_VOD_MANAGER:
               pClient->setVodManager(cmd.sOptArg1, cmd.sOptArg2);
               break;
            case Kartina::REQ_ADD_VOD_FAV:
               pClient->addVodFav(cmd.iOptArg1, cmd.sOptArg1);
               break;
            case Kartina::REQ_REM_VOD_FAV:
               pClient->remVodFav(cmd.iOptArg1, cmd.sOptArg1);
               break;
            case Kartina::REQ_GET_VOD_FAV:
               pClient->getVodFav();
               break;
            case Kartina::REQ_SET_PCODE:
               pClient->setParentCode(cmd.sOptArg1, cmd.sOptArg2);
               break;
            default:
               break;
            }
         }
      }
      else
      {
         // queue empty - wait a little ...
         msleep(5);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: TriggerRequest
|  Begin: 19.01.2010 / 15:55:06
|  Author: Jo2003
|  Description: request kartina action
|
|  Parameters: action, action params
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::TriggerRequest(Kartina::EReq req, int iArg1, int iArg2)
{
   CommandQueue::SCmd cmd;

   cmd.eReq     = req;
   cmd.iOptArg1 = iArg1;
   cmd.iOptArg2 = iArg2;
   cmd.sOptArg1 = "";
   cmd.sOptArg2 = "";
   queueIn(cmd);
}

/* -----------------------------------------------------------------\
|  Method: TriggerRequest
|  Begin: 19.01.2010 / 15:55:06
|  Author: Jo2003
|  Description: request kartina action
|
|  Parameters: action, action params
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::TriggerRequest (Kartina::EReq req, const QString &sReq1, const QString &sReq2)
{
   CommandQueue::SCmd cmd;

   cmd.eReq     = req;
   cmd.iOptArg1 = -1;
   cmd.iOptArg2 = -1;
   cmd.sOptArg1 = sReq1;
   cmd.sOptArg2 = sReq2;
   queueIn(cmd);
}

/* -----------------------------------------------------------------\
|  Method: TriggerRequest
|  Begin: 30.05.2012
|  Author: Jo2003
|  Description: request kartina action
|
|  Parameters: action, action params
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::TriggerRequest (Kartina::EReq req, int iArg1, const QString &sArg1)
{
   CommandQueue::SCmd cmd;

   cmd.eReq     = req;
   cmd.iOptArg1 = iArg1;
   cmd.iOptArg2 = -1;
   cmd.sOptArg1 = sArg1;
   cmd.sOptArg2 = "";
   queueIn(cmd);
}

/* -----------------------------------------------------------------\
|  Method: slotReqChanList
|  Begin: 19.01.2010 / 15:55:44
|  Author: Jo2003
|  Description: channel list request, signalized by refresh timer
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::slotReqChanList()
{
   CommandQueue::SCmd cmd;

   cmd.eReq     = Kartina::REQ_CHANNELLIST;
   cmd.iOptArg1 = -1;
   cmd.iOptArg2 = -1;
   cmd.sOptArg1 = "";
   cmd.sOptArg2 = "";
   queueIn(cmd);
}

/* -----------------------------------------------------------------\
|  Method: queueIn
|  Begin: 28.09.2011
|  Author: Jo2003
|  Description: queue in new command, check for cookie and abort
|
|  Parameters: ref. to command
|
|  Returns: --
\----------------------------------------------------------------- */
void CWaitTrigger::queueIn (const CommandQueue::SCmd &cmd)
{
   // lock command queue ...
   mutex.lock();

   // abort means delete all pending requests ...
   if (cmd.eReq == Kartina::REQ_ABORT)
   {
      commandQueue.clear ();

      // abort pending request ...
      if (pClient->busy ())
      {
         pClient->abort ();
      }
   }
   // cookie means all pending requests are unusable ...
   else if (cmd.eReq == Kartina::REQ_COOKIE)
   {
      commandQueue.clear ();

      // abort pending request ...
      if (pClient->busy ())
      {
         pClient->abort ();
      }

      // queue in cookie request ...
      commandQueue.append (cmd);
   }
   else
   {
      // queue in request ...
      commandQueue.append (cmd);
   }

   // unlock command queue ...
   mutex.unlock();
}

/************************* History ***************************\
| $Log$
\*************************************************************/

