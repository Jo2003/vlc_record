/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.02.2010 / 10:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cplayer.h"
#include "ui_cplayer.h"

// log file functions ...
extern CLogFile VlcLog;

const char *CPlayer::pAspectRatio[] = {
   "4:3","16:9","16:10","1:1", "5:4","2.35"
};

/* -----------------------------------------------------------------\
|  Method: CPlayer / constructor
|  Begin: 24.02.2010 / 12:17:51
|  Author: Jo2003
|  Description: init values
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CPlayer::CPlayer(QWidget *parent) : QWidget(parent), ui(new Ui::CPlayer)
{
   ui->setupUi(this);

   // nothing playing so far ...
   pMedia       = NULL;
   pMediaPlayer = NULL;
   pVlcInstance = NULL;
   pEMMedia     = NULL;
   pEMPlay      = NULL;
   pLibVlcLog   = NULL;

#ifndef QT_NO_DEBUG
   uiVerboseLevel = 3;
#else
   uiVerboseLevel = 1;
#endif /* QT_NO_DEBUG */

   // init exception structure ...
   libvlc_exception_init(&vlcExcpt);

   // connect volume slider with volume change function ...
   connect(ui->volSlider, SIGNAL(sliderMoved(int)), this, SLOT(slotChangeVolume(int)));

   // connect state change signal with state label in dialog ...
   connect(this, SIGNAL(sigStateChg(QString)), ui->labState, SLOT(setText(QString)));

   // do periodical logging ...
   connect(&poller, SIGNAL(timeout()), this, SLOT(slotLibVLCLog()));

   // fill aspect ratio box ...
   for(uint i = 0; i < sizeof(pAspectRatio) / sizeof(const char *); i++)
   {
      ui->cbxAspect->addItem(pAspectRatio[i]);
   }

   poller.start(1000);
}

/* -----------------------------------------------------------------\
|  Method: ~CPlayer / destructor
|  Begin: 24.02.2010 / 12:17:51
|  Author: Jo2003
|  Description: clean on destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CPlayer::~CPlayer()
{
   releasePlayer();
   delete ui;
}

/* -----------------------------------------------------------------\
|  Method: setPlugInPath
|  Begin: 02.03.2010 / 14:17:51
|  Author: Jo2003
|  Description: set the plugin path ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::setPlugInPath(const QString &sPath)
{
   mInfo(tr("Set PlugIn path to '%1'").arg(sPath));
   sPlugInPath = sPath;
}

/* -----------------------------------------------------------------\
|  Method: releasePlayer
|  Begin: 24.02.2010 / 12:25:51
|  Author: Jo2003
|  Description: release all the vlc player stuff
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::releasePlayer()
{
   // release player ...
   if (pMediaPlayer)
   {
      libvlc_media_player_stop (pMediaPlayer, &vlcExcpt);
      libvlc_media_player_release(pMediaPlayer);
      pMediaPlayer = NULL;
   }

   // release media ...
   if (pMedia)
   {
      libvlc_media_release(pMedia);
      pMedia       = NULL;
   }

   // close log if opened ...
   if (pLibVlcLog)
   {
      libvlc_log_close (pLibVlcLog, &vlcExcpt);
      pLibVlcLog = NULL;
   }

   // release vlc instance ...
   if (pVlcInstance)
   {
      libvlc_release (pVlcInstance);
      pVlcInstance = NULL;
   }
}

/* -----------------------------------------------------------------\
|  Method: createArgs
|  Begin: 24.02.2010 / 14:25:51
|  Author: Jo2003
|  Description: create argument array, free it with freeArgs
|
|  Parameters: ref. to argument list, ref to vlcArgs struct
|
|  Returns: number of args
|
\----------------------------------------------------------------- */
int CPlayer::createArgs (const QStringList &lArgs, Ui::vlcArgs &args)
{
   int i         = 0;
   args.argArray = new char *[lArgs.count()];

   if (args.argArray)
   {
      args.argc = lArgs.count();

      for (i = 0; i < args.argc; i++)
      {
         args.argArray[i] = new char[lArgs[i].size() + 1];

         if (args.argArray[i])
         {
            strcpy (args.argArray[i], lArgs[i].toAscii().constData());
         }
      }
   }

   return i;
}

