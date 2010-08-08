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

// for folders ...
extern CDirStuff *pFolders;

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
   bForcedTranslit   = false;
   sFrcMx            = "no";
   bTranslit         = false;
   pTranslit         = NULL;
   bUseLibVlc        = false;
   libVlcPlayState   = IncPlay::PS_WTF;
   reqState          = IncPlay::PS_WTF;

   if (path != "")
   {
      LoadPlayerModule(path);
   }

   setStandardOutputFile(QString("%1/%2").arg(pFolders->getDataDir()).arg(PLAYER_LOG_FILE),
                         QIODevice::Truncate);

   setProcessChannelMode(QProcess::MergedChannels);

   connect (this, SIGNAL(stateChanged(QProcess::ProcessState)), this,
            SLOT(slotStateChanged(QProcess::ProcessState)));
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
|  Method: SetLibVLCPlayer
|  Begin: 03.03.2010 / 08:45:00
|  Author: Jo2003
|  Description: set libVlc player instance
|
|  Parameters: pointer to CPlayer
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::UseLibVlc(bool bUsage)
{
   bUseLibVlc = bUsage;
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

   if (sPath != "")
   {
      QFile fModule(sPath);
      QRegExp rx("^([^ =]*).*=.*<<(.*)>>.*$");

      if (fModule.open(QIODevice::ReadOnly))
      {
         // reset strings ...
         sLivePlay       = "";
         sArchPlay       = "";
         sLiveRec        = "";
         sArchRec        = "";
         sLiveSilentRec  = "";
         sArchSilentRec  = "";
         bForcedTranslit = false;
         sFrcMx          = "no";

         QTextStream str(&fModule);
         QString     sLine;
         str.setCodec ("UTF-8");

         do
         {
            // read line by line from mod file ...
            sLine = str.readLine();

            if (rx.indexIn(sLine) > -1)
            {
               if (rx.cap(1) == CMD_PLAY_LIVE)
               {
                  sLivePlay = rx.cap(2);
               }
               else if (rx.cap(1) == CMD_PLAY_ARCH)
               {
                  sArchPlay = rx.cap(2);
               }
               else if (rx.cap(1) == CMD_REC_LIVE)
               {
                  sLiveRec = rx.cap(2);
               }
               else if (rx.cap(1) == CMD_REC_ARCH)
               {
                  sArchRec = rx.cap(2);
               }
               else if (rx.cap(1) == CMD_SIL_REC_LIVE)
               {
                  sLiveSilentRec = rx.cap(2);
               }
               else if (rx.cap(1) == CMD_SIL_REC_ARCH)
               {
                  sArchSilentRec = rx.cap(2);
               }
               else if (rx.cap(1) == FLAG_TRANSLIT)
               {
                  bForcedTranslit = (rx.cap(2).toLower() == "yes") ? true : false;
               }
               else if (rx.cap(1) == FORCE_MUX)
               {
                  sFrcMx = rx.cap(2);
                  sFrcMx = sFrcMx.toLower();
               }
            }
         } while (!str.atEnd());

         // if the http play stuff is set
         // we assume that all is well ...
         if (sLivePlay != "")
         {
            iRV = 0;
            mInfo(tr("Player module '%1' successfully parsed ...").arg(sPath));
         }

         fModule.close();
      }
   }

   if (iRV)
   {
      QMessageBox::warning(NULL, tr("Warning!"), tr("Sorry! Can't parse player module!"));
      mWarn(tr("Can't parse player module!"));
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: start
|  Begin: 01.02.2010 / 10:05:00
|  Author: Jo2003
|  Description: start vlc
|
|  Parameters: command line, optional runtime in seconds, detach flag
|
|  Returns: <= 0 --> error starting vlc
|           else --> process id
\----------------------------------------------------------------- */
Q_PID CVlcCtrl::start(const QString &sCmdLine, int iRunTime, bool bDetach, IncPlay::ePlayStates req, bool bArchiv)
{
   Q_PID vlcPid = 0;

   // store what we request ...
   if (req != IncPlay::PS_WTF)
   {
      reqState = req;
   }

   // -------------------------------------------------------
   // use libVLC player ...
   // -------------------------------------------------------
   if (bUseLibVlc)
   {
      // do we allow control shortcuts ... ?
      bool bAllowCtrl = false;

      // allow control on archiv play only ...
      if (bArchiv && (req == IncPlay::PS_PLAY))
      {
         bAllowCtrl = true;
      }

      // detach isn't possible ...
      bDetach = false;

      // play media ...
      emit sigLibVlcPlayMedia(sCmdLine, bAllowCtrl);

      // assume that all is well ...
      vlcPid = (Q_PID)99; // anything but 0 ...
   }
   else
   {
      // -------------------------------------------------------
      // external player ...
      // -------------------------------------------------------
      if (bDetach)
      {
         // detach player ...
         mInfo(tr("Start player detached using folling command line:\n  --> %1").arg(sCmdLine));

         if (QProcess::startDetached(sCmdLine))
         {
            vlcPid = (Q_PID)99; // anything but 0 ...
         }
      }
      else
      {
         // player is under control. check if it is running ...
         QMessageBox::StandardButton btn = QMessageBox::Yes;

         if (IsRunning())
         {
            btn = QMessageBox::question(NULL, tr("Warning!"),
                                        tr("Player is already running! Do you want to proceed?"),
                                        QMessageBox::No | QMessageBox::Yes, QMessageBox::No);

            // we want to proceed -> stop former started player ...
            if (btn == QMessageBox::Yes)
            {
               // stop process kindly ...
               QProcess::terminate();

               if (!waitForFinished(1000))
               {
                  // process not stopped -> kill it ...
                  kill();
               }
            }
         }

         // now player shouldn't run ... however ...
         if (btn == QMessageBox::Yes)
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
         }
      }
   }

   // should we stop the player after a while ... ?
   if ((iRunTime > 0) && (vlcPid != 0) && !bDetach)
   {
      tRunTime.singleShot(iRunTime * 1000, this, SLOT(terminate()));
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
   bool bRV = false;

   if (bUseLibVlc)
   {
      switch (libVlcPlayState)
      {
      case IncPlay::PS_BUFFER:
      case IncPlay::PS_OPEN:
      case IncPlay::PS_PAUSE:
      case IncPlay::PS_PLAY:
         bRV = true;
         break;
      default:
         break;
      }
   }
   else
   {
      switch (state())
      {
      case QProcess::Running:
      case QProcess::Starting:
         bRV = true;
         break;

      default:
         break;
      }
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
   QString   sMux;
   QRegExp   rx ("^([a-zA-Z]{1})://(.*)$");

   // is muxing forced ... ?
   if (sFrcMx == "no")
   {
      // standard muxing ...
      sMux = mux;
   }
   else
   {
      mInfo(tr("Muxing forced to '%1'!").arg(sFrcMx));
      sMux = sFrcMx;
   }

   if (bTranslit || bForcedTranslit)
   {
      sDstFile = QString("%1/%2").arg(dstInfo.path())
                 .arg(pTranslit->CyrToLat(dstInfo.baseName()));
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
   case vlcctrl::VLC_PLAY_LIVE:
      sCmdLine = sLivePlay;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      break;
   // play stream using rtsp protocol ...
   case vlcctrl::VLC_PLAY_ARCH:
      sCmdLine = sArchPlay;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      break;
   // record stream using http protocol ...
   case vlcctrl::VLC_REC_LIVE:
      sCmdLine = sLiveRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, sMux);
      sCmdLine.replace(TMPL_DST, sDstFile);
      break;
   // record stream using rtsp protocol ...
   case vlcctrl::VLC_REC_ARCH:
      sCmdLine = sArchRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, sMux);
      sCmdLine.replace(TMPL_DST, sDstFile);
      break;
   // silently record stream using http protocol ...
   case vlcctrl::VLC_REC_LIVE_SILENT:
      sCmdLine = sLiveSilentRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, sMux);
      sCmdLine.replace(TMPL_DST, sDstFile);
      break;
   // silently record stream using rtsp protocol ...
   case vlcctrl::VLC_REC_ARCH_SILENT:
      sCmdLine = sArchSilentRec;
      sCmdLine.replace(TMPL_PLAYER, sPlayer);
      sCmdLine.replace(TMPL_URL, url);
      sCmdLine.replace(TMPL_CACHE, QString::number(iCacheTime));
      sCmdLine.replace(TMPL_MUX, sMux);
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
      mInfo(tr("Player has ended ..."));
      emit sigVlcEnds((int)reqState);
      break;
   case QProcess::Running:
      mInfo(tr("Player was started ..."));
      emit sigVlcStarts((int)reqState);
      break;
   default:
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: terminate [slot]
|  Begin: 03.03.2010 / 10:05:00
|  Author: Jo2003
|  Description: override terminate slot with own one ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::terminate()
{
   if (bUseLibVlc)
   {
      // This is no external process,
      // simply stop the player ...
      emit sigLibVlcStop();
      emit finished(0, QProcess::NormalExit);
   }
   else
   {
      // terminate only if it is running ...
      if (IsRunning())
      {
         QProcess::terminate();
      }
      else
      {
         emit finished(0, QProcess::NormalExit);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotLibVlcStateChange [slot]
|  Begin: 03.03.2010 / 12:05:00
|  Author: Jo2003
|  Description: state change from libVlc player ...
|
|  Parameters: new play state
|
|  Returns: --
\----------------------------------------------------------------- */
void CVlcCtrl::slotLibVlcStateChange (int ps)
{
   // something changed ... ?
   if (libVlcPlayState != (IncPlay::ePlayStates)ps)
   {
      libVlcPlayState = (IncPlay::ePlayStates)ps;

      mInfo(tr("libVLC reports new state %1").arg((int)ps));

      switch(libVlcPlayState)
      {
      case IncPlay::PS_OPEN:
         emit stateChanged(QProcess::Starting);
         break;

      case IncPlay::PS_PLAY:
         emit stateChanged(QProcess::Running);
         break;

      case IncPlay::PS_STOP:
      case IncPlay::PS_END:
      case IncPlay::PS_ERROR:
         emit stateChanged(QProcess::NotRunning);
         break;

      default:
         break;
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: withLibVLC [slot]
|  Begin: 24.06.2010 / 18:05:00
|  Author: Jo2003
|  Description: tell if libVLC is used
|
|  Parameters: --
|
|  Returns: true --> used
|          false --> not used
\----------------------------------------------------------------- */
bool CVlcCtrl::withLibVLC()
{
   return bUseLibVlc;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
