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

#include "qfusioncontrol.h"

// fusion control ...
extern QFusionControl missionControl;

// log file functions ...
extern CLogFile VlcLog;

// storage db ...
extern CVlcRecDB *pDb;

// global showinfo class ...
extern CShowInfo showInfo;

// global api client class ...
extern ApiClient *pApiClient;

// help macros to let QSlider support GMT values ...
#define mFromGmt(__x__) (int)((__x__) - TIME_OFFSET)
#define mToGmt(__x__) (uint)((__x__) + TIME_OFFSET)

QVector<libvlc_event_type_t> CPlayer::_eventQueue;
QMutex                       CPlayer::_mtxEvt;
float                        CPlayer::_flBuffPrt = 0.0;
const char*                  CPlayer::_pAspect[] = {"", "1:1", "4:3", "16:9", "16:10", "221:100", "5:4"};
const char*                  CPlayer::_pCrop[]   = {"", "1:1", "4:3", "16:9", "16:10", "185:100", "221:100", "235:100", "239:100", "5:4"};

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
   pMediaList       = NULL;
   pEMPlay          = NULL;
   pSettings        = NULL;
   bSpoolPending    = true;
   bOmitNextEvent   = false;
   bScanAuTrk       = true;
   uiDuration       = (uint)-1;
   ulLibvlcVersion  = 0;
   QStringList slKey;
   uint i;

   // feed mission control ...
   missionControl.addMuteLab(ui->labSound);
   missionControl.addCngSlider(ui->posSlider);
   missionControl.addTimeLab(ui->labPos);
   missionControl.addVolSlider(ui->volSlider);
   missionControl.addButton(ui->btnFullScreen,     QFusionControl::BTN_FS);
   missionControl.addButton(ui->btnSaveAspectCrop, QFusionControl::BTN_FRMT);
   missionControl.addButton(ui->btnWindowed,       QFusionControl::BTN_WNDWD);
   missionControl.addVidFormCbx(ui->cbxAspect,     QFusionControl::CBX_ASPECT);
   missionControl.addVidFormCbx(ui->cbxCrop,       QFusionControl::CBX_CROP);

   // libVlcVersion ...
   QRegExp rx("^([0-9.]+).*$");
   QString s = libvlc_get_version();

   if (rx.indexIn(s) > -1)
   {
      s               = rx.cap(1);
      ulLibvlcVersion = (s.section('.', 0, 0).toUInt() << 24)
                      | (s.section('.', 1, 1).toUInt() << 16)
                      | (s.section('.', 2, 2).toUInt() <<  8);
   }

   // aspect ratio ...
   for (i = 0; i < (sizeof(_pAspect) / sizeof (_pAspect[0])); i++)
   {
      slKey.append(aspectCropToString(_pAspect[i]));
   }

   missionControl.vidFormCbxClear(QFusionControl::CBX_ASPECT);
   missionControl.vidFormCbxInsertValues(0, slKey, QFusionControl::CBX_ASPECT);

   // crop geometry ...
   slKey.clear();

   for (i = 0; i < (sizeof(_pCrop) / sizeof (_pCrop[0])); i++)
   {
      slKey.append(aspectCropToString(_pCrop[i]));
   }

   missionControl.vidFormCbxClear(QFusionControl::CBX_CROP);
   missionControl.vidFormCbxInsertValues(0, slKey, QFusionControl::CBX_CROP);

   // set aspect shot timer to single shot ...
   tAspectShot.setSingleShot (true);
   tAspectShot.setInterval (2500);

   // poll for state change events with 250ms interval ...
   tEventPoll.setInterval(250);

   // aspect and crop ...
   connect(&missionControl, SIGNAL(sigAspectCurrentIndexChanged(int)), this, SLOT(slotCbxAspectCurrentIndexChanged(int)));
   connect(&missionControl, SIGNAL(sigCropCurrentIndexChanged(int)), this, SLOT(slotCbxCropCurrentIndexChanged(int)));

   // save aspect ...
   connect(&missionControl, SIGNAL(sigSaveVideoFormat()), this, SLOT(slotBtnSaveAspectCropClicked()));

   // connect volume slider with volume change function ...
   connect(&missionControl, SIGNAL(sigVolSliderMoved(int)), this, SLOT(slotChangeVolume(int)));

   // connect double click signal from videoframe with fullscreen toggle ...
   connect(ui->videoWidget, SIGNAL(fullScreen()), this, SLOT(slotToggleFullscreen()));

   // mouse wheel changes volume ...
   connect(ui->videoWidget, SIGNAL(wheel(bool)), this, SLOT(slotChangeVolumeDelta(bool)));

   // connect slider timer with slider position slot ...
   connect(&sliderTimer, SIGNAL(timeout()), this, SLOT(slotUpdateSlider()));

   // connect aspect shot timer with aspect change function ...
   connect(&tAspectShot, SIGNAL(timeout()), this, SLOT(slotStoredAspectCrop()));

   // connect slider click'n'Go ...
   connect(&missionControl, SIGNAL(sigPosClickNGo(int)), this, SLOT(slotSliderPosChanged()));
   connect(&missionControl, SIGNAL(sigPosSliderReleased()), this, SLOT(slotSliderPosChanged()));
   connect(&missionControl, SIGNAL(sigPosSliderValueChanged(int)), this, SLOT(slotPositionChanged(int)));

   // screenshot ...
   connect(&missionControl, SIGNAL(sigScrShot()), this, SLOT(slotTakeScreenShot()));

   // event poll ...
   connect(&tEventPoll, SIGNAL(timeout()), this, SLOT(slotEventPoll()));

   connect(this, SIGNAL(sigBuffPercent(int)), this, SLOT(slotFinallyPlays(int)));
   connect(this, SIGNAL(sigAudioTracks(QLangVector)), ui->videoWidget, SLOT(slotUpdLangVector(QLangVector)));
   connect(ui->videoWidget, SIGNAL(sigDeinterlace(bool)), this, SLOT(slotDeinterlace(bool)));
   connect(ui->videoWidget, SIGNAL(sigNewATrack(int)), this, SLOT(slotChangeATrack(int)));

   // update position slider every second ...
   sliderTimer.start(1000);
   tEventPoll.start();
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
   tEventPoll.stop();

   stop();

   cleanupLibVLC(true);

   delete ui;
}

