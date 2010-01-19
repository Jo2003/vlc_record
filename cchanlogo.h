/*********************** Information *************************\
| $HeadURL$
| 
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 16:16:25
| 
| Last edited by: $Author$
| 
| $Id$
\*************************************************************/
#ifndef __011810__CCHANLOGO_H
   #define __011810__CCHANLOGO_H

#include <QHttp>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QBuffer>
#include <QMutex>
#include <QDir>

#include "ckartinaclnt.h"
#include "ckartinaxmlparser.h"
#include "clogfile.h"

#include "defdef.h"

// for logging ...
extern CLogFile VlcLog;

/********************************************************************\
|  Class: CChanLogo
|  Date:  18.01.2010 / 16:16:59
|  Author: Joerg Neubert
|  Description: helper class to download channel logos
|
\********************************************************************/
class CChanLogo : public QHttp
{
   Q_OBJECT

public:
   CChanLogo();
   virtual ~CChanLogo();
   void SetChanList (const QVector<cparser::SChan> &list);
   bool IsRunning () { return bRun; }
   QString GetLogoPath () { return sPath; }

signals:
   void sigLogosReady ();

protected:
   void StartDownLoad ();

private:
   QString sPath;
   QBuffer dataBuffer;
   QVector<cparser::SChan> chanList;
   QVector<cparser::SChan>::const_iterator cit;
   QMutex mutex;
   bool bRun;
   bool bIsAnswer;
   int  iReq;

private slots:
   // void slotResp (bool err);
   void slotCheckResp (int iReqID, bool err);
};

#endif /* __011810__CCHANLOGO_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

