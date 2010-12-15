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

// storage db ...
extern CVlcRecDB *pDb;

// global showinfo class ...
extern CShowInfo showInfo;

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
   pMediaPlayer  = NULL;
   pVlcInstance  = NULL;
   pEMPlay       = NULL;
   pLibVlcLog    = NULL;
   pSettings     = NULL;
   pTrigger      = NULL;
   bCtrlStream   = false;
   bSpoolPending = true;

   // set log poller to single shot ...
   poller.setSingleShot(true);

   // set aspect shot timer to single shot ...
   tAspectShot.setSingleShot (true);
   tAspectShot.setInterval (800);

#ifdef QT_NO_DEBUG
   uiVerboseLevel = 1;
#else
   uiVerboseLevel = 3;
#endif /* QT_NO_DEBUG */

   // connect volume slider with volume change function ...
   connect(ui->volSlider, SIGNAL(sliderMoved(int)), this, SLOT(slotChangeVolume(int)));

   // connect double click signal from videoframe with fullscreen toggle ...
   connect(ui->fVideo, SIGNAL(sigToggleFullscreen()), this, SLOT(slotToggleFullScreen()));

   // connect slider timer with slider position slot ...
   connect(&sliderTimer, SIGNAL(timeout()), this, SLOT(slotUpdateSlider()));

   // do periodical logging ...
   connect(&poller, SIGNAL(timeout()), this, SLOT(slotLibVLCLog()));

   // connect aspect shot timer with aspect change function ...
   connect(&tAspectShot, SIGNAL(timeout()), this, SLOT(slotStoredAspectCrop()));

   // connect aspect trigger signal with timer start ...
   connect(this, SIGNAL(sigTriggerAspectChg()), &tAspectShot, SLOT(start()));

   poller.start(1000);
   sliderTimer.start(500);
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
   // libvlc stuff already released in function "cleanExit"
   // which is called from recorder.cpp / closeEvent
   delete ui;
}

/* -----------------------------------------------------------------\
|  Method: cleanExit
|  Begin: 23.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: pseudo destructor should be called from outside ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::cleanExit()
{
   // stop timer ...
   poller.stop();
   sliderTimer.stop();

   /**
    * Note: Closing the log here leads to
    * segmentation faults. Also disabling
    * and cleaning the log first doesn't help.
    * Therefore I assume that this is a libVLC
    * problem. But since this function only is called
    * when program ends, not closing the log shouldn't
    * be a big problem!
    */

   stop();

   if (pMediaPlayer)
   {
      libvlc_media_player_release (pMediaPlayer);
   }

   if (pVlcInstance)
   {
      libvlc_release(pVlcInstance);
   }
}

