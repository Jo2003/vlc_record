/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
|
| Begin: 01.02.2010 / 10:50:35
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cvlcctrl.h"

// logging stuff ...
extern CLogFile VlcLog;

/* -----------------------------------------------------------------\
|  Method: CVlcCtrl / constructor
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: create object, init values
|
|  Parameters: pointer to parent window
|
|  Returns: --
\----------------------------------------------------------------- */
CVlcCtrl::CVlcCtrl(const QString &path, QObject *parent) : QProcess(parent)
{
   sProgPath = path;
}

/* -----------------------------------------------------------------\
|  Method: ~CVlcCtrl / destructor
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: clean at destruction, stop process, if running
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CVlcCtrl::~CVlcCtrl()
{
   if (IsRunning())
   {
      terminate();
   }
}

/* -----------------------------------------------------------------\
|  Method: SetProgPath
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: set program path to vlc
|
|  Parameters: path
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::SetProgPath(const QString &str)
{
   sProgPath = str;
}

/* -----------------------------------------------------------------\
|  Method: start
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: start vlc
|
|  Parameters: command line args, optional runtime in seconds
|
|  Returns: <= 0 --> error starting vlc
|           else --> process id
\----------------------------------------------------------------- */
Q_PID CVlcCtrl::start(const QString &clargs, int iRunTime)
{
   Q_PID   vlcPid   = 0;
   QString sCmdLine = sProgPath;
   sCmdLine += QString(" %1").arg(clargs);

   QProcess::start(sCmdLine);

   if (waitForStarted(2000))
   {
      vlcPid = pid();
   }
   else
   {
      kill();
   }

   if (iRunTime > 0)
   {
      tRunTime.singleShot(iRunTime * 1000, this, SLOT(terminate()));
   }

   return vlcPid;
}

/* -----------------------------------------------------------------\
|  Method: stop
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: stop vlc
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::stop()
{
   if (IsRunning())
   {
      terminate();
   }
}

/* -----------------------------------------------------------------\
|  Method: CancelTimer
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: cancel run time timer
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::CancelTimer()
{
   tRunTime.stop();
}

/* -----------------------------------------------------------------\
|  Method: IsRunning
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: is vlc running ?
|
|  Parameters: --
|
|  Returns: true --> running
|          false --> not running
\----------------------------------------------------------------- */
bool CVlcCtrl::IsRunning()
{
   bool bRV;

   switch (state())
   {
   case QProcess::Running:
   case QProcess::Starting:
      bRV = true;
      break;

   default:
      bRV = false;
      break;
   }

   return bRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
