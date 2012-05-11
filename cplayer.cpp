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

// help macros to let QSlider support GMT values ...
#define mFromGmt(__x__) (int)((__x__) - TIME_OFFSET)
#define mToGmt(__x__) (uint)((__x__) + TIME_OFFSET)

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

   pMediaPlayer     = NULL;
   pVlcInstance     = NULL;
   pMedialistPlayer = NULL;
   pEMPlay          = NULL;
   pSettings        = NULL;
   pTrigger         = NULL;
   bCtrlStream      = false;
   bSpoolPending    = true;
   uiDuration       = (uint)-1;
   mAspect.clear();
   mCrop.clear();

   QStringList slKey, slVal;
   int i;

   // aspect ratio ...
   slKey << "std" << "1:1" << "4:3" << "16:9" << "16:10" << "2.21:1"  << "5:4";
   slVal << ""    << "1:1" << "4:3" << "16:9" << "16:10" << "221:100" << "5:4";

   for (i = 0; i < slKey.size(); i++)
   {
      mAspect.insert(slKey.value(i), slVal.value(i));
   }

   ui->cbxAspect->clear();
   ui->cbxAspect->insertItems(0, slKey);

   // crop geometry ...
   slKey.clear();
   slVal.clear();

   slKey << "std" << "1:1" << "4:3" << "16:9" << "16:10" << "1.85:1"  << "2.21:1"  << "2.35:1"  << "2.39:1"  << "5:4";
   slVal << ""    << "1:1" << "4:3" << "16:9" << "16:10" << "185:100" << "221:100" << "235:100" << "239:100" << "5:4";

   for (i = 0; i < slKey.size(); i++)
   {
      mCrop.insert(slKey.value(i), slVal.value(i));
   }

   ui->cbxCrop->clear();
   ui->cbxCrop->insertItems(0, slKey);

   // set aspect shot timer to single shot ...
   tAspectShot.setSingleShot (true);
   tAspectShot.setInterval (800);

   // connect volume slider with volume change function ...
   connect(ui->volSlider, SIGNAL(sliderMoved(int)), this, SLOT(slotChangeVolume(int)));

   // connect double click signal from videoframe with fullscreen toggle ...
   connect(ui->videoWidget, SIGNAL(fullScreen()), this, SLOT(slotToggleFullscreen()));

   // mouse wheel changes volume ...
   connect(ui->videoWidget, SIGNAL(wheel(bool)), this, SLOT(slotChangeVolumeDelta(bool)));

   // connect slider timer with slider position slot ...
   connect(&sliderTimer, SIGNAL(timeout()), this, SLOT(slotUpdateSlider()));

   // connect aspect shot timer with aspect change function ...
   connect(&tAspectShot, SIGNAL(timeout()), this, SLOT(slotStoredAspectCrop()));

   // connect aspect trigger signal with timer start ...
   connect(this, SIGNAL(sigTriggerAspectChg()), &tAspectShot, SLOT(start()));

   // connect slider click'n'Go ...
   connect(ui->posSlider, SIGNAL(sigClickNGo(int)), this, SLOT(slotSliderPosChanged()));
   connect(ui->posSlider, SIGNAL(sliderReleased()), this, SLOT(slotSliderPosChanged()));

   // update position slider every second ...
   sliderTimer.start(1000);
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
   // stop timer ...
   sliderTimer.stop();

   stop();

   if (pMediaPlayer)
   {
      libvlc_media_player_release (pMediaPlayer);
      pMediaPlayer = NULL;
   }

   if (pMedialistPlayer)
   {
       libvlc_media_list_player_release (pMedialistPlayer);
       pMedialistPlayer = NULL;
   }

   if (pVlcInstance)
   {
#ifdef Q_OS_MACX
      // releasing it on Mac leads to crash if you end the
      // player with running video ... no problem at all 'cause
      // we want release at program close!
      libvlc_retain(pVlcInstance);
#else
      libvlc_release(pVlcInstance);
#endif
   }

   delete ui;
}

