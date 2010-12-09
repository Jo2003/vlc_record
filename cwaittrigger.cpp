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
   eCurrReq = Kartina::REQ_UNKNOWN;
   sOptArg  = "";
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
   while (iGo)
   {
      if (eCurrReq != Kartina::REQ_UNKNOWN)
      {
         // cookie request is send without busy check ...
         if (eCurrReq != Kartina::REQ_COOKIE)
         {
            // wait until api is available ...
            while (pClient->busy() && iGo)
            {
               msleep(10);
            }
         }

         if (iGo)
         {
            switch (eCurrReq)
            {
            case Kartina::REQ_CHANNELLIST:
               pClient->GetChannelList();
               break;
            case Kartina::REQ_COOKIE:
               pClient->GetCookie();
               break;
            case Kartina::REQ_EPG:
               pClient->GetEPG(iOptArg1, iOptArg2);
               break;
            case Kartina::REQ_SERVER:
               pClient->SetServer(sOptArg);
               break;
            case Kartina::REQ_HTTPBUFF:
               pClient->SetHttpBuffer(iOptArg1);
               break;
            case Kartina::REQ_STREAM:
               pClient->GetStreamURL(iOptArg1);
               break;
            case Kartina::REQ_TIMERREC:
               pClient->GetStreamURL(iOptArg1, true);
               break;
            case Kartina::REQ_ARCHIV:
               pClient->GetArchivURL(sOptArg);
               break;
            case Kartina::REQ_TIMESHIFT:
               pClient->SetTimeShift(iOptArg1);
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
            default:
               break;
            }
         }

         eCurrReq = Kartina::REQ_UNKNOWN;
      }

      msleep(5);
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
   iOptArg1 = iArg1;
   iOptArg2 = iArg2;
   sOptArg  = "";
   eCurrReq = req;
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
void CWaitTrigger::TriggerRequest(Kartina::EReq req, const QString &sReq)
{
   iOptArg1 = -1;
   iOptArg2 = -1;
   sOptArg  = sReq;
   eCurrReq = req;
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
   iOptArg1 = -1;
   iOptArg2 = -1;
   sOptArg  = "";
   eCurrReq = Kartina::REQ_CHANNELLIST;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