/* -----------------------------------------------------------------\
|  Method: setShortCuts
|  Begin: 24.03.2010 / 14:17:51
|  Author: Jo2003
|  Description: store a pointer to shortcuts vector
|
|  Parameters: pointer to shortcuts vector
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::setShortCuts(QVector<CShortcutEx *> *pvSc)
{
   ui->fVideo->setShortCuts(pvSc);
}

/* -----------------------------------------------------------------\
|  Method: setSettings
|  Begin: 16.06.2010 / 14:17:51
|  Author: Jo2003
|  Description: store a pointer to settings class
|
|  Parameters: pointer to settings class
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::setSettings(CSettingsDlg *pDlg)
{
   pSettings = pDlg;
}

/* -----------------------------------------------------------------\
|  Method: setTrigger
|  Begin: 16.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: store a pointer to trigger class
|
|  Parameters: pointer to trigger class
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::setTrigger(CWaitTrigger *pTrig)
{
   pTrigger = pTrig;
}

/* -----------------------------------------------------------------\
|  Method: initPlayer
|  Begin: 24.02.2010 / 14:00:51
|  Author: Jo2003
|  Description: init player with arguments
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::initPlayer()
{
   int iRV = -1;

   // reset crop and aspect cbx ... because it should show the state
   // as used ...
   ui->cbxAspect->setCurrentIndex(0);
   ui->cbxCrop->setCurrentIndex(0);

   //create a new libvlc instance
   pVlcInstance = libvlc_new(0, NULL);

   if (pVlcInstance)
   {
      // set verbose mode ...
      libvlc_set_log_verbosity (pVlcInstance, uiVerboseLevel);

      // get logger and mediaplayer ...
      pLibVlcLog   = libvlc_log_open(pVlcInstance);
      pMediaPlayer = libvlc_media_player_new (pVlcInstance);
   }

   if (pLibVlcLog && pMediaPlayer)
   {
      // add player to window ...
      connect_to_wnd(pMediaPlayer, ui->fVideo->winId());

      // get volume ...
      ui->volSlider->setSliderPosition(libvlc_audio_get_volume (pMediaPlayer));

      // get event manager ...
      pEMPlay = libvlc_media_player_event_manager(pMediaPlayer);

      // switch off handling of hotkeys ...
      libvlc_video_set_key_input(pMediaPlayer, 0);

      libvlc_video_set_mouse_input(pMediaPlayer, 0);
   }

   // if we've got the event manager, register for some events ...
   if (pEMPlay)
   {
      iRV  = libvlc_event_attach(pEMPlay, libvlc_MediaPlayerEncounteredError,
                                CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerOpening,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerBuffering,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerPlaying,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerPaused,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerStopped,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerEndReached,
                                 CPlayer::eventCallback, (void *)this);
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
      libvlc_audio_set_volume (pMediaPlayer, newVolume);
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
      libvlc_media_player_play (pMediaPlayer);
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

   if (pMediaPlayer)
   {
      libvlc_media_player_stop (pMediaPlayer);
   }

   stopPlayTimer();

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

   if (pMediaPlayer && bCtrlStream)
   {
      libvlc_media_player_pause(pMediaPlayer);
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
int CPlayer::playMedia(const QString &sCmdLine, bool bAllowCtrl)
{
   int                         iRV  = 0;
   libvlc_media_t             *p_md = NULL;
   QStringList                 lArgs;
   QStringList::const_iterator cit;

   // store control flag ...
   bCtrlStream = bAllowCtrl;

   // reset play timer stuff ...
   timer.reset();

   // while not showing video, disable spooling ...
   bSpoolPending = true;
   enableDisablePlayControl (false);

   // enable / disable position slider ...
   ui->posSlider->setValue(0);
   // ui->posSlider->setEnabled(bCtrlStream);
   ui->labPos->setEnabled(bCtrlStream);
   ui->labPos->setText("00:00:00");

   // get MRL ...
   QString     sMrl  = sCmdLine.section(";;", 0, 0);
   // QString     sMrl  = "d:/bbb.avi";
   // QString     sMrl  = "/home/joergn/Videos/bbb.avi";
   // QString     sMrl  = "d:/BR-test.ts";

   // are there mrl options ... ?
   if (sCmdLine.contains(";;"))
   {
      // get player arguments ...
      lArgs = sCmdLine.mid(sCmdLine.indexOf(";;", 0))
                          .split(";;", QString::SkipEmptyParts);
   }

   if (!pVlcInstance)
   {
      iRV = initPlayer();
   }

   if (!iRV)
   {
      mInfo(tr("Use following URL:\n  --> %1").arg(sMrl));
      p_md = libvlc_media_new_location(pVlcInstance, sMrl.toUtf8().constData());

      if (p_md)
      {
         // add mrl options ...
         for (cit = lArgs.constBegin(); cit != lArgs.constEnd(); cit ++)
         {
            mInfo(tr("Add MRL Option: %1").arg(*cit));
            libvlc_media_add_option(p_md, (*cit).toUtf8().constData());
         }

         // set media in player ...
         libvlc_media_player_set_media (pMediaPlayer, p_md);

         // now it's safe to release media ...
         libvlc_media_release (p_md);
      }
      else
      {
         mInfo(tr("Can't create media description ..."));
         iRV = -1;
      }
   }

   if (!iRV)
   {
      iRV = play();
   }

   if (!iRV && bCtrlStream)
   {
      // set slider to position ...
      int iSliderPos;

      // set slider range to seconds ...

      // we add 5 minutes at start and 5 minutes at end because start
      // and end time are not always accurate ...
      ui->posSlider->setRange(0, showInfo.ends() - showInfo.starts());

      iSliderPos = showInfo.lastJump();

      ui->posSlider->setValue(iSliderPos);

      ui->labPos->setText(QTime((int)iSliderPos / 3600,
                                (int)(iSliderPos % 3600) / 60,
                                iSliderPos % 60).toString("hh:mm:ss"));
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: slotUpdateSlider
|  Begin: 22.06.2010 / 17:16:51
|  Author: Jo2003
|  Description: update slider
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotUpdateSlider()
{
   if (pMediaPlayer)
   {
      if (libvlc_media_player_is_playing(pMediaPlayer) && bCtrlStream)
      {
         int iSliderPos = timer.elapsedEx() / 1000 + showInfo.lastJump();

         if (!ui->posSlider->isSliderDown())
         {
            ui->posSlider->setValue(iSliderPos);
            ui->labPos->setText(QTime((int)iSliderPos / 3600,
                                      (int)(iSliderPos % 3600) / 60,
                                      iSliderPos % 60).toString("hh:mm:ss"));
         }
      }
   }
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
      libvlc_state_t playState = libvlc_media_player_get_state (pMediaPlayer);

      switch (playState)
      {
      case libvlc_Opening:
      case libvlc_Buffering:
      case libvlc_Playing:
      case libvlc_Paused:
         bRV = true;
         break;

      default:
         break;
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
   switch (e->type())
   {
   case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
   default:
      break;
   }
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
   // error ...
   case libvlc_MediaPlayerEncounteredError:
      emit pPlayer->sigPlayState((int)IncPlay::PS_ERROR);
      pPlayer->stopPlayTimer();
      mInfo("libvlc_MediaPlayerEncounteredError ...");
      break;

   // opening media ...
   case libvlc_MediaPlayerOpening:
      emit pPlayer->sigPlayState((int)IncPlay::PS_OPEN);
      mInfo("libvlc_MediaPlayerOpening ...");
      break;

   // buffering media ...
   case libvlc_MediaPlayerBuffering:
      emit pPlayer->sigPlayState((int)IncPlay::PS_BUFFER);
      mInfo("libvlc_MediaPlayerBuffering ...");
      break;

   // playing media ...
   case libvlc_MediaPlayerPlaying:
      emit pPlayer->sigPlayState((int)IncPlay::PS_PLAY);
      emit pPlayer->sigTriggerAspectChg ();
      pPlayer->startPlayTimer();
      mInfo("libvlc_MediaPlayerPlaying ...");
      break;

   // player paused ...
   case libvlc_MediaPlayerPaused:
      emit pPlayer->sigPlayState((int)IncPlay::PS_PAUSE);
      pPlayer->pausePlayTimer();
      mInfo("libvlc_MediaPlayerPaused ...");
      break;

   // player stopped ...
   case libvlc_MediaPlayerStopped:
      emit pPlayer->sigPlayState((int)IncPlay::PS_STOP);
      pPlayer->stopPlayTimer();
      mInfo("libvlc_MediaPlayerStopped ...");
      break;

   // end of media reached ...
   case libvlc_MediaPlayerEndReached:
      emit pPlayer->sigPlayState((int)IncPlay::PS_END);
      pPlayer->stopPlayTimer();
      mInfo("libvlc_MediaPlayerEndReached ...");
      break;

   default:
      mInfo(tr("Unknown Event No. %1 received ...").arg(ev->type));
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
   // do we have a logger handle ... ?
   if (pLibVlcLog)
   {
      // how many entries in log ... ?
      uint uiEntryCount = libvlc_log_count (pLibVlcLog);

      // no error and entries there ...
      if (uiEntryCount > 0)
      {
         // log message buffer ...
         libvlc_log_message_t   logMsg;
         libvlc_log_message_t  *pLogMsg;

         // get iterator to go through log entries ...
         libvlc_log_iterator_t *it = libvlc_log_get_iterator(pLibVlcLog);

         // do we have an iterator ... ?
         if (it)
         {
            // while there are entries in log ...
            while (libvlc_log_iterator_has_next(it))
            {
               // get log message presented by log iterator ...
               pLogMsg = libvlc_log_iterator_next (it, &logMsg);

               if (pLogMsg)
               {
                  // build log message ...
                  mInfo(tr("Name: \"%1\", Type: \"%2\", Severity: %3\n  --> %4")
                         .arg(QString::fromUtf8(pLogMsg->psz_name))
                         .arg(QString::fromUtf8(pLogMsg->psz_type))
                         .arg(pLogMsg->i_severity)
                         .arg(QString::fromUtf8(pLogMsg->psz_message)));
               }
            }

            // free log iterator ...
            libvlc_log_iterator_free (it);
         }

         // delete all log entries ...
         libvlc_log_clear(pLibVlcLog);
      }
   }

   // check log again in a second ...
   poller.start(1000);
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
      QString sAspect, sCrop;

      // set new aspect ratio ...
      libvlc_video_set_aspect_ratio(pMediaPlayer, str.toAscii().data());

      // save aspect if changed ...
      pDb->aspect(showInfo.channelId(), sAspect, sCrop);

      if (sAspect != str)
      {
         // save to database ...
         pDb->addAspect(showInfo.channelId(), str, ui->cbxCrop->currentText());
      }

      mInfo(tr("Aspect ratio: %1")
            .arg(libvlc_video_get_aspect_ratio(pMediaPlayer)));
   }
}

/* -----------------------------------------------------------------\
|  Method: on_cbxCrop_currentIndexChanged
|  Begin: 23.03.2010 / 09:55:10
|  Author: Jo2003
|  Description: set new crop geometry ...
|
|  Parameters: new crop geometry as string ...
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::on_cbxCrop_currentIndexChanged(QString str)
{
   if (pMediaPlayer)
   {
      QString sAspect, sCrop;

      // set new aspect ratio ...
      libvlc_video_set_crop_geometry(pMediaPlayer, str.toAscii().data());

      // save crop if changed ...
      pDb->aspect(showInfo.channelId(), sAspect, sCrop);

      if (sCrop != str)
      {
         // save to database ...
         pDb->addAspect(showInfo.channelId(), ui->cbxAspect->currentText(), str);
      }

      mInfo(tr("Crop ratio: %1")
            .arg(libvlc_video_get_crop_geometry(pMediaPlayer)));
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
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::slotToggleFullScreen()
{
   return myToggleFullscreen();
}

/* -----------------------------------------------------------------\
|  Method: slotToggleAspectRatio
|  Begin: 08.03.2010 / 15:10:10
|  Author: Jo2003
|  Description: switch aspect ratio to next one ...
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> any error
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

/* -----------------------------------------------------------------\
|  Method: slotToggleCropGeometry
|  Begin: 08.03.2010 / 15:10:10
|  Author: Jo2003
|  Description: switch aspect ratio to next one ...
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::slotToggleCropGeometry()
{
   int iRV = -1;
   if (pMediaPlayer)
   {
      int idx = ui->cbxCrop->currentIndex();
      idx ++;

      // if end reached, start with index 0 ...
      if (idx >= ui->cbxCrop->count())
      {
         idx = 0;
      }

      // set new aspect ratio ...
      ui->cbxCrop->setCurrentIndex(idx);

      iRV = 0;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: slotTimeJumpRelative
|  Begin: 18.03.2010 / 15:10:10
|  Author: Jo2003
|  Description: time jump (back to the future ;-) )
|
|  Parameters: position value (jump + / - seconds)
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::slotTimeJumpRelative (int iSeconds)
{
   if (isPlaying() && bCtrlStream &&!bSpoolPending)
   {
      int  iNewPos = ui->posSlider->value() + iSeconds;
      uint uiGmt   = 0;

      // check that we don't reach another show ...
      if (iNewPos < 0)
      {
         iNewPos = 0;
      }
      else if (iNewPos > (int)(showInfo.ends() - showInfo.starts()))
      {
         iNewPos = showInfo.ends() - showInfo.starts();
      }

      // save new position ...
      showInfo.setLastJumpTime(iNewPos);

      // add the jump value ...
      uiGmt        = (uint)(iNewPos + showInfo.starts());

      // we now have the new GMT value for the archive stream ...

      // trigger request for the new stream position ...
      QString req = QString("cid=%1&gmt=%2")
                       .arg(showInfo.channelId()).arg(uiGmt);

      // mark spooling as active ...
      bSpoolPending = true;

      enableDisablePlayControl (false);

      pTrigger->TriggerRequest(Kartina::REQ_ARCHIV, req);
   }

   return 0;
}

/* -----------------------------------------------------------------\
|  Method: startPlayTimer
|  Begin: 25.03.2010 / 11:10:10
|  Author: Jo2003
|  Description: set player startup time
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::startPlayTimer()
{
   timer.start();
}

/* -----------------------------------------------------------------\
|  Method: pausePlayTimer
|  Begin: 09.04.2010 / 11:10:10
|  Author: Jo2003
|  Description: pause play timer
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::pausePlayTimer()
{
   timer.pause();
}

/* -----------------------------------------------------------------\
|  Method: stopPlayTimer
|  Begin: 22.06.2010 / 11:10:10
|  Author: Jo2003
|  Description: stop play timer
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::stopPlayTimer()
{
   timer.reset();
}

/* -----------------------------------------------------------------\
|  Method: on_btnFullScreen_clicked
|  Begin: 27.05.2010 / 11:10:10
|  Author: Jo2003
|  Description: change to full screen
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::on_btnFullScreen_clicked()
{
   slotToggleFullScreen();
}

/* -----------------------------------------------------------------\
|  Method: slotStoredAspectCrop [slot]
|  Begin: 15.06.2010 / 16:10:10
|  Author: Jo2003
|  Description: use stored aspect / crop for channel
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotStoredAspectCrop ()
{
   QString sAspect, sCrop;

   // enable spooling again ...
   bSpoolPending = false;
   enableDisablePlayControl (true);

   if(!pDb->aspect(showInfo.channelId(), sAspect, sCrop))
   {
      int iIdxOld, iIdxNew;

      // change combo box value for aspect ratio ...
      iIdxOld = ui->cbxAspect->currentIndex();
      iIdxNew = ui->cbxAspect->findText(sAspect);

      if (iIdxOld != iIdxNew)
      {
         // updating combobox' actual value will also
         // trigger the libVLC call ...
         ui->cbxAspect->setCurrentIndex (iIdxNew);
      }
      else
      {
         // since values don't differ, updating combobox will not
         // trigger format change. So set it directly to libVLC ...
         libvlc_video_set_aspect_ratio(pMediaPlayer, sAspect.toAscii().data());
      }

      // change combo box value for crop ratio ...
      iIdxOld = ui->cbxCrop->currentIndex();
      iIdxNew = ui->cbxCrop->findText(sCrop);

      if (iIdxOld != iIdxNew)
      {
         // updating combobox' actual value will also
         // trigger the libVLC call ...
         ui->cbxCrop->setCurrentIndex (iIdxNew);
      }
      else
      {
         // since values don't differ, updating combobox will not
         // trigger format change. So set it directly to libVLC ...
         libvlc_video_set_crop_geometry(pMediaPlayer, sCrop.toAscii().data());
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: myToggleFullscreen
|  Begin: 20.06.2010 / 14:10:10
|  Author: Jo2003
|  Description: toggle fullscreen (only supported with libVLC1.10)
|
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
int CPlayer::myToggleFullscreen()
{
   int iRV = 0;

   if (pMediaPlayer)
   {
      // check if fullscreen is enabled ...
      if (ui->fParent->isFullScreen ())
      {
         // hide screen ...
         ui->fParent->hide ();

         // end fullscreen ...
         ui->fParent->showNormal();

         // put parent frame back into the layout where it belongs to ...
         // this also sets parent and resizes as needed ...
         ui->vlMasterFrame->addWidget (ui->fParent);

         // show normal ...
         ui->fParent->show();

         // video frame doesn't need any focus when in windowed mode ...
         ui->fVideo->setFocusPolicy(Qt::NoFocus);
      }
      else
      {
         Qt::WindowFlags f;

         // get active desktop widget ...
         QDesktopWidget *pDesktop    = QApplication::desktop ();
         int             iScreen     = pDesktop->screenNumber (this);
         QWidget        *pActScreen  = pDesktop->screen (iScreen);
         QRect           sizeDesktop = pDesktop->screenGeometry (this);

         mInfo(tr("\n  --> Player Widget is located at %2 screen "
                  "(Screen No. %1, Resolution %3px x %4px) ...")
                  .arg(iScreen)
                  .arg((iScreen == pDesktop->primaryScreen ()) ? "primary" : "secondary")
                  .arg(sizeDesktop.width ())
                  .arg(sizeDesktop.height ()));

         if (!pActScreen)
         {
            mInfo(tr("Can't get active screen QWidget!"));
         }

         // frameless window which stays on top ...
         f  = Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint;

#ifdef Q_OS_LINUX
         f |= Qt::X11BypassWindowManagerHint;
#endif // Q_OS_LINUX

         // hide screen ...
         ui->fParent->hide ();

         // reparent to active screen ...
         ui->fParent->setParent(pActScreen, f);
         ui->fParent->setGeometry (sizeDesktop);
         ui->fParent->showFullScreen ();

         // to grab keyboard input we need the focus ...
         // set policy so we can get focus ...
         ui->fVideo->setFocusPolicy(Qt::StrongFocus);

         // get the focus ...
         ui->fVideo->setFocus(Qt::OtherFocusReason);
      }
   }
   else
   {
      iRV = -1;
      mInfo(tr("Can't switch to fullscreen if there is no media to play!"));
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: on_posSlider_sliderReleased
|  Begin: 23.06.2010 / 09:10:10
|  Author: Jo2003
|  Description: update position label to relect
|               slider position change
|  Parameters: actual slider position
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::on_posSlider_sliderReleased()
{
   if (isPlaying() && bCtrlStream && !bSpoolPending)
   {
      uint position = (uint)ui->posSlider->value();

      // ignore slider position change of +/- 10 seconds ...
      uint uiPlayTime = showInfo.lastJump();

      // play position ...
      uiPlayTime += (uint)(timer.elapsedEx() / 1000);

      // check if slider position is in 10 sec. limit ...
      if ((position >= (uiPlayTime - 10)) && (position <= (uiPlayTime + 10)))
      {
         mInfo(tr("Ignore slightly slider position change..."));
      }
      else
      {
         // request new stream ...
         QString req = QString("cid=%1&gmt=%2")
                      .arg(showInfo.channelId())
                      .arg(showInfo.starts() + position);

         // update last jump time ...
         showInfo.setLastJumpTime(position);

         // mark spooling as active ...
         bSpoolPending = true;

         enableDisablePlayControl (false);

         // trigger stream request ...
         pTrigger->TriggerRequest(Kartina::REQ_ARCHIV, req);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: on_posSlider_valueChanged
|  Begin: 23.06.2010 / 09:10:10
|  Author: Jo2003
|  Description: update position label to relect
|               slider position change
|  Parameters: actual slider position
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::on_posSlider_valueChanged(int value)
{
   if (isPlaying() && bCtrlStream)
   {
      if (ui->posSlider->isSliderDown())
      {
         ui->labPos->setText(QTime((int)value / 3600,
                                   (int)(value % 3600) / 60,
                                   value % 60).toString("hh:mm:ss"));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: enableDisablePlayControl
|  Begin: 25.07.2010 / 09:10:10
|  Author: Jo2003
|  Description: enable / disable play control items
|
|  Parameters: enable / or disable
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::enableDisablePlayControl (bool bEnable)
{
   ui->btnFwd->setEnabled (bEnable && bCtrlStream);
   ui->btnBwd->setEnabled (bEnable && bCtrlStream);
   ui->posSlider->setEnabled (bEnable && bCtrlStream);
}

/************************* History ***************************\
| $Log$
\*************************************************************/