/* -----------------------------------------------------------------\
|  Method: freeArgs
|  Begin: 24.02.2010 / 15:25:51
|  Author: Jo2003
|  Description: free argument array
|
|  Parameters: ref. to vlcArgs struct
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::freeArgs (Ui::vlcArgs &args)
{
   for (int i = 0; i < args.argc; i++)
   {
      delete [] args.argArray[i];
   }

   delete [] args.argArray;
   args.argArray = NULL;
   args.argc     = 0;
}

/* -----------------------------------------------------------------\
|  Method: initPlayer
|  Begin: 24.02.2010 / 14:00:51
|  Author: Jo2003
|  Description: init player with arguments
|
|  Parameters: list of arguments
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::initPlayer(QStringList &slArgs)
{
   int iRV = -1;
   Ui::vlcArgs args;

   releasePlayer();

   if (slArgs.size() == 0)
   {
      // no arguments given --> create standard arguments ...
      slArgs << "-I" << "dummy" << "--ignore-config";
   }

   // is plugin path set ...
   if (sPlugInPath.length() > 0)
   {
      slArgs << QString("--plugin-path=\"%1\"").arg(sPlugInPath);
   }

   // don't catch key press events ... (should work in patched libVLC)
   slArgs << "--vout-event=3";

   // fill vlcArgs struct ...
   createArgs(slArgs, args);

   // arguments there ... ?
   if (args.argArray)
   {
      //create a new libvlc instance
      libvlc_exception_clear(&vlcExcpt);
      pVlcInstance = libvlc_new(args.argc, args.argArray, &vlcExcpt);
      iRV = raise (&vlcExcpt);

      if (!iRV)
      {
         // set verbose mode ...
         libvlc_set_log_verbosity (pVlcInstance, uiVerboseLevel, &vlcExcpt);
         iRV = raise (&vlcExcpt);
      }

      if (!iRV)
      {
         // open logger ...
         pLibVlcLog = libvlc_log_open(pVlcInstance, &vlcExcpt);
         iRV = raise (&vlcExcpt);
      }

      if (!iRV)
      {
         // Create a media player playing environement
         pMediaPlayer = libvlc_media_player_new (pVlcInstance, &vlcExcpt);
         iRV = raise (&vlcExcpt);
      }

      if (!iRV)
      {
         // add player to window ...
         connect_to_wnd(pMediaPlayer, ui->fVideo->winId(), &vlcExcpt);
         iRV = raise(&vlcExcpt);
      }

      if (!iRV)
      {
         // get volume ...
         ui->volSlider->setSliderPosition(libvlc_audio_get_volume (pVlcInstance, &vlcExcpt));
         iRV = raise(&vlcExcpt);
      }

      if (!iRV)
      {
         // get event manager ...
         pEMPlay = libvlc_media_player_event_manager(pMediaPlayer, &vlcExcpt);
         iRV = raise(&vlcExcpt);
      }

      if (!iRV)
      {
         libvlc_event_attach(pEMPlay, libvlc_MediaPlayerEncounteredError, CPlayer::eventCallback,
                             (void *)this, &vlcExcpt);
         iRV = raise(&vlcExcpt);
      }

      freeArgs(args);
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: setMedia
|  Begin: 24.02.2010 / 16:00:51
|  Author: Jo2003
|  Description: set media
|
|  Parameters: media MRL
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::setMedia(const QString &sMrl)
{
   int iRV = -1;

   // is player playing ... ?
   libvlc_exception_clear(&vlcExcpt);
   libvlc_media_t *curMedia = libvlc_media_player_get_media (pMediaPlayer, &vlcExcpt);

   // if playing, stop and then release media ...
   if (curMedia)
   {
      if (isPlaying())
      {
         libvlc_exception_clear(&vlcExcpt);
         libvlc_media_player_stop(pMediaPlayer, &vlcExcpt);
      }

      // release media ...
      libvlc_media_release (curMedia);
      pMedia = NULL;
   }

   // create new media ...
   libvlc_exception_clear(&vlcExcpt);
   pMedia = libvlc_media_new (pVlcInstance, sMrl.toAscii().constData(), &vlcExcpt);
   iRV = raise(&vlcExcpt);

   if (!iRV)
   {
      // get event manager ...
      pEMMedia = libvlc_media_event_manager(pMedia, &vlcExcpt);
      iRV = raise(&vlcExcpt);
   }

   if (!iRV)
   {
      // attach media state change event ...
      libvlc_event_attach(pEMMedia, libvlc_MediaStateChanged, CPlayer::eventCallback, (void *)this, &vlcExcpt);
      iRV = raise(&vlcExcpt);
   }

   if (!iRV)
   {
      // add media ...
      libvlc_media_player_set_media (pMediaPlayer, pMedia, &vlcExcpt);
      iRV = raise(&vlcExcpt);
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: slotChangeVolume
|  Begin: 28.02.2010 / 19:00:51
|  Author: Jo2003
|  Description: set volume
|
|  Parameters: new volume
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotChangeVolume(int newVolume)
{
   if (!newVolume)
   {
      ui->labSound->setPixmap(QPixmap(":/player/sound_off"));
   }
   else
   {
      ui->labSound->setPixmap(QPixmap(":/player/sound_on"));
   }

   if (pVlcInstance)
   {
      libvlc_exception_clear(&vlcExcpt);
      libvlc_audio_set_volume (pVlcInstance, newVolume, &vlcExcpt);
      raise(&vlcExcpt);
   }
}

/* -----------------------------------------------------------------\
|  Method: play
|  Begin: 24.02.2010 / 16:00:51
|  Author: Jo2003
|  Description: play media
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::play()
{
   int iRV = 0;

   if (pMediaPlayer)
   {
      // reset exception stuff ...
      libvlc_exception_clear(&vlcExcpt);
      libvlc_media_player_play (pMediaPlayer, &vlcExcpt);
      iRV = raise(&vlcExcpt);
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: stop
|  Begin: 24.02.2010 / 16:00:51
|  Author: Jo2003
|  Description: stop playing
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::stop()
{
   int iRV = 0;

   if (pMediaPlayer && isPlaying())
   {
      libvlc_exception_clear(&vlcExcpt);
      libvlc_media_player_stop (pMediaPlayer, &vlcExcpt);
      iRV = raise(&vlcExcpt);
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: pause
|  Begin: 24.02.2010 / 16:00:51
|  Author: Jo2003
|  Description: pause / unpause playing
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::pause()
{
   int iRV = 0;

   if (pMediaPlayer)
   {
      // reset exception stuff ...
      libvlc_exception_clear(&vlcExcpt);

      if (isPlaying() && libvlc_media_player_can_pause(pMediaPlayer, &vlcExcpt))
      {
         iRV = raise(&vlcExcpt);

         if (!iRV)
         {
            libvlc_media_player_pause(pMediaPlayer, &vlcExcpt);
            iRV = raise(&vlcExcpt);
         }
      }
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: playMedia
|  Begin: 03.03.2010 / 09:16:51
|  Author: Jo2003
|  Description: init player, set media, start play
|
|  Parameters: complete command line
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::playMedia(const QString &sCmdLine)
{
   int iRV;

   // get MRL ...
   QString     sMrl  = sCmdLine.section(";;", 0, 0);
   // QString     sMrl  = "h:/Documents/Videos/BR-test.ts";

   // get player arguments ...
   QStringList lArgs = sCmdLine.mid(sCmdLine.indexOf(";;", 0))
                          .split(";;", QString::SkipEmptyParts);

   mInfo(tr("starting libVLC play of:\n  --> %2\n  --> with following arguments: %1")
         .arg(lArgs.join(" ")).arg(sMrl));

   iRV = initPlayer(lArgs);

   if (!iRV)
   {
      iRV = setMedia(sMrl);
   }

   if (!iRV)
   {
      iRV = play();
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: isPlaying
|  Begin: 03.03.2010 / 09:40:51
|  Author: Jo2003
|  Description: is player playing ?
|
|  Parameters: --
|
|  Returns: true --> playing
|          false --> not playing
\----------------------------------------------------------------- */
bool CPlayer::isPlaying()
{
   bool bRV = false;

   if (pMediaPlayer)
   {
      int iRV;

      // reset exception stuff ...
      libvlc_exception_clear(&vlcExcpt);
      iRV = libvlc_media_player_is_playing (pMediaPlayer, &vlcExcpt);

      if (!raise(&vlcExcpt))
      {
         bRV = (iRV) ? true : false;
      }
   }

   return bRV;
}