/* -----------------------------------------------------------------\
|  Method: cleanupLibVLC
|  Begin: 24.04.2013
|  Author: Jo2003
|  Description: clean libvlc stuff
|
|  Parameters: bDestruct flag to tell that function
|              is called from destructor
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::cleanupLibVLC(bool bDestruct)
{
   if (pMediaList)
   {
      libvlc_media_list_release (pMediaList);
      pMediaList = NULL;
   }

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
#ifdef Q_OS_MAC
      // releasing it on Mac leads to crash if you end the
      // player with running video ... no problem at all 'cause
      // we want release at program close!
      if (bDestruct)
      {
         libvlc_retain(pVlcInstance);
      }
      else
      {
         libvlc_release(pVlcInstance);
      }
#else
      Q_UNUSED(bDestruct)
      libvlc_release(pVlcInstance);
#endif

      pVlcInstance = NULL;
   }
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
|  Method: initPlayer
|  Begin: 24.02.2010 / 14:00:51
|  Author: Jo2003
|  Description: init player with arguments
|
|  Parameters: additions options for libVLC
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::initPlayer(const QString &sOpts)
{
   int          iRV     = -1;
   int          i;
   int          iEventCount;
   int          argc    = 0;
   QStringList  slOpts;

   vArgs.clear();

   // set verbose mode ...
   if (pSettings->libVlcVerboseLevel() <= 0)
   {
      vArgs.append("--quiet");
   }
   else if (pSettings->libVlcVerboseLevel() >= 2)
   {
      vArgs.append("--verbose=2");
   }
   else
   {
      vArgs.append("--verbose=1");
   }

   // reset crop and aspect cbx ... because it should show the state
   // as used ...
   missionControl.vidFormCbxSetCurrentIndex(0, QFusionControl::CBX_ASPECT);
   missionControl.vidFormCbxSetCurrentIndex(0, QFusionControl::CBX_CROP);

   vArgs.append("--ignore-config");
   vArgs.append("--intf=dummy");
   vArgs.append("--no-media-library");
   vArgs.append("--no-osd");
   vArgs.append("--no-stats");
   vArgs.append("--no-video-title-show");

#ifdef Q_WS_MAC
   // vout as well as opengl-provider MIGHT be "minimal_macosx" ...
   vArgs.append("--vout=macosx");
#endif

   // do we have to add additional options from mod file?
   if (!sOpts.isEmpty())
   {
      slOpts = sOpts.split(" ", QString::SkipEmptyParts);

      for (i = 0; i < slOpts.count(); i++)
      {
         vArgs.append(slOpts[i].toUtf8().constData());
      }

      slOpts.clear();
   }

   // fill vlcArgs array - up to MAX_LVLC_ARGS options
   // are supported ...
   for (i = 0; i < MAX_LVLC_ARGS; i++)
   {
      if (i < vArgs.count())
      {
         vlcArgs[i] = vArgs[i].constData();
         slOpts << vlcArgs[i];
      }
      else
      {
         vlcArgs[i] = NULL;
      }
   }

   mInfo(tr("Create libVLC with following global options:\n %1").arg(slOpts.join(" ")));

   argc         = (vArgs.count() > MAX_LVLC_ARGS) ? MAX_LVLC_ARGS : vArgs.count();
   pVlcInstance = libvlc_new(argc, vlcArgs);

   if (pVlcInstance)
   {
      // get mediaplayer ...
      if ((pMediaPlayer = libvlc_media_player_new (pVlcInstance)) != NULL)
      {
         // add player to window ...
         connectToVideoWidget();

         // get volume ...
         missionControl.setVolSliderPosition(libvlc_audio_get_volume (pMediaPlayer));

         // switch off handling of hotkeys ...
         libvlc_video_set_key_input(pMediaPlayer, 0);

         libvlc_video_set_mouse_input(pMediaPlayer, 0);

         // create media list player ...
         if ((pMedialistPlayer = libvlc_media_list_player_new(pVlcInstance)) != NULL)
         {
            libvlc_media_list_player_set_media_player (pMedialistPlayer, pMediaPlayer);

            // get event manager ...
            if ((pEMPlay = libvlc_media_player_event_manager(pMediaPlayer)) != NULL)
            {
               // if we've got the event manager, register for some events ...
               libvlc_event_type_t eventsMediaPlayer[] = {
                  libvlc_MediaPlayerEncounteredError,
                  libvlc_MediaPlayerOpening,
                  libvlc_MediaPlayerPlaying,
                  libvlc_MediaPlayerPaused,
                  libvlc_MediaPlayerStopped,
                  libvlc_MediaPlayerEndReached,
                  libvlc_MediaPlayerBuffering
               };

               // so far so good ...
               iRV = 0;

               iEventCount = sizeof(eventsMediaPlayer) / sizeof(*eventsMediaPlayer);
               for (i = 0; i < iEventCount; i++)
               {
                  iRV |= libvlc_event_attach(pEMPlay, eventsMediaPlayer[i],
                                            eventCallback, NULL);
               }
            }

            // create media list ...
            if ((pMediaList = libvlc_media_list_new(pVlcInstance)) != NULL)
            {
               // set media list ...
               libvlc_media_list_player_set_media_list (pMedialistPlayer, pMediaList);
            }
            else
            {
               mInfo(tr("Error: Can't create media list!"));
               iRV = -1;
            }
         }
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
      missionControl.setMutePixmap(QPixmap(":/player/sound_off"));
   }
   else
   {
      missionControl.setMutePixmap(QPixmap(":/player/sound_on"));
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
      iVol     = (iVol > 200) ? 200 : ((iVol < 0) ? 0 : iVol);

      if (iVol != missionControl.getVolume())
      {
         missionControl.setVolume(iVol);
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
|  Method: silentStop
|  Begin: 31.08.2012
|  Author: Jo2003
|  Description: stop playing, omit next event
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::silentStop()
{
   int iRV = 0;

   if (pMedialistPlayer)
   {
      if (isPlaying())
      {
         // don't emit next event ...
         bOmitNextEvent = true;
         iRV            = stop();
      }
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

   if (pMedialistPlayer && showInfo.canCtrlStream())
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
|  Parameters: complete command line, additional libVLC options
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CPlayer::playMedia(const QString &sCmdLine, const QString &sOpts)
{
   int                         iRV    = 0;
   libvlc_media_t             *p_md   = NULL;
   QStringList                 lArgs;
   QStringList::const_iterator cit;
   bool                        bLocal = false;

   // reset play timer stuff ...
   timer.reset();
   timer.setOffset(showInfo.lastJump() ? showInfo.lastJump() : showInfo.starts());
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
      iRV = initPlayer(sOpts);
   }

   // set aspect and crop cbx to current values ...
   slotResetVideoFormat();

   // make sure we stop the player - needed since playlist support ...
   if (isPlaying())
   {
       stop();
   }

   if (!iRV)
   {
      // clear media list ...
      clearMediaList();

      // check for local file ...
      if (!sMrl.contains("://"))
      {
         // local file to play ...
         bLocal = true;

         // add file prefix ...
         sMrl   = QString("file:///%1").arg(sMrl);

         // replace all backslashes with slashes ...
         sMrl.replace('\\', '/');
      }

      if ((p_md = libvlc_media_new_location(pVlcInstance, QUrl::toPercentEncoding(sMrl, "/:?&=%@"))) != NULL)
      {
         mInfo(tr("Media successfully created from MRL:\n --> %1").arg(sMrl));

         // do we use GPU acceleration ... ?
         if (pSettings->useGpuAcc())
         {
            mInfo(tr("Add MRL Option: %1").arg(GPU_ACC_TOKEN));
            libvlc_media_add_option(p_md, GPU_ACC_TOKEN);
         }

         // proxy and timeshift are only needed when we play a remote file ...
         if (!bLocal)
         {
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
            sMrl = QString(":input-timeshift-granularity=%1").arg(0x7FFFFFFF); // max. positive integer value (about 2047MB)  ...
            mInfo(tr("Add MRL Option: %1").arg(sMrl));
            libvlc_media_add_option(p_md, sMrl.toUtf8().constData());

            sMrl = QString(":ipv4-timeout=%1").arg(10 * 1000); // 10 sec. timeout for ipv4 connections
            mInfo(tr("Add MRL Option: %1").arg(sMrl));
            libvlc_media_add_option(p_md, sMrl.toUtf8().constData());
         }

         ///////////////////////////////////////////////////////////////////////////
         // screensaver stuff ...
         ///////////////////////////////////////////////////////////////////////////
         sMrl = ":disable-screensaver";
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

         // add commercial to media list (if any) ...
         addAd();

         // add main feature ...
         libvlc_media_list_lock (pMediaList);
         libvlc_media_list_add_media (pMediaList, p_md);
         libvlc_media_list_unlock (pMediaList);

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
|  Parameters: --
|
|  Returns: 0 --> none added
|           1 --> added
\----------------------------------------------------------------- */
int CPlayer::addAd()
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
         libvlc_media_list_lock (pMediaList);
         libvlc_media_list_add_media(pMediaList, p_mdad);
         libvlc_media_list_unlock (pMediaList);

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
         if (isPositionable() && !showInfo.streamLoader())
         {
            pos = libvlc_media_player_get_time (pMediaPlayer) / 1000;

            if (!missionControl.isPosSliderDown())
            {
               missionControl.setPosValue(pos);
               missionControl.setTime(pos);
            }
         }
         else
         {
            pos = timer.pos();

            if (!missionControl.isPosSliderDown())
            {
               // reaching the end of this show ... ?
               if (pos > mToGmt(missionControl.posMaximum()))
               {
                  // check archive program ...
                  emit sigCheckArchProg(pos);
               }

               missionControl.setPosValue(mFromGmt(pos));

               pos -= showInfo.starts();

               missionControl.setTime(pos);
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
|  Parameters: pointer to event raised, pointer to user data
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::eventCallback(const libvlc_event_t *ev, void *userdata)
{
   Q_UNUSED(userdata)

   /////////////////////////////////////////////////////////////////
   // Note:
   // Sending signals etc. from this callback brings much trouble
   // on MacOSX. So I decided to only store the event type here.
   // A poller will periodically check this value for changes.
   // Using it this way fixes the strange behavior!
   /////////////////////////////////////////////////////////////////

   // store event type so the event poller can handle it...
   CPlayer::_mtxEvt.lock();
   if (ev->type != libvlc_MediaPlayerBuffering)
   {
      CPlayer::_eventQueue.append(ev->type);
   }
   else
   {
      // store buffer percents ...
      CPlayer::_flBuffPrt = ev->u.media_player_buffering.new_cache;
   }
   CPlayer::_mtxEvt.unlock();
}

/* -----------------------------------------------------------------\
|  Method: slotEventPoll [slot]
|  Begin: 11.08.2012
|  Author: Jo2003
|  Description: poll for state changes
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotEventPoll()
{
   libvlc_event_type_t lastEvent = libvlc_MediaPlayerNothingSpecial;
   float               buffPercent;
   bool                bEmpty;

   // lock event vector ...
   _mtxEvt.lock();

   // get buffer percent ...
   buffPercent = _flBuffPrt;

   if (!(bEmpty = _eventQueue.isEmpty()))
   {
      // get next event ...
      lastEvent = _eventQueue.at(0);

      // delete event from queue ...
      _eventQueue.remove(0);
   }
   _mtxEvt.unlock();

   // signal buffer state ...
   if (isPlaying())
   {
      emit sigBuffPercent((int)buffPercent);
      missionControl.setBuff((int)buffPercent);
   }

   if (!bEmpty)
   {
      if (!bOmitNextEvent)
      {
         // what happened ?
         switch (lastEvent)
         {
         // error ...
         case libvlc_MediaPlayerEncounteredError:
            mInfo("libvlc_MediaPlayerEncounteredError --> clean up libVLC!");
            emit sigPlayState((int)IncPlay::PS_ERROR);
            stopPlayTimer();

            // no way to go on ... prepare to use a new instance of libVLC
            cleanupLibVLC();
            break;

         // opening media ...
         case libvlc_MediaPlayerOpening:
            mInfo("libvlc_MediaPlayerOpening ...");
            emit sigPlayState((int)IncPlay::PS_OPEN);
            break;

         // playing media ...
         case libvlc_MediaPlayerPlaying:
            mInfo("libvlc_MediaPlayerPlaying ...");
            emit sigPlayState((int)IncPlay::PS_PLAY);
            tAspectShot.start();
            startPlayTimer();
            initSlider();
            break;

         // player paused ...
         case libvlc_MediaPlayerPaused:
            mInfo("libvlc_MediaPlayerPaused ...");
            emit sigPlayState((int)IncPlay::PS_PAUSE);
            pausePlayTimer();
            break;

         // player stopped ...
         case libvlc_MediaPlayerStopped:
            mInfo("libvlc_MediaPlayerStopped ...");
            emit sigPlayState((int)IncPlay::PS_STOP);
            resetBuffPercent();
            stopPlayTimer();
            break;

         // end of media reached ...
         case libvlc_MediaPlayerEndReached:
            mInfo("libvlc_MediaPlayerEndReached ...");
            emit sigPlayState((int)IncPlay::PS_END);
            stopPlayTimer();
            break;

         default:
            mInfo(tr("Unknown Event No. %1 received ...").arg(lastEvent));
            break;
         }
      } // !bOmitNextEvent
      else
      {
         bOmitNextEvent = false;
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotCbxAspectCurrentIndexChanged
|  Begin: 08.03.2010 / 09:55:10
|  Author: Jo2003
|  Description: set new aspect ration ...
|
|  Parameters: new aspect ratio as string ...
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotCbxAspectCurrentIndexChanged(int idx)
{
   if (pMediaPlayer)
   {
      if (libvlc_media_player_is_playing (pMediaPlayer))
      {
         // set new aspect ratio ...
         libvlc_video_set_aspect_ratio(pMediaPlayer, _pAspect[idx]);

         mInfo(tr("Aspect ratio: %1").arg(aspectCropToString(_pAspect[idx])));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotCbxCropCurrentIndexChanged
|  Begin: 23.03.2010 / 09:55:10
|  Author: Jo2003
|  Description: set new crop geometry ...
|
|  Parameters: new crop geometry as string ...
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotCbxCropCurrentIndexChanged(int idx)
{
   if (pMediaPlayer)
   {
      if (libvlc_media_player_is_playing (pMediaPlayer))
      {
         // set new aspect ratio ...
         libvlc_video_set_crop_geometry(pMediaPlayer, _pCrop[idx]);

         mInfo(tr("Crop ratio: %1").arg(aspectCropToString(_pCrop[idx])));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotBtnSaveAspectCropClicked [slot]
|  Begin: 15.08.2012
|  Author: Jo2003
|  Description: save video format as set in cbx aspect + crop
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotBtnSaveAspectCropClicked()
{
   if ((missionControl.vidFormCbxCurrentText(QFusionControl::CBX_ASPECT) == "std")
       && (missionControl.vidFormCbxCurrentText(QFusionControl::CBX_CROP) == "std"))
   {
      // default values --> delete from DB ...
      pDb->delAspect(showInfo.channelId());
   }
   else
   {
      pDb->addAspect(showInfo.channelId(),
                     missionControl.vidFormCbxCurrentText(QFusionControl::CBX_ASPECT),
                     missionControl.vidFormCbxCurrentText(QFusionControl::CBX_CROP));
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
      int idx = missionControl.vidFormCbxCurrentIndex(QFusionControl::CBX_ASPECT);
      idx ++;

      // if end reached, start with index 0 ...
      if (idx >= missionControl.vidFormCbxCount(QFusionControl::CBX_ASPECT))
      {
         idx = 0;
      }

      // set new aspect ratio ...
      missionControl.vidFormCbxSetCurrentIndex(idx, QFusionControl::CBX_ASPECT);

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
      int idx = missionControl.vidFormCbxCurrentIndex(QFusionControl::CBX_CROP);
      idx ++;

      // if end reached, start with index 0 ...
      if (idx >= missionControl.vidFormCbxCount(QFusionControl::CBX_CROP))
      {
         idx = 0;
      }

      // set new crop ratio ...
      missionControl.vidFormCbxSetCurrentIndex(idx, QFusionControl::CBX_CROP);

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
   if (isPlaying() && showInfo.canCtrlStream() &&!bSpoolPending)
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

         missionControl.setPosValue((int)(pos / 1000));
      }
      else
      {
         // get new gmt value ...
         pos = timer.pos() + iSeconds;

         // trigger request for the new stream position ...
         QString req = QString("cid=%1&gmt=%2")
                          .arg(showInfo.channelId()).arg(pos);

         // mark spooling as active ...
         bSpoolPending = true;

         enableDisablePlayControl (false);

         // save jump time ...
         showInfo.setLastJumpTime(pos);

         pApiClient->queueRequest(CIptvDefs::REQ_ARCHIV, req, showInfo.pCode());

         // do we reach another show?
         if ((pos < mToGmt(missionControl.posMinimum()))
             || (pos > mToGmt(missionControl.posMaximum())))
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
   if (pMediaPlayer)
   {
      if (libvlc_media_player_is_playing (pMediaPlayer))
      {
         QString sAspect, sCrop;
         int     iIdxOld, iIdxNew;
         bool    bErr = false;

         // enable spooling again ...
         bSpoolPending = false;
         enableDisablePlayControl (true);

         if(!pDb->aspect(showInfo.channelId(), sAspect, sCrop))
         {
            // change combo box value for aspect ratio ...
            iIdxOld      = missionControl.vidFormCbxCurrentIndex(QFusionControl::CBX_ASPECT);
            if ((iIdxNew = missionControl.vidFormCbxFindText(sAspect, QFusionControl::CBX_ASPECT)) != -1)
            {
               if (iIdxOld != iIdxNew)
               {
                  // updating combobox' actual value will also
                  // trigger the libVLC call ...
                  missionControl.vidFormCbxSetCurrentIndex(iIdxNew, QFusionControl::CBX_ASPECT);
               }
               else
               {
                  // since values don't differ, updating combobox will not
                  // trigger format change. So set it directly to libVLC ...
                  libvlc_video_set_aspect_ratio(pMediaPlayer, _pAspect[iIdxNew]);
               }
            }
            else
            {
               bErr = true;
            }

            // change combo box value for crop ratio ...
            iIdxOld      = missionControl.vidFormCbxCurrentIndex(QFusionControl::CBX_CROP);
            if ((iIdxNew = missionControl.vidFormCbxFindText(sCrop, QFusionControl::CBX_CROP)) != -1)
            {
               if (iIdxOld != iIdxNew)
               {
                  // updating combobox' actual value will also
                  // trigger the libVLC call ...
                  missionControl.vidFormCbxSetCurrentIndex(iIdxNew, QFusionControl::CBX_CROP);
               }
               else
               {
                  // since values don't differ, updating combobox will not
                  // trigger format change. So set it directly to libVLC ...
                  libvlc_video_set_crop_geometry(pMediaPlayer, _pCrop[iIdxNew]);
               }
            }
            else
            {
               bErr = true;
            }

            if (bErr)
            {
               // delete entry in error case ...
               pDb->delAspect(showInfo.channelId());
            }
         }
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
   if (isPlaying() && showInfo.canCtrlStream() && !bSpoolPending)
   {
      // stop slider update timer ...
      sliderTimer.stop();

      uint position = (uint)missionControl.posValue();

      if (isPositionable())
      {
         libvlc_media_player_set_time(pMediaPlayer, position * 1000);
      }
      else
      {
         position = mToGmt(position);

         // check if slider position is in 10 sec. limit ...
         if (abs(position - timer.pos()) <= 10)
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
            pApiClient->queueRequest(CIptvDefs::REQ_ARCHIV, req, showInfo.pCode());
         }
      }

      // restart slider update timer ...
      sliderTimer.start(1000);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotPositionChanged [slot]
|  Begin: 23.06.2010 / 09:10:10
|  Author: Jo2003
|  Description: update position label to relect
|               slider position change
|  Parameters: actual slider position
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotPositionChanged(int value)
{
   if (isPlaying() && showInfo.canCtrlStream())
   {
      if (!isPositionable())
      {
         value  = mToGmt(value);
         value -= showInfo.starts();
      }

      missionControl.setTime(value);
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
   if (bEnable && showInfo.canCtrlStream())
   {
      missionControl.enablePosSlider(true);
   }
   else
   {
      missionControl.enablePosSlider(false);
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

   if (isPositionable() && !showInfo.streamLoader())
   {
      // VOD stuff ...
      missionControl.setPosRange(0, (int)(uiDuration / 1000));

      missionControl.setTime(0);
   }
   else
   {
      // set slider range to seconds ...
      missionControl.setPosRange(mFromGmt(showInfo.starts()), mFromGmt(showInfo.ends()));

      if (showInfo.lastJump())
      {
         missionControl.setPosValue(mFromGmt(showInfo.lastJump()));

         missionControl.setTime(showInfo.lastJump() - showInfo.starts());
      }
      else
      {
         missionControl.setPosValue(mFromGmt(showInfo.starts()));

         missionControl.setTime(0);
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
   return mToGmt(missionControl.posValue());
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

      if (newVolume > missionControl.volMaximum())
      {
         newVolume = missionControl.volMaximum();
      }

      if(!libvlc_audio_set_volume (pMediaPlayer, newVolume))
      {
         missionControl.setMutePixmap(QPixmap(":/player/sound_on"));
         missionControl.setVolume(newVolume);
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
            missionControl.setMutePixmap(QPixmap(":/player/sound_off"));
         }
         missionControl.setVolume(newVolume);
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
      if (libvlc_audio_get_mute(pMediaPlayer))
      {
         // muted --> unmute ...
         missionControl.setMutePixmap(QPixmap(":/player/sound_on"));
         libvlc_audio_set_mute(pMediaPlayer, 0);
      }
      else
      {
         // unmuted --> mute ...
         missionControl.setMutePixmap(QPixmap(":/player/sound_off"));
         libvlc_audio_set_mute(pMediaPlayer, 1);
      }
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
   ulong gmt = timer.pos();
   timer.reset();
   timer.setOffset(gmt);
   timer.start();

   // set slider range to seconds ...
   missionControl.setPosRange(mFromGmt(showInfo.starts()), mFromGmt(showInfo.ends()));
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

/* -----------------------------------------------------------------\
|  Method: slotWindowed
|  Begin: 25.11.2013
|  Author: Jo2003
|  Description: windowed mode toggled
|
|  Parameters: enabled or disabled
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotWindowed(int on)
{
   ui->videoWidget->windowed(on);
}

/* -----------------------------------------------------------------\
|  Method: clearMediaList
|  Begin: 13.08.2012
|  Author: Jo2003
|  Description: clear medial list
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CPlayer::clearMediaList()
{
   int iRV = 0;

   if (pMediaList)
   {
      int iCount;

      // media list should be locked while removing items ...
      libvlc_media_list_lock (pMediaList);

      do {

         // are there some items to remove ... ?
         iCount = libvlc_media_list_count (pMediaList);

         if (iCount > 0)
         {
            // remove first list item ...
            if (!libvlc_media_list_remove_index(pMediaList, 0))
            {
               iCount --;
            }
            else
            {
               iRV |= -1;
               mInfo(tr("Error: Can't delete media item from media list!"));
            }
         }

      } while (iCount > 0);

      libvlc_media_list_unlock (pMediaList);
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: libvlcVersion
|  Begin: 14.08.2012
|  Author: Jo2003
|  Description: return libVlcVersion as ULONG
|
|  Parameters: --
|
|  Returns: 0 --> parse error
|        else --> version
\----------------------------------------------------------------- */
ulong CPlayer::libvlcVersion()
{
   return ulLibvlcVersion;
}

/* -----------------------------------------------------------------\
|  Method: aspectCropToString
|  Begin: 03.09.2012
|  Author: Jo2003
|  Description: format aspect / crop value into string
|
|  Parameters: pointer to aspect / crop
|
|  Returns: formatted string
\----------------------------------------------------------------- */
QString CPlayer::aspectCropToString (const char *pFormat)
{
   QString format = pFormat;
   QString divident, divisor;
   QRegExp rx("^([^:]*):(.*)$");

   if (format == "")
   {
      format = "std";
   }
   else if (rx.indexIn(format) > -1)
   {
      divident = rx.cap(1);
      divisor  = rx.cap(2);

      if (divisor.toUInt() == 100)
      {
         format = QString("%1:1").arg((double)(divident.toUInt() / 100.0));
      }
   }

   return format;
}

/* -----------------------------------------------------------------\
|  Method: slotResetVideoFormat [slot]
|  Begin: 04.10.2012
|  Author: Jo2003
|  Description: reset aspect / crop to std
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::slotResetVideoFormat()
{
   char *pForm;
   uint  idx;
   bool  bFound;

   if (pMediaPlayer)
   {
      ///////////////////////// ASPECT RATIO //////////////////////////////////
      bFound = false;

      if ((pForm = libvlc_video_get_aspect_ratio(pMediaPlayer)) != NULL)
      {
         for (idx = 0; (idx < (sizeof(_pAspect) / sizeof (_pAspect[0]))) && !bFound; idx++)
         {
            if (!strcmp(pForm, _pAspect[idx]))
            {
               missionControl.vidFormCbxSetCurrentIndex(idx, QFusionControl::CBX_ASPECT);
               bFound = true;
            }
         }

         libvlc_free((void *)pForm);
      }

      if (!bFound)
      {
         missionControl.vidFormCbxSetCurrentIndex(0, QFusionControl::CBX_ASPECT);
         libvlc_video_set_aspect_ratio(pMediaPlayer, "");
      }

      ///////////////////////// CROP GEOMETRY ////////////////////////////////

      bFound = false;

      if ((pForm = libvlc_video_get_crop_geometry(pMediaPlayer)) != NULL)
      {
         for (idx = 0; (idx < (sizeof(_pCrop) / sizeof (_pCrop[0]))) && !bFound; idx++)
         {
            if (!strcmp(pForm, _pCrop[idx]))
            {
               missionControl.vidFormCbxSetCurrentIndex(idx, QFusionControl::CBX_CROP);
               bFound = true;
            }
         }

         libvlc_free((void *)pForm);
      }

      if (!bFound)
      {
         missionControl.vidFormCbxSetCurrentIndex(0, QFusionControl::CBX_CROP);
         libvlc_video_set_crop_geometry(pMediaPlayer, "");
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: resetBuffPercent
|  Begin: 19.04.2013
|  Author: Jo2003
|  Description: rest buffer percent value
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::resetBuffPercent()
{
   _mtxEvt.lock();
   _flBuffPrt = 0;
   _mtxEvt.unlock();
}

//---------------------------------------------------------------------------
//
//! \brief   check audio tracks when stream really plays
//
//! \author  Jo2003
//! \date    09.07.2013
//
//! \param   percent (int) buffer in percent
//
//! \return  --
//---------------------------------------------------------------------------
void CPlayer::slotFinallyPlays(int percent)
{
   libvlc_track_description_t* pAuTracks = NULL;
   int               iAuIdx;
   QLangVector       lv;
   vlcvid::SContLang al;

   if (percent == 0)
   {
      bScanAuTrk = true;
   }
   else if ((percent >= 99) && bScanAuTrk)
   {
      if (isPlaying())
      {
         // get audio track description ...
         pAuTracks = libvlc_audio_get_track_description(pMediaPlayer);

         // get current index ...
         iAuIdx    = libvlc_audio_get_track(pMediaPlayer);

         mInfo(tr("Scan for Audio tracks:"));

         while (pAuTracks != NULL)
         {
            if (pAuTracks->i_id >= 0)
            {
               al.desc    = QString::fromUtf8(pAuTracks->psz_name);
               al.id      = pAuTracks->i_id;
               al.current = (iAuIdx == pAuTracks->i_id) ? true : false;

               lv.append(al);

               mInfo(tr("-> Audio track %1 %2%3")
                     .arg(pAuTracks->i_id)
                     .arg(QString::fromUtf8(pAuTracks->psz_name))
                     .arg((iAuIdx == pAuTracks->i_id) ? " (current)" : ""));
            }
            pAuTracks = pAuTracks->p_next;
         }

         emit sigAudioTracks(lv);
      }
      bScanAuTrk = false;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   enable / disable deinterlacing
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   bDeintl (bool) deinterlace or even not ...
//
//! \return  --
//---------------------------------------------------------------------------
void CPlayer::slotDeinterlace(bool bDeintl)
{
   if (pMediaPlayer)
   {
      mInfo(tr("%1 %2 deinterlace").arg(bDeintl ? "enable" : "disable").arg(pSettings->getDeinlMode()));
      libvlc_video_set_deinterlace(pMediaPlayer, bDeintl ? pSettings->getDeinlMode().toUtf8().constData() : NULL);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   switch to other audio track
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   id (int) audio track id
//
//! \return  --
//---------------------------------------------------------------------------
void CPlayer::slotChangeATrack(int id)
{
   int iRet;
   if (pMediaPlayer)
   {
      iRet = libvlc_audio_set_track(pMediaPlayer, id);

      mInfo(tr("Change audio track to id %1: %2!").arg(id).arg((iRet == 0) ? "ok" : "error"));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   take a screenshot and store it in video folder
//
//! \author  Jo2003
//! \date    13.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void CPlayer::slotTakeScreenShot()
{
   if (pMediaPlayer)
   {
      if (isPlaying())
      {
         libvlc_video_take_snapshot(pMediaPlayer, 0, pSettings->GetTargetDir().toUtf8().constData(), 0, 0);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get pointer to video widget
//
//! \author  Jo2003
//! \date    25.11.2013
//
//
//! \return  pointer to video widget
//---------------------------------------------------------------------------
QVlcVideoWidget*& CPlayer::getVideoWidget()
{
   return ui->videoWidget;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

