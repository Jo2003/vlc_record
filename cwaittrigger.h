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
#include <QVector>
#include <QMutex>
#include "ckartinaclnt.h"

#include "clogfile.h"

namespace CommandQueue
{
   struct SCmd
   {
      Kartina::EReq eReq;
      QString       sOptArg1;
      QString       sOptArg2;
      int           iOptArg1;
      int           iOptArg2;
   };
}

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
   void TriggerRequest (Kartina::EReq req, const QString &sReq1, const QString &sReq2 = QString());
   void stop ();

protected:
   virtual void run ();
   void queueIn(const CommandQueue::SCmd &cmd);

public slots:
   void slotReqChanList ();

private:
   CKartinaClnt *pClient;
   int           iGo;
   QMutex        mutex;
   QVector<CommandQueue::SCmd> commandQueue;
};

#endif /* __011910__CWAITTRIGGER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