/* -----------------------------------------------------------------\
|  Method: changeEvent
|  Begin: 24.02.2010 / 11:46:10
|  Author: Jo2003
|  Description: catch event when language changes
|
|  Parameters: pointer to event
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::changeEvent(QEvent *e)
{
   QWidget::changeEvent(e);
   switch (e->type()) {
   case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
   default:
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: raise
|  Begin: 24.02.2010 / 11:46:10
|  Author: Jo2003
|  Description: check if there is any problem, if so display
|               error message
|  Parameters: pointer to exception
|
|  Returns: 0 ==> ok
|        else ==> any error
\----------------------------------------------------------------- */
int CPlayer::raise(libvlc_exception_t * ex)
{
   int iRV = libvlc_exception_raised (ex);
   if (iRV)
   {
      QMessageBox::critical(this, tr("LibVLC Error!"),
                            tr("LibVLC reports following error:\n%1")
                            .arg(libvlc_exception_get_message(ex)));
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: eventCallback
|  Begin: 01.03.2010 / 11:00:10
|  Author: Jo2003
|  Description: callback for vlc events
|
|  Parameters: pointer to event raised, pointer to player class
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::eventCallback(const libvlc_event_t *ev, void *player)
{
   CPlayer *pPlayer = (CPlayer *)player;

   switch (ev->type)
   {
   // media change events ...
   case libvlc_MediaStateChanged:
      {
         // media state changed ... what is the new state ...
         switch (ev->u.media_state_changed.new_state)
         {
         case libvlc_Opening:
            emit pPlayer->sigPlayState((int)IncPlay::PS_OPEN);
            emit pPlayer->sigStateChg(tr("OPENING"));
            break;

         case libvlc_Buffering:
            emit pPlayer->sigPlayState((int)IncPlay::PS_BUFFER);
            emit pPlayer->sigStateChg(tr("BUFFERING"));
            break;

         case libvlc_Playing:
            emit pPlayer->sigPlayState((int)IncPlay::PS_PLAY);
            emit pPlayer->sigStateChg(tr("PLAYING"));
            break;

         case libvlc_Paused:
            emit pPlayer->sigPlayState((int)IncPlay::PS_PAUSE);
            emit pPlayer->sigStateChg(tr("PAUSED"));
            break;

         case libvlc_Stopped:
            emit pPlayer->sigPlayState((int)IncPlay::PS_STOP);
            emit pPlayer->sigStateChg(tr("STOPPED"));
            break;

         case libvlc_Ended:
            emit pPlayer->sigPlayState((int)IncPlay::PS_END);
            emit pPlayer->sigStateChg(tr("ENDED"));
            break;

         case libvlc_Error:
            emit pPlayer->sigPlayState((int)IncPlay::PS_ERROR);
            emit pPlayer->sigStateChg(tr("ERROR"));
            break;

         default:
            break;
         }
      }
      break;

   // player error event ...
   case libvlc_MediaPlayerEncounteredError:
      emit pPlayer->sigPlayState((int)IncPlay::PS_ERROR);
      emit pPlayer->sigStateChg(tr("ERROR"));
      break;

   default:
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: slotDoLog
|  Begin: 02.03.2010 / 08:30:10
|  Author: Jo2003
|  Description: check libvlc_log for new entries in write into
|               log file
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotLibVLCLog()
{
   int     iRV;

   // do we have a logger handle ... ?
   if (pLibVlcLog)
   {
      // how many entries in log ... ?
      uint uiEntryCount = 0;

      // reset exception stuff ...
      libvlc_exception_clear(&vlcExcpt);

      // are there entries available ... ?
      uiEntryCount = libvlc_log_count (pLibVlcLog, &vlcExcpt);
      iRV          = raise(&vlcExcpt);

      // no error and entries there ...
      if (!iRV && uiEntryCount)
      {
         // log message buffer ...
         libvlc_log_message_t   logMsg;
         libvlc_log_message_t  *pLogMsg;

         // get iterator to go through log entries ...
         libvlc_log_iterator_t *it = libvlc_log_get_iterator(pLibVlcLog, &vlcExcpt);
         iRV                       = raise (&vlcExcpt);

         // while there are entries ...
         while (!iRV)
         {
            // get log message presented by log iterator ...
            pLogMsg = libvlc_log_iterator_next (it, &logMsg, &vlcExcpt);
            iRV     = raise(&vlcExcpt);

            if (!iRV)
            {
               // build log message ...
               mInfo(tr("Name: \"%1\", Type: \"%2\", Severity: %3\n  --> %4")
                      .arg(pLogMsg->psz_name).arg(pLogMsg->psz_type)
                      .arg(pLogMsg->i_severity).arg(pLogMsg->psz_message));

               // is there a next entry ... ?
               if (!libvlc_log_iterator_has_next(it, &vlcExcpt))
               {
                  // no --> break while ...
                  iRV = 1;
               }
            }
         }

         // delete all log entries ...
         libvlc_log_clear(pLibVlcLog, &vlcExcpt);

         // free log iterator ...
         libvlc_log_iterator_free (it, &vlcExcpt);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: on_cbxAspect_currentIndexChanged
|  Begin: 08.03.2010 / 09:55:10
|  Author: Jo2003
|  Description: set new aspect ration ...
|
|  Parameters: new aspect ratio as string ...
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::on_cbxAspect_currentIndexChanged(QString str)
{
   if (pMediaPlayer)
   {
      // reset exception stuff ...
      libvlc_exception_clear(&vlcExcpt);

      // set new aspect ratio ...
      libvlc_video_set_aspect_ratio(pMediaPlayer, str.toAscii().data(), &vlcExcpt);

      raise(&vlcExcpt);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotToggleFullScreen
|  Begin: 08.03.2010 / 09:55:10
|  Author: Jo2003
|  Description: toggle fullscreen mode ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
int CPlayer::slotToggleFullScreen()
{
   int iRV = -1;

   if (pMediaPlayer)
   {
      // reset exception stuff ...
      libvlc_exception_clear(&vlcExcpt);

      // set new aspect ratio ...
      libvlc_toggle_fullscreen (pMediaPlayer, &vlcExcpt);

      iRV = raise(&vlcExcpt);
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: slotToggleAspectRatio
|  Begin: 08.03.2010 / 15:10:10
|  Author: Jo2003
|  Description: switch aspect ratio to next one ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
int CPlayer::slotToggleAspectRatio()
{
   int iRV = -1;
   if (pMediaPlayer)
   {
      int idx = ui->cbxAspect->currentIndex();
      idx ++;

      // if end reached, start with index 0 ...
      if (idx >= ui->cbxAspect->count())
      {
         idx = 0;
      }

      // set new aspect ratio ...
      ui->cbxAspect->setCurrentIndex(idx);

      iRV = 0;
   }

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

