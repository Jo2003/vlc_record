/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
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
|  Author: Jo2003
|  Description: create object, init values
|
|  Parameters: pointer to parent window
|
|  Returns: --
\----------------------------------------------------------------- */
CVlcCtrl::CVlcCtrl(const QString &path, QObject *parent) : QProcess(parent)
{
   bForcedTranslit = false;
   bTranslit       = false;
   pTranslit       = NULL;

   if (path != "")
   {
      LoadPlayerModule(path);
   }

   setStandardOutputFile(QString(PLAYER_LOG).arg(getenv(APPDATA)), QIODevice::Truncate);
   setProcessChannelMode(QProcess::MergedChannels);
   connect (this, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(slotStateChanged(QProcess::ProcessState)));
}

/* -----------------------------------------------------------------\
|  Method: ~CVlcCtrl / destructor
|  Begin: 01.02.2010 / 10:05:00
|  Author: Jo2003
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
|  Method: SetTranslitPointer
|  Begin: 11.02.2010 / 12:05:00
|  Author: Jo2003
|  Description: set translit class pointer
|
|  Parameters: pointer to CTranslit
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::SetTranslitPointer(CTranslit *pTr)
{
   pTranslit = pTr;
}

/* -----------------------------------------------------------------\
|  Method: SetTranslitSettings
|  Begin: 11.02.2010 / 12:05:00
|  Author: Jo2003
|  Description: set translit settings
|
|  Parameters: enable / disable translit
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::SetTranslitSettings(bool bTr)
{
   bTranslit = bTr;
}

/* -----------------------------------------------------------------\
|  Method: LoadPlayerModule
|  Begin: 11.02.2010 / 10:05:00
|  Author: Jo2003
|  Description: load player module
|
|  Parameters: path to player module
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CVlcCtrl::LoadPlayerModule(const QString &sPath)
{
   int iRV = -1;
   QFile fModule(sPath);
   QRegExp rx("^([^ =]*).*=.*<<(.*)>>.*$");

   if (fModule.open(QIODevice::ReadOnly))
   {
      // reset strings ...
      sHttpPlay       = "";
      sRtspPlay       = "";
      sHttpRec        = "";
      sRtspRec        = "";
      sHttpSilentRec  = "";
      sRtspSilentRec  = "";
      bForcedTranslit = false;

      QTextStream str(&fModule);
      QString     sLine;
      str.setCodec ("UTF-8");

      do
      {
         // read line by line from mod file ...
         sLine = str.readLine();

         if (rx.indexIn(sLine) > -1)
         {
            if (rx.cap(1) == CMD_PLAY_HTTP)
            {
               sHttpPlay = rx.cap(2);
            }
            else if (rx.cap(1) == CMD_PLAY_RTSP)
            {
               sRtspPlay = rx.cap(2);
            }
            else if (rx.cap(1) == CMD_REC_HTTP)
            {
               sHttpRec = rx.cap(2);
            }
            else if (rx.cap(1) == CMD_REC_RTSP)
            {
               sRtspRec = rx.cap(2);
            }
            else if (rx.cap(1) == CMD_SIL_REC_HTTP)
            {
               sHttpSilentRec = rx.cap(2);
            }
            else if (rx.cap(1) == CMD_SIL_REC_RTSP)
            {
               sRtspSilentRec = rx.cap(2);
            }
            else if (rx.cap(1) == FLAG_TRANSLIT)
            {
               bForcedTranslit = (rx.cap(2) == "yes") ? true : false;
            }
         }
      } while (!str.atEnd());

      // if the http play stuff is set
      // we assume that all is well ...
      if (sHttpPlay != "")
      {
         iRV = 0;
         mInfo(tr("Player module '%1' successfully parsed ...").arg(sPath));
      }

      fModule.close();
   }

   if (iRV)
   {
      QMessageBox::warning(NULL, tr("Warning!"), tr("Can't parse player module '%1'!").arg(sPath));
      mWarn(tr("Can't parse player module '%1'").arg(sPath));
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: start
|  Begin: 01.02.2010 / 10:05:00
|  Author: Jo2003
|  Description: start vlc
|
|  Parameters: command line, optional runtime in seconds
|
|  Returns: <= 0 --> error starting vlc
|           else --> process id
\----------------------------------------------------------------- */
Q_PID CVlcCtrl::start(const QString &sCmdLine, int iRunTime)
{
   Q_PID   vlcPid   = 0;

   if (IsRunning())
   {
      QMessageBox::warning(NULL, tr("Warning!"), tr("Player is already running!"));
   }
   else
   {
      mInfo(tr("Start player using folling command line:\n  --> %1").arg(sCmdLine));

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
|  Author: Jo2003
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
|  Author: Jo2003
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
|  Author: Jo2003
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
|  Author: Jo2003
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
|  Author: Jo2003
|  Description: create command line arguments for vlc ...
|
|  Parameters: vlc action enum, url, destination, mux
|
|  Returns: command line arguments as string
\----------------------------------------------------------------- */
QString CVlcCtrl::CreateClArgs (vlcctrl::eVlcAct eAct, const QString &sPlayer,
                                const QString &url, int iCacheTime,
                                const QString &dst, const QString &mux)
{
   QString   sCmdLine;
   QFileInfo dstInfo(dst);
   QString   sDstFile;
   QRegExp   rx ("^([a-zA-Z]{1})://(.*)$");

   if (bTranslit || bForcedTranslit)
   {
      sDstFile = QString("%1/%2.%3").arg(dstInfo.path())
                 .arg(pTranslit->CyrToLat(dstInfo.baseName()))
                 .arg(dstInfo.completeSuffix());
   }
   else
   {
      sDstFile = dst;
   }

   // fix destination path to avoid double slashes ...
   if (rx.indexIn(sDstFile) > -1)
   {
      // we found a double slash ...
      mInfo(tr("Remove double slash in target path ..."));
      sDstFile = QString("%1:/%2").arg(rx.cap(1)).arg(rx.cap(2));
   }

   switch (eAct)
   {
   // play stream using http protocol ...
   case vlcctrl::VLC_PLAY_HTTP:
      sCmdLine = sHttpPlay;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      break;
   // play stream using rtsp protocol ...
   case vlcctrl::VLC_PLAY_RTSP:
      sCmdLine = sRtspPlay;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      break;
   // record stream using http protocol ...
   case vlcctrl::VLC_REC_HTTP:
      sCmdLine = sHttpRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, sDstFile);
      break;
   // record stream using rtsp protocol ...
   case vlcctrl::VLC_REC_RTSP:
      sCmdLine = sRtspRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, sDstFile);
      break;
   // silently record stream using http protocol ...
   case vlcctrl::VLC_REC_HTTP_SILENT:
      sCmdLine = sHttpSilentRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, sDstFile);
      break;
   // silently record stream using rtsp protocol ...
   case vlcctrl::VLC_REC_RTSP_SILENT:
      sCmdLine = sRtspSilentRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, mux);
      sCmdLine.replace(TMPL_DST, sDstFile);
      break;
   default:
      break;
   }

   return sCmdLine;
}

/* -----------------------------------------------------------------\
|  Method: slotStateChanged
|  Begin: 02.02.2010 / 14:05:00
|  Author: Jo2003
|  Description: send signal when vlc starts / ends ...
|
|  Parameters: new process state
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::slotStateChanged(QProcess::ProcessState newState)
{
   switch (newState)
   {
   case QProcess::NotRunning:
      mInfo(tr("Player was started ..."));
      emit sigVlcEnds();
      break;
   case QProcess::Running:
      mInfo(tr("Player has ended ..."));
      emit sigVlcStarts();
      break;
   default:
      break;
   }
}

/************************* History ***************************\
| $Log$
\*************************************************************/