/* -----------------------------------------------------------------\
|  Method: isPositionable
|  Begin: 27.12.2010 / 11:15
|  Author: Jo2003
|  Description: is stream positionable?
|
|  Parameters: --
|
|  Returns: true --> yes
|          false --> no
\----------------------------------------------------------------- */
bool CPlayer::isPositionable()
{
   return ((uiDuration > 0) && (uiDuration != (uint)-1)) ? true : false;
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
   ui->videoWidget->setShortCuts(pvSc);
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
   int          iRV     = -1;
   int          argc    = 0;
   const char **argv    = NULL;
   const char  *pVerbose;
   QStringList  slOpts;

   // set verbose mode ...
   if (pSettings->libVlcVerboseLevel() <= 0)
   {
      pVerbose = "--quiet";
   }
   else if (pSettings->libVlcVerboseLevel() >= 2)
   {
      pVerbose = "--verbose=2";
   }
   else
   {
      pVerbose = "--verbose=1";
   }

   // reset crop and aspect cbx ... because it should show the state
   // as used ...
   ui->cbxAspect->setCurrentIndex(0);
   ui->cbxCrop->setCurrentIndex(0);

   // create a new libvlc instance ...
   const char *vlc_args[] = {
      "--ignore-config",
      "--intf=dummy",
      "--no-media-library",
      "--no-osd",
      "--no-stats",
      "--no-video-title-show",
   #ifdef Q_WS_MAC
      // vout as well as opengl-provider MIGHT be "minimal_macosx" ...
      "--vout=macosx",
   #endif
      pVerbose
   };

   argc = sizeof(vlc_args) / sizeof(vlc_args[0]);
   argv = vlc_args;

   for (int i = 0; i < argc; i++)
   {
      slOpts.append(argv[i]);
   }

   mInfo(tr("Create libVLC with following global options:\n %1").arg(slOpts.join(" ")));

   pVlcInstance = libvlc_new(argc, argv);

   if (pVlcInstance)
   {
      // get mediaplayer ...
      pMediaPlayer = libvlc_media_player_new (pVlcInstance);
   }

   if (pMediaPlayer)
   {
      // add player to window ...
      connectToVideoWidget();

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

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerPlaying,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerPaused,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerStopped,
                                 CPlayer::eventCallback, (void *)this);

      iRV |= libvlc_event_attach(pEMPlay, libvlc_MediaPlayerEndReached,
                                 CPlayer::eventCallback, (void *)this);
   }

   // create media list player ...
   if (!iRV)
   {
      if ((pMedialistPlayer = libvlc_media_list_player_new(pVlcInstance)) != NULL)
      {
          libvlc_media_list_player_set_media_player (pMedialistPlayer, pMediaPlayer);
      }
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

   if (pMediaPlayer)
   {
      libvlc_audio_set_volume (pMediaPlayer, newVolume);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotChangeVolumeDelta [slot]
|  Begin: 16.02.2012
|  Author: Jo2003
|  Description: change volume
|
|  Parameters: up: true --> up, false --> down
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotChangeVolumeDelta(const bool up)
{
   if (pMediaPlayer)
   {
      int iVol = libvlc_audio_get_volume(pMediaPlayer);
#ifdef Q_OS_MACX
      // mighty mouse may act to fast ...
      iVol    += up ? 1 : -1;
#else
      iVol    += up ? 5 : -5;
#endif
      iVol     = (iVol > 100) ? 100 : ((iVol < 0) ? 0 : iVol);

      if (iVol != ui->volSlider->value())
      {
         ui->volSlider->setValue(iVol);
         slotChangeVolume(iVol);
      }
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
   int  iRV    = 0;

   if (pMedialistPlayer)
   {
      libvlc_media_list_player_play (pMedialistPlayer);
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

   if (pMedialistPlayer)
   {
      libvlc_media_list_player_stop (pMedialistPlayer);
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

   if (pMedialistPlayer && bCtrlStream)
   {
      libvlc_media_list_player_pause(pMedialistPlayer);
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
   int                         iRV    = 0;
   libvlc_media_t             *p_md   = NULL;
   libvlc_media_list_t        *p_ml   = NULL;
   QStringList                 lArgs;
   QStringList::const_iterator cit;

   // do we can control the stream ... ?
   if ((showInfo.playState() == IncPlay::PS_PLAY)
      && showInfo.canCtrlStream())
   {
      bCtrlStream = true;
   }
   else
   {
      bCtrlStream = false;
   }

   // reset play timer stuff ...
   timer.reset();
   timer.setStartGmt(showInfo.lastJump() ? showInfo.lastJump() : showInfo.starts());
   uiDuration = (uint)-1;

   // while not showing video, disable spooling ...
   bSpoolPending = true;
   enableDisablePlayControl (false);

   // get MRL ...
   QString     sMrl  = sCmdLine.section(";;", 0, 0);
   // QString     sMrl  = "http://172.25.1.145/~joergn/hobbit.mov";

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
         // do we use GPU acceleration ... ?
         if (pSettings->useGpuAcc())
         {
            mInfo(tr("Add MRL Option: %1").arg(GPU_ACC_TOKEN));
            libvlc_media_add_option(p_md, GPU_ACC_TOKEN);
         }

         ///////////////////////////////////////////////////////////////////////////
         // set proxy server ...
         ///////////////////////////////////////////////////////////////////////////
         if (pSettings->UseProxy())
         {
            sMrl = ":http_proxy=http://";

            if (pSettings->GetProxyUser() != "")
            {
               sMrl += QString("%1@").arg(pSettings->GetProxyUser());
            }

            sMrl += QString("%1:%2/").arg(pSettings->GetProxyHost()).arg(pSettings->GetProxyPort());
            mInfo(tr("Add MRL Option: %1").arg(sMrl));
            libvlc_media_add_option(p_md, sMrl.toUtf8().constData());

            if ((pSettings->GetProxyPasswd() != "") && (pSettings->GetProxyUser() != ""))
            {
               sMrl = QString(":http_proxy_pwd=%1").arg(pSettings->GetProxyPasswd());
               mInfo(tr("Add MRL Option: :http_proxy_pwd=******"));
               libvlc_media_add_option(p_md, sMrl.toUtf8().constData());
            }
         }

         ///////////////////////////////////////////////////////////////////////////
         // timeshift stuff ...
         ///////////////////////////////////////////////////////////////////////////
         sMrl = QString(":input-timeshift-path=%1").arg(QDir::tempPath());
         mInfo(tr("Add MRL Option: %1").arg(sMrl));
         libvlc_media_add_option(p_md, sMrl.toUtf8().constData());

         sMrl = QString(":input-timeshift-granularity=%1").arg((int)(1.5 * 1024.0 * 1024.0 * 1024.0)); // 1.5GB
         mInfo(tr("Add MRL Option: %1").arg(sMrl));
         libvlc_media_add_option(p_md, sMrl.toUtf8().constData());

         ///////////////////////////////////////////////////////////////////////////
         // further mrl options from player module file ...
         ///////////////////////////////////////////////////////////////////////////
         for (cit = lArgs.constBegin(); cit != lArgs.constEnd(); cit ++)
         {
            mInfo(tr("Add MRL Option: %1").arg(*cit));
            libvlc_media_add_option(p_md, (*cit).toUtf8().constData());
         }

         // create media list ...
         if ((p_ml = libvlc_media_list_new(pVlcInstance)) != NULL)
         {
            // add commercials if needed ...
            addAd(p_ml);

            // add media ...
            libvlc_media_list_add_media(p_ml, p_md);

            // set media in player ...
            libvlc_media_list_player_set_media_list (pMedialistPlayer, p_ml);

            // now it's safe to release medialist ...
            libvlc_media_list_release (p_ml);
         }
         else
         {
            mInfo(tr("Can't create media list ..."));
            iRV = -1;
         }

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

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: addAd
|  Begin: 11.05.2012
|  Author: Jo2003
|  Description: add commercial
|
|  Parameters: medialist to add ad
|
|  Returns: 0 --> none added
|           1 --> added
\----------------------------------------------------------------- */
int CPlayer::addAd(libvlc_media_list_t *pList)
{
   // play add ... ?
   int             iRV    = 0;
   libvlc_media_t *p_mdad = NULL;
   QString         adUrl  = showInfo.adUrl();
   QString         sOpt;

   if ((adUrl != "") && (showInfo.showType() == ShowInfo::VOD) && pSettings->showAds())
   {
      mInfo(tr("Prepend Ad (Url):\n  --> %1").arg(adUrl));
      if ((p_mdad = libvlc_media_new_location(pVlcInstance, adUrl.toUtf8().constData())) != NULL)
      {
         sOpt = QString(":http-caching=%1").arg(pSettings->GetBufferTime());
         mInfo(tr("Add MRL Option: %1").arg(sOpt));
         libvlc_media_add_option(p_mdad, sOpt.toUtf8().constData());

         sOpt = ":no-http-reconnect";
         mInfo(tr("Add MRL Option: %1").arg(sOpt));
         libvlc_media_add_option(p_mdad, sOpt.toUtf8().constData());

         ///////////////////////////////////////////////////////////////////////////
         // set proxy server ...
         ///////////////////////////////////////////////////////////////////////////
         if (pSettings->UseProxy())
         {
            sOpt = ":http_proxy=http://";

            if (pSettings->GetProxyUser() != "")
            {
               sOpt += QString("%1@").arg(pSettings->GetProxyUser());
            }

            sOpt += QString("%1:%2/").arg(pSettings->GetProxyHost()).arg(pSettings->GetProxyPort());
            mInfo(tr("Add MRL Option: %1").arg(sOpt));
            libvlc_media_add_option(p_mdad, sOpt.toUtf8().constData());

            if ((pSettings->GetProxyPasswd() != "") && (pSettings->GetProxyUser() != ""))
            {
               sOpt = QString(":http_proxy_pwd=%1").arg(pSettings->GetProxyPasswd());
               mInfo(tr("Add MRL Option: :http_proxy_pwd=******"));
               libvlc_media_add_option(p_mdad, sOpt.toUtf8().constData());
            }
         }

         // add media ...
         libvlc_media_list_add_media(pList, p_mdad);

         // release ad ...
         libvlc_media_release (p_mdad);

         iRV = 1;
      }
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
      if (libvlc_media_player_is_playing(pMediaPlayer))
      {
         uint pos;
         if (isPositionable())
         {
            pos = libvlc_media_player_get_time (pMediaPlayer) / 1000;

            if (!ui->posSlider->isSliderDown())
            {
               ui->posSlider->setValue(pos);
               ui->labPos->setTime(pos);
            }
         }
         else
         {
            pos = timer.gmtPosition();

            if (!ui->posSlider->isSliderDown())
            {
               // reaching the end of this show ... ?
               if (pos > mToGmt(ui->posSlider->maximum()))
               {
                  // check archive program ...
                  emit sigCheckArchProg(pos);
               }

               ui->posSlider->setValue(mFromGmt(pos));

               pos -= showInfo.starts();

               ui->labPos->setTime(pos);
            }
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
      mInfo("libvlc_MediaPlayerEncounteredError ...");
      emit pPlayer->sigPlayState((int)IncPlay::PS_ERROR);
      pPlayer->stopPlayTimer();
      break;

   // opening media ...
   case libvlc_MediaPlayerOpening:
      mInfo("libvlc_MediaPlayerOpening ...");
      emit pPlayer->sigPlayState((int)IncPlay::PS_OPEN);
      break;

   // playing media ...
   case libvlc_MediaPlayerPlaying:
      mInfo("libvlc_MediaPlayerPlaying ...");
      emit pPlayer->sigPlayState((int)IncPlay::PS_PLAY);
      emit pPlayer->sigTriggerAspectChg ();
      pPlayer->startPlayTimer();
      pPlayer->initSlider();
      break;

   // player paused ...
   case libvlc_MediaPlayerPaused:
      mInfo("libvlc_MediaPlayerPaused ...");
      emit pPlayer->sigPlayState((int)IncPlay::PS_PAUSE);
      pPlayer->pausePlayTimer();
      break;

   // player stopped ...
   case libvlc_MediaPlayerStopped:
      mInfo("libvlc_MediaPlayerStopped ...");
      emit pPlayer->sigPlayState((int)IncPlay::PS_STOP);
      pPlayer->stopPlayTimer();
      break;

   // end of media reached ...
   case libvlc_MediaPlayerEndReached:
      mInfo("libvlc_MediaPlayerEndReached ...");
      emit pPlayer->sigPlayState((int)IncPlay::PS_END);
      pPlayer->stopPlayTimer();
      break;

   default:
      mInfo(tr("Unknown Event No. %1 received ...").arg(ev->type));
      break;
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
      QString sAspect, sCrop;

      // set new aspect ratio ...
      libvlc_video_set_aspect_ratio(pMediaPlayer, mAspect.value(str).toUtf8().constData());

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
      libvlc_video_set_crop_geometry(pMediaPlayer, mCrop.value(str).toUtf8().constData());

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
      uint pos;

      if (isPositionable())
      {
         pos  = libvlc_media_player_get_time(pMediaPlayer);

         // make sure we don't go negative ...
         if ((iSeconds < 0) && (((uint)abs(iSeconds) * 1000) > pos))
         {
            pos = 0;
         }
         else
         {
            pos += iSeconds * 1000; // ms ...
         }

         libvlc_media_player_set_time(pMediaPlayer, pos);

         ui->posSlider->setValue((int)(pos / 1000));
      }
      else
      {
         // get new gmt value ...
         pos = timer.gmtPosition() + iSeconds;

         // trigger request for the new stream position ...
         QString req = QString("cid=%1&gmt=%2")
                          .arg(showInfo.channelId()).arg(pos);

         // mark spooling as active ...
         bSpoolPending = true;

         enableDisablePlayControl (false);

         // save jump time ...
         showInfo.setLastJumpTime(pos);

         pTrigger->TriggerRequest(Kartina::REQ_ARCHIV, req);

         // do we reach another show?
         if ((pos < mToGmt(ui->posSlider->minimum()))
             || (pos > mToGmt(ui->posSlider->maximum())))
         {
            // yes --> update show info ...
            emit sigCheckArchProg(pos);
         }
      }
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
   emit sigToggleFullscreen();
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
         libvlc_video_set_aspect_ratio(pMediaPlayer, mAspect.value(sAspect).toUtf8().constData());
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
         libvlc_video_set_crop_geometry(pMediaPlayer, mCrop.value(sCrop).toUtf8().constData());
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotSliderPosChanged [slot]
|  Begin: 17.02.2012
|  Author: Jo2003
|  Description: slider position was changed active
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotSliderPosChanged()
{
   if (isPlaying() && bCtrlStream && !bSpoolPending)
   {
      // stop slider update timer ...
      sliderTimer.stop();

      uint position = (uint)ui->posSlider->value();

      if (isPositionable())
      {
         libvlc_media_player_set_time(pMediaPlayer, position * 1000);
      }
      else
      {
         position = mToGmt(position);

         // check if slider position is in 10 sec. limit ...
         if (abs(position - timer.gmtPosition()) <= 10)
         {
            mInfo(tr("Ignore slightly slider position change..."));
         }
         else
         {
            // request new stream ...
            QString req = QString("cid=%1&gmt=%2")
                         .arg(showInfo.channelId())
                         .arg(position);

            // mark spooling as active ...
            bSpoolPending = true;

            enableDisablePlayControl (false);

            // save new start value ...
            showInfo.setLastJumpTime(position);

            // trigger stream request ...
            pTrigger->TriggerRequest(Kartina::REQ_ARCHIV, req);
         }
      }

      // restart slider update timer ...
      sliderTimer.start(1000);
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
      if (!isPositionable())
      {
         value  = mToGmt(value);
         value -= showInfo.starts();
      }

      ui->labPos->setTime(value);
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
   if (bEnable && bCtrlStream)
   {
      ui->posSlider->setEnabled (true);
   }
   else
   {
      ui->posSlider->setEnabled (false);
   }
}

/* -----------------------------------------------------------------\
|  Method: initSlider
|  Begin: 27.12.2010 / 11:50
|  Author: Jo2003
|  Description: init slider...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::initSlider()
{
   // check if we need the pseudo archive spool
   // or the real spool in vod ...
   uiDuration = libvlc_media_player_get_length(pMediaPlayer);
   mInfo(tr("Film length: %1ms.").arg(uiDuration));

   if (isPositionable())
   {
      // VOD stuff ...
      ui->posSlider->setRange(0, (int)(uiDuration / 1000));

      ui->labPos->setTime(0);
   }
   else
   {
      // set slider range to seconds ...
      ui->posSlider->setRange(mFromGmt(showInfo.starts()), mFromGmt(showInfo.ends()));

      if (showInfo.lastJump())
      {
         ui->posSlider->setValue(mFromGmt(showInfo.lastJump()));

         ui->labPos->setTime(showInfo.lastJump() - showInfo.starts());
      }
      else
      {
         ui->posSlider->setValue(mFromGmt(showInfo.starts()));

         ui->labPos->setTime(0);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: getSilderPos
|  Begin: 07.01.2011 / 10:20
|  Author: Jo2003
|  Description: get slider position
|
|  Parameters: --
|
|  Returns: gmt of slider position
\----------------------------------------------------------------- */
uint CPlayer::getSilderPos ()
{
   return mToGmt(ui->posSlider->value());
}

/* -----------------------------------------------------------------\
|  Method: slotMoreLoudly
|  Begin: 25.03.2011 / 9:30
|  Author: Jo2003
|  Description: make it louder
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotMoreLoudly()
{
   if (pMediaPlayer)
   {
      int newVolume = libvlc_audio_get_volume (pMediaPlayer) + 5;

      if (newVolume > ui->volSlider->maximum())
      {
         newVolume = ui->volSlider->maximum();
      }

      if(!libvlc_audio_set_volume (pMediaPlayer, newVolume))
      {
         ui->labSound->setPixmap(QPixmap(":/player/sound_on"));
         ui->volSlider->setValue(newVolume);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotMoreQuietly
|  Begin: 25.03.2011 / 9:30
|  Author: Jo2003
|  Description: make it more quietly
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotMoreQuietly()
{
   if (pMediaPlayer)
   {
      int newVolume = libvlc_audio_get_volume (pMediaPlayer) - 5;

      if (newVolume < 0)
      {
         newVolume = 0;
      }

      if (!libvlc_audio_set_volume (pMediaPlayer, newVolume))
      {
         if (!newVolume)
         {
            ui->labSound->setPixmap(QPixmap(":/player/sound_off"));
         }
         ui->volSlider->setValue(newVolume);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotMute
|  Begin: 25.03.2011 / 9:30
|  Author: Jo2003
|  Description: toggle mute
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotMute()
{
   if (pMediaPlayer)
   {
      int mute = (libvlc_audio_get_mute(pMediaPlayer)) ? 0 : 1;

      if (mute)
      {
         ui->labSound->setPixmap(QPixmap(":/player/sound_off"));
      }
      else
      {
         ui->labSound->setPixmap(QPixmap(":/player/sound_on"));
      }

      libvlc_audio_set_mute(pMediaPlayer, mute);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotShowInfoUpdated
|  Begin: 04.11.2011
|  Author: Jo2003
|  Description: showinfo struct was updated ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotShowInfoUpdated()
{
   // we have to do the following:
   // - Reset Timer
   // - Reset Slider
   ulong gmt = timer.gmtPosition();
   timer.reset();
   timer.setStartGmt(gmt);
   timer.start();

   // set slider range to seconds ...
   ui->posSlider->setRange(mFromGmt(showInfo.starts()), mFromGmt(showInfo.ends()));
}

/* -----------------------------------------------------------------\
|  Method: connectToVideoWidget
|  Begin: 31.01.2012
|  Author: Jo2003
|  Description: connect media player to video widget
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::connectToVideoWidget()
{
#ifdef Q_OS_WIN
   libvlc_media_player_set_hwnd (pMediaPlayer, (void *)ui->videoWidget->widgetId());
#elif defined Q_OS_MAC
   libvlc_media_player_set_nsobject(pMediaPlayer, (void *)ui->videoWidget->widgetId());
#else
   libvlc_media_player_set_xwindow(pMediaPlayer, ui->videoWidget->widgetId());
#endif
}

/* -----------------------------------------------------------------\
|  Method: slotToggleFullscreen
|  Begin: 20.04.2012
|  Author: Jo2003
|  Description: a wrapper for toogle fullscreen of video widget
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotToggleFullscreen()
{
   emit sigToggleFullscreen();
}

/* -----------------------------------------------------------------\
|  Method: getAndRemoveVideoWidget
|  Begin: 27.04.2012
|  Author: Jo2003
|  Description: remove video widget from player layout and
|               return it
|
|  Parameters: --
|
|  Returns: pointer to video widget
\----------------------------------------------------------------- */
QVlcVideoWidget* CPlayer::getAndRemoveVideoWidget()
{
   ui->vPlayerLayout->removeWidget(ui->videoWidget);
   return ui->videoWidget;
}

/* -----------------------------------------------------------------\
|  Method: addAndEmbedVideoWidget
|  Begin: 27.04.2012
|  Author: Jo2003
|  Description: embed videoWidget again
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::addAndEmbedVideoWidget()
{
   ui->vPlayerLayout->insertWidget(1, ui->videoWidget, 100);
   ui->videoWidget->show();
   ui->videoWidget->raise();
   ui->videoWidget->raiseRender();
}

/* -----------------------------------------------------------------\
|  Method: slotFsToggled
|  Begin: 30.04.2012
|  Author: Jo2003
|  Description: fullscreen was toggled
|
|  Parameters: enabled or disabled
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotFsToggled(int on)
{
   ui->videoWidget->fullScreenToggled(on);
}

/************************* History ***************************\
| $Log$
\*************************************************************/

