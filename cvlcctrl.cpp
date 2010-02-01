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
   sProgPath  = path;
   iCacheTime = 0;
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
   // since the process isn't started detached, it will be closed
   // when vlc-record closes ...
   if (IsRunning())
   {
      // close vlc ...
      terminate();

      // wait until vlc closes ...
      waitForFinished(3000);
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

   if (IsRunning())
   {
      QMessageBox::warning(NULL, tr("Warning!"), tr("Vlc Player is already running!"));
   }
   else
   {
      QString sCmdLine = QString("\"%1\" %2").arg(sProgPath).arg(clargs);

      mInfo(tr("Start vlc player using folling command line:\n%1").arg(sCmdLine));

      QProcess::start(sCmdLine);

      if (waitForStarted(3000))
      {
         vlcPid = pid();
      }
      else
      {
         kill();
      }

      // set runtime ...
      if (iRunTime > 0)
      {
         tRunTime.singleShot(iRunTime * 1000, this, SLOT(terminate()));
      }
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
|  Method: SetTimer
|  Begin: 01.02.2010 / 10:05:00
|  Author: Joerg Neubert
|  Description: (re) set stop timer
|
|  Parameters: new timeout (sec.)
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::SetTimer(uint uiTime)
{
   tRunTime.stop();
   tRunTime.singleShot(uiTime * 1000, this, SLOT(terminate()));
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

/* -----------------------------------------------------------------\
|  Method: CreateClArgs
|  Begin: 01.02.2010 / 14:05:00
|  Author: Joerg Neubert
|  Description: create command line arguments for vlc ...
|
|  Parameters: vlc action enum, url, destination, mux
|
|  Returns: command line arguments as string
\----------------------------------------------------------------- */
QString CVlcCtrl::CreateClArgs(vlcctrl::eVlcAct eAct,
                               const QString &url,
                               const QString &dst,
                               const QString &mux)
{
   QString sCmdLine;

   switch (eAct)
   {
   // play stream using http protocol ...
   case vlcctrl::VLC_PLAY_HTTP:
      sCmdLine = TMPL_PLAY_HTTP;
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      break;
   // play stream using rtsp protocol ...
   case vlcctrl::VLC_PLAY_RTSP:
      sCmdLine = TMPL_PLAY_RTSP;
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      break;
   // record stream using http protocol ...
   case vlcctrl::VLC_REC_HTTP:
      sCmdLine = TMPL_PLAY_HTTP TMPL_REC;
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, dst);
      break;
   // record stream using rtsp protocol ...
   case vlcctrl::VLC_REC_RTSP:
      sCmdLine = TMPL_PLAY_RTSP TMPL_REC;
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, dst);
      break;
   // silently record stream using http protocol ...
   case vlcctrl::VLC_REC_HTTP_SILENT:
      sCmdLine = TMPL_PLAY_HTTP TMPL_SILENT_REC;
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, dst);
      break;
   // silently record stream using rtsp protocol ...
   case vlcctrl::VLC_REC_RTSP_SILENT:
      sCmdLine = TMPL_PLAY_RTSP TMPL_SILENT_REC;
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, dst);
      break;
   default:
      break;
   }

   return sCmdLine;
}

/* -----------------------------------------------------------------\
|  Method: SetCache
|  Begin: 01.02.2010 / 14:05:00
|  Author: Joerg Neubert
|  Description: set cache time
|
|  Parameters: new cache time value
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::SetCache(int iTime)
{
   iCacheTime = iTime;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
