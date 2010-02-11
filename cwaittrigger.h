/*********************** Information *************************\
| $HeadURL$
| 
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:50:35
| 
| Last edited by: $Author$
| 
| $Id$
\*************************************************************/
#ifndef __011910__CWAITTRIGGER_H
   #define __011910__CWAITTRIGGER_H

#include <QThread>
#include "ckartinaclnt.h"

#include "clogfile.h"

/********************************************************************\
|  Class: CWaitTrigger
|  Date:  19.01.2010 / 15:51:08
|  Author: Jo2003
|  Description: thread class to trigger kartina.tv requests
|
\********************************************************************/
class CWaitTrigger : public QThread
{
   Q_OBJECT

public:
   CWaitTrigger(QObject * parent = 0);
   ~CWaitTrigger ();
   void SetKartinaClient (CKartinaClnt *pKartinaClient);
   void TriggerRequest (Kartina::EReq req, int iArg1 = 0, int iArg2 = 0);
   void TriggerRequest (Kartina::EReq req, const QString &sReq);
   void run ();
   void stop ();

public slots:
   void slotReqChanList ();

private:
   CKartinaClnt  *pClient;
   Kartina::EReq  eCurrReq;
   int iOptArg1, iOptArg2;
   QString sOptArg;
   int iGo;
};

#endif /* __011910__CWAITTRIGGER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

