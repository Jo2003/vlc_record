/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 16:01:09
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "recorder.h"
#include "small_helpers.h"

#ifndef INCLUDE_LIBVLC
   #include "ui_recorder.h"
#else
   #include "ui_recorder_inc.h"
#endif

// for logging ...
extern CLogFile VlcLog;

// for folders ...
extern CDirStuff *pFolders;

// global showinfo class ...
extern CShowInfo showInfo;

/* -----------------------------------------------------------------\
|  Method: Recorder / constructor
|  Begin: 19.01.2010 / 16:01:44
|  Author: Jo2003
|  Description: create recorder object, init values,
|               make connections
|
|  Parameters: pointer to translator, pointer to parent widget
|
|  Returns: --
\----------------------------------------------------------------- */
Recorder::Recorder(QTranslator *trans, QWidget *parent)
    : QDialog(parent, Qt::Window),
    ui(new Ui::Recorder)
{
   ui->setupUi(this);

#ifdef INCLUDE_LIBVLC
   // build layout stack ...
   pVideoWidget  = NULL;
   stackedLayout = new QStackedLayout();
   stackedLayout->setMargin(0);
   ui->vMainLayout->removeWidget(ui->masterFrame);
   stackedLayout->addWidget(ui->masterFrame);
   ui->vMainLayout->addLayout(stackedLayout);
#endif

   // set (customized) windows title ...
   setWindowTitle(APP_NAME);

   ePlayState     =  IncPlay::PS_WTF;
   pTranslator    =  trans;
   iEpgOffset     =  0;
   iFontSzChg     =  0;
   iDwnReqId      = -1;
   ulStartFlags   =  0;

   // init account info ...
   accountInfo.bHasArchive = false;
   accountInfo.bHasVOD     = false;
   accountInfo.sExpires    = QDateTime::currentDateTime().toString(DEF_TIME_FORMAT);

   // init genre info ...
   genreInfo.iCount        = 0;
   genreInfo.iPage         = 0;
   genreInfo.iTotal        = 0;
   genreInfo.sType         = "wtf";

   // init VOD site backup ...
   lastVodSite.iScrollBarVal = 0;

   // init favourite buttons ...
   for (int i = 0; i < MAX_NO_FAVOURITES; i++)
   {
      pFavBtn[i]     = NULL;
      pFavAct[i]     = NULL;
      pContextAct[i] = NULL;
   }

   VlcLog.SetLogFile(pFolders->getDataDir(), APP_LOG_FILE);

   // set channel list model and delegate ...
   pModel    = new QStandardItemModel;
   pDelegate = new QChanListDelegate;

   ui->channelList->setItemDelegate(pDelegate);
   ui->channelList->setModel(pModel);

   // update checker ...
   pUpdateChecker = new QNetworkAccessManager(this);

   // set this dialog as parent for settings and timerRec ...
   Settings.setParent(this, Qt::Dialog);
   secCodeDlg.setParent(this, Qt::Dialog);
   Settings.setXmlParser(&XMLParser);
   Settings.setWaitTrigger(&Trigger);
   Settings.setAccountInfo(&accountInfo);
   timeRec.setParent(this, Qt::Dialog);
   trayIcon.setParent(this);
   vlcCtrl.setParent(this);
   favContext.setParent(this, Qt::Popup);

   // set host for pix cache ...
   pixCache.setHost(Settings.GetAPIServer());

   // set settings for vod browser ...
   ui->vodBrowser->setSettings(&Settings);

   // set pix cache ...
   ui->vodBrowser->setPixCache(&pixCache);

   // set log level ...
   VlcLog.SetLogLevel(Settings.GetLogLevel());

   // log folder locations ...
   mInfo (tr("\ndataDir: %1\n").arg(pFolders->getDataDir())
          + tr("logoDir: %1\n").arg(pFolders->getLogoDir())
          + tr("langDir: %1\n").arg(pFolders->getLangDir())
          + tr("modDir:  %1\n").arg(pFolders->getModDir())
          + tr("appDir:  %1").arg(pFolders->getAppDir()));

   // set connection data ...
   KartinaTv.SetData(Settings.GetAPIServer(), Settings.GetUser(), Settings.GetPasswd());


   // set proxy stuff ...
   if (Settings.UseProxy())
   {
      QNetworkProxy proxy(QNetworkProxy::HttpCachingProxy,
                          Settings.GetProxyHost(), Settings.GetProxyPort(),
                          Settings.GetProxyUser(), Settings.GetProxyPasswd());

      KartinaTv.setProxy(proxy);
      pixCache.setProxy(proxy);
      streamLoader.setProxy(proxy);
      pUpdateChecker->setProxy(proxy);
   }

   // configure trigger and start it ...
   Trigger.SetKartinaClient(&KartinaTv);
   Trigger.start();

   // give vlcCtrl needed infos ...
   vlcCtrl.SetTranslitPointer(&translit);
   vlcCtrl.SetTranslitSettings(Settings.TranslitRecFile());

   // give timerRec all needed infos ...
   timeRec.SetXmlParser(&XMLParser);
   timeRec.SetKartinaTrigger(&Trigger);
   timeRec.SetSettings(&Settings);
   timeRec.SetVlcCtrl(&vlcCtrl);
   timeRec.SetStreamLoader(&streamLoader);

   // hide / remove VOD tab widget ...
   vodTabWidget.iPos    = 1;  // index of VOD tab
   vodTabWidget.icon    = ui->tabEpgVod->tabIcon(vodTabWidget.iPos);
   vodTabWidget.sText   = ui->tabEpgVod->tabText(vodTabWidget.iPos);
   vodTabWidget.pWidget = ui->tabEpgVod->widget(vodTabWidget.iPos);
   ui->tabEpgVod->removeTab(vodTabWidget.iPos);

#ifdef INCLUDE_LIBVLC

   // do we use libVLC ?
   if (Settings.GetPlayerModule().contains("libvlc", Qt::CaseInsensitive))
   {
      vlcCtrl.UseLibVlc(true);
   }
   else
   {
      vlcCtrl.UseLibVlc(false);
   }

   // give player the list of shortcuts ...
   ui->player->setShortCuts (&vShortcutPool);

   // give player settings and wait trigger access ...
   ui->player->setSettings(&Settings);
   ui->player->setTrigger(&Trigger);

   // connect vlc control with libvlc player ...
   connect (ui->player, SIGNAL(sigPlayState(int)), &vlcCtrl, SLOT(slotLibVlcStateChange(int)));
   connect (&vlcCtrl, SIGNAL(sigLibVlcPlayMedia(QString)), ui->player, SLOT(playMedia(QString)));
   connect (&vlcCtrl, SIGNAL(sigLibVlcStop()), ui->player, SLOT(stop()));

   // aspect ratio, crop and full screen ...
   connect (this, SIGNAL(sigToggleFullscreen()), ui->player, SLOT(on_btnFullScreen_clicked()));
   connect (this, SIGNAL(sigToggleAspectRatio()), ui->player, SLOT(slotToggleAspectRatio()));
   connect (this, SIGNAL(sigToggleCropGeometry()), ui->player, SLOT(slotToggleCropGeometry()));

   // get state if libVLC player to change player state display ...
   connect (ui->player, SIGNAL(sigPlayState(int)), this, SLOT(slotIncPlayState(int)));

   // short info update on archive play ...
   connect (ui->player, SIGNAL(sigCheckArchProg(ulong)), this, SLOT(slotCheckArchProg(ulong)));
   connect (this, SIGNAL(sigShowInfoUpdated()), ui->player, SLOT(slotShowInfoUpdated()));

   connect (ui->player, SIGNAL(sigToggleFullscreen()), this, SLOT(slotToogleFullscreen()));
   connect (this, SIGNAL(sigFullScreenToggled(int)), ui->player, SLOT(slotFsToggled(int)));


#endif /* INCLUDE_LIBVLC */

   // connect signals and slots ...
   connect (&pixCache,     SIGNAL(allDone()), this, SLOT(slotRefreshChanLogos()));
   connect (&KartinaTv,    SIGNAL(sigHttpResponse(QString,int)), this, SLOT(slotKartinaResponse(QString,int)));
   connect (&KartinaTv,    SIGNAL(sigError(QString,int,int)), this, SLOT(slotKartinaErr(QString,int,int)));
   connect (&streamLoader, SIGNAL(sigStreamDownload(int,QString)), this, SLOT(slotDownloadStarted(int,QString)));
   connect (&Refresh,      SIGNAL(timeout()), &Trigger, SLOT(slotReqChanList()));
   connect (ui->textEpg,   SIGNAL(anchorClicked(QUrl)), this, SLOT(slotEpgAnchor(QUrl)));
   connect (&Settings,     SIGNAL(sigReloadLogos()), this, SLOT(slotReloadLogos()));
   connect (&Settings,     SIGNAL(sigSetServer(QString)), this, SLOT(slotSetSServer(QString)));
   connect (&Settings,     SIGNAL(sigSetBitRate(int)), this, SLOT(slotSetBitrate(int)));
   connect (&Settings,     SIGNAL(sigSetTimeShift(int)), this, SLOT(slotSetTimeShift(int)));
   connect (&timeRec,      SIGNAL(sigRecDone()), this, SLOT(slotTimerRecordDone()));
   connect (&timeRec,      SIGNAL(sigRecActive(int)), this, SLOT(slotTimerRecActive(int)));
   connect (&trayIcon,     SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotSystrayActivated(QSystemTrayIcon::ActivationReason)));
   if (Settings.HideToSystray())
   {
      connect (this,          SIGNAL(sigHide()), &trayIcon, SLOT(show()));
      connect (this,          SIGNAL(sigShow()), &trayIcon, SLOT(hide()));
   }
   connect (&vlcCtrl,       SIGNAL(sigVlcStarts(int)), this, SLOT(slotVlcStarts(int)));
   connect (&vlcCtrl,       SIGNAL(sigVlcEnds(int)), this, SLOT(slotVlcEnds(int)));
   connect (&timeRec,       SIGNAL(sigShutdown()), this, SLOT(slotShutdown()));
   connect (ui->channelList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotChanListContext(QPoint)));
   connect (&favContext,    SIGNAL(triggered(QAction*)), this, SLOT(slotChgFavourites(QAction*)));
   connect (this,           SIGNAL(sigLCDStateChange(int)), ui->labState, SLOT(updateState(int)));
   connect (ui->vodBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotVodAnchor(QUrl)));
   connect (ui->channelList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentChannelChanged(QModelIndex)));
   connect (pUpdateChecker, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotUpdateAnswer (QNetworkReply*)));
   connect (this,           SIGNAL(sigLockParentalManager()), &Settings, SLOT(slotLockParentalManager()));

   // trigger read of saved timer records ...
   timeRec.ReadRecordList();

   // -------------------------------------------
   // get favourites ...
   // -------------------------------------------
   lFavourites = Settings.GetFavourites();

   // last used EPG day ...
   QString sDate;
   if ((sDate = Settings.lastEpgDay()) != "")
   {
      iEpgOffset = QDate::currentDate().daysTo(QDate::fromString(sDate, "ddMMyyyy"));

      // if offset exceeds our limits: reset!
      if (!CSmallHelpers::inBetween(-14, 7, iEpgOffset))
      {
         iEpgOffset = 0;
      }
   }

   // enable button ...
   TouchPlayCtrlBtns(false);

   // fill type combo box ...
   touchLastOrBestCbx();

   // start refresh timer, if needed ...
   if (Settings.DoRefresh())
   {
      Refresh.start(Settings.GetRefrInt() * 60000); // 1 minutes: (60 * 1000 msec) ...
   }
}

/* -----------------------------------------------------------------\
|  Method: ~Recorder / dstructor
|  Begin: 19.01.2010 / 16:04:40
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
Recorder::~Recorder()
{
   delete ui;

   if (pModel)
   {
      delete pModel;
      pModel = NULL;
   }

   if (pDelegate)
   {
      delete pDelegate;
      pDelegate = NULL;
   }

   if (pUpdateChecker)
   {
      delete pUpdateChecker;
      pUpdateChecker = NULL;
   }
#ifdef INCLUDE_LIBVLC
   if (stackedLayout)
   {
      delete stackedLayout;
      stackedLayout = NULL;
   }
#endif // INCLUDE_LIBVLC
}

////////////////////////////////////////////////////////////////////////////////
//                           Events                                           //
////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------\
|  Method: changeEvent
|  Begin: 19.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: catch language change event
|
|  Parameters: pointer to event
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::changeEvent(QEvent *e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
   // catch minimize event ...
   case QEvent::WindowStateChange:

      // only hide window, if trayicon stuff is available ...
      if (QSystemTrayIcon::isSystemTrayAvailable ())
      {
         if (isMinimized())
         {
            QWindowStateChangeEvent *pEvent = (QWindowStateChangeEvent *)e;

            // store last position only, if it wasn't maximized or fullscreen ...
            if (!(pEvent->oldState() & (Qt::WindowMaximized | Qt::WindowFullScreen)))
            {
               sizePos = geometry();
            }

            if (Settings.HideToSystray())
            {
               // hide dialog ...
               QTimer::singleShot(300, this, SLOT(hide()));
            }
         }
      }
      break;

   // language switch ...
   case QEvent::LanguageChange:
      ui->retranslateUi(this);

      // translate manual created navbar ...
      TouchEpgNavi (false);

      // translate systray tooltip ...
      CreateSystray();

      // translate type cbx ...
      touchLastOrBestCbx();

      // translate genre cbx ...
      touchGenreCbx();

      // translate shortcut table ...
      retranslateShortcutTable();

      // translate error strings ...
      KartinaTv.fillErrorMap();
      break;

   default:
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: closeEvent
|  Begin: 01.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: catch close event
|
|  Parameters: pointer to close event
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::closeEvent(QCloseEvent *event)
{
   bool bAccept = true;

   // if vlc is running, ask if we want
   // to close it ...
   switch (ePlayState)
   {
   case IncPlay::PS_RECORD:
   case IncPlay::PS_TIMER_RECORD:
   case IncPlay::PS_TIMER_STBY:
      if (!WantToStopRec())
      {
         bAccept = false;
      }
      break;

   default:
      break;
   }

   if (bAccept)
   {
      // disconnect trayicon stuff ...
      disconnect (&trayIcon);

      // We want to close program, store all needed values ...
      // Note: putting this function in destructor doesn't work!
      savePositions();

      // save channel and epg position ...
      Settings.saveChannel(getCurrentCid());
      Settings.saveEpgDay(iEpgOffset ? QDate::currentDate().addDays(iEpgOffset).toString("ddMMyyyy") : "");

      // clear shortcuts ...
      ClearShortCuts ();

      // clean favourites ...
      lFavourites.clear();
      HandleFavourites();

      // delete context menu stuff ...
      CleanContextMenu();

      // cancel any running kartina request ...
      Trigger.TriggerRequest (Kartina::REQ_ABORT);

      // are we authenticated ... ?
      if (KartinaTv.cookieSet())
      {
         // logout from kartina ...
         Trigger.TriggerRequest (Kartina::REQ_LOGOUT);

         // ignore event here ...
         // we'll close app in logout slot ...
         event->ignore ();
      }
      else
      {
         // no logout needed ...
         // close programm right now ...
         event->accept();
      }
   }
   else
   {
      event->ignore();
   }
}

/* -----------------------------------------------------------------\
|  Method: keyPressEvent
|  Begin: 01.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: catch esc key
|
|  Parameters: pointer to keypress event
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::keyPressEvent(QKeyEvent *event)
{
   switch (event->key())
   {
   case Qt::Key_Escape:
      // ignore escape key ...
      event->ignore();
      break;

   default:
      event->accept();
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: showEvent
|  Begin: 30.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: catch show event and emit show signal
|
|  Parameters: event pointer
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::showEvent(QShowEvent *event)
{
   emit sigShow();

   if (!(ulStartFlags & FLAG_CONN_CHAIN))
   {
      ulStartFlags |= FLAG_CONN_CHAIN;

      // start connection stuff in 0.5 seconds ...
      QTimer::singleShot(500, this, SLOT(slotStartConnectionChain()));
   }

   QWidget::showEvent(event);
}

/* -----------------------------------------------------------------\
|  Method: hideEvent
|  Begin: 30.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: catch hide event and emit hide signal
|
|  Parameters: event pointer
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::hideEvent(QHideEvent *event)
{
   emit sigHide();
   QWidget::hideEvent(event);
}

////////////////////////////////////////////////////////////////////////////////
//                           "on_" - Slots                                    //
////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------\
|  Method: on_pushSettings_clicked
|  Begin: 19.01.2010 / 16:07:19
|  Author: Jo2003
|  Description: show settings dialog
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushSettings_clicked()
{
   // pause EPG reload ...
   if (Refresh.isActive())
   {
      Refresh.stop();
   }

   if (Settings.exec() == QDialog::Accepted)
   {
      // if changes where saved, accept it here ...
      VlcLog.SetLogLevel(Settings.GetLogLevel());

      pModel->clear();
      KartinaTv.abort();

      // update connection data ...
      KartinaTv.SetData(Settings.GetAPIServer(), Settings.GetUser(), Settings.GetPasswd());

      // set proxy ...
      if (Settings.UseProxy())
      {
         QNetworkProxy proxy(QNetworkProxy::HttpCachingProxy,
                             Settings.GetProxyHost(), Settings.GetProxyPort(),
                             Settings.GetProxyUser(), Settings.GetProxyPasswd());

         KartinaTv.setProxy(proxy);
         pixCache.setProxy(proxy);
         streamLoader.setProxy(proxy);
         pUpdateChecker->setProxy(proxy);
      }

      // set language as read ...
      (pTranslator + Translators::TRANS_OWN)->load(QString("lang_%1").arg(Settings.GetLanguage ()), pFolders->getLangDir());
      (pTranslator + Translators::TRANS_QT)->load(QString("qt_%1").arg(Settings.GetLanguage ()), pFolders->getQtLangDir());

#ifdef INCLUDE_LIBVLC
      // do we use libVLC ?
      if (Settings.GetPlayerModule().contains("libvlc", Qt::CaseInsensitive))
      {
         vlcCtrl.UseLibVlc(true);
      }
      else
      {
         vlcCtrl.UseLibVlc(false);
      }
#endif /* INCLUDE_LIBVLC */

      // give vlcCtrl needed infos ...
      vlcCtrl.LoadPlayerModule(Settings.GetPlayerModule());
      vlcCtrl.SetTranslitSettings(Settings.TranslitRecFile());

      TouchPlayCtrlBtns(false);

      // authenticate ...
      Trigger.TriggerRequest(Kartina::REQ_COOKIE);

      // set refresh timer ...
      if (Settings.DoRefresh())
      {
         if (!Refresh.isActive())
         {
            Refresh.start(Settings.GetRefrInt() * 60000); // 1 minutes: (60 * 1000 msec) ...
         }
      }
      else
      {
         if (Refresh.isActive())
         {
            Refresh.stop();
         }
      }
   }

   // lock parental manager ...
   emit sigLockParentalManager();

   if (Settings.HideToSystray())
   {
      connect (this, SIGNAL(sigHide()), &trayIcon, SLOT(show()));
      connect (this, SIGNAL(sigShow()), &trayIcon, SLOT(hide()));
   }
   else
   {
      disconnect(this, SIGNAL(sigHide()));
      disconnect(this, SIGNAL(sigShow()));
   }

   // enable EPG reload again ...
   if (Settings.DoRefresh() && !Refresh.isActive())
   {
      Refresh.start(Settings.GetRefrInt() * 60000); // 1 minutes: (60 * 1000 msec) ...
   }
}

/* -----------------------------------------------------------------\
|  Method: on_pushRecord_clicked
|  Begin: 19.01.2010 / 16:11:52
|  Author: Jo2003
|  Description: request stream url for record
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushRecord_clicked()
{
#ifdef INCLUDE_LIBVLC

   // is archive play active ...
   if ((showInfo.showType() == ShowInfo::Archive)
      && (showInfo.playState () == IncPlay::PS_PLAY))
   {
      if (AllowAction(IncPlay::PS_RECORD))
      {
         // archive play active ...
         uint    gmt = ui->player->getSilderPos ();
         QString req = QString("cid=%1&gmt=%2").arg(showInfo.channelId()).arg(gmt);

         showInfo.setPlayState(IncPlay::PS_RECORD);

         TouchPlayCtrlBtns(false);
         Trigger.TriggerRequest(Kartina::REQ_ARCHIV, req, showInfo.pCode());
      }
   }
   else
   {

#endif // INCLUDE_LIBVLC
      int cid = getCurrentCid();

      if (chanMap.contains(cid))
      {
         if (AllowAction(IncPlay::PS_RECORD))
         {
            cparser::SChan chan = chanMap[cid];

            if (grantAdultAccess(chan.bIsProtected))
            {
               // new own downloader ...
               if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
               {
                  streamLoader.stopDownload (iDwnReqId);
                  iDwnReqId = -1;
               }

               showInfo.cleanShowInfo();
               showInfo.setChanId(cid);
               showInfo.setChanName(chan.sName);
               showInfo.setShowType(ShowInfo::Live);
               showInfo.setShowName(chan.sProgramm);
               showInfo.setStartTime(chan.uiStart);
               showInfo.setEndTime(chan.uiEnd);
               showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
               showInfo.setPCode(secCodeDlg.passWd());
               showInfo.setPlayState(IncPlay::PS_RECORD);
               showInfo.setHtmlDescr((QString(TMPL_BACKCOLOR)
                                      .arg("rgb(255, 254, 212)")
                                      .arg(CShowInfo::createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd))));

               TouchPlayCtrlBtns(false);
               Trigger.TriggerRequest(Kartina::REQ_STREAM, cid, secCodeDlg.passWd());
            }
         }
      }

#ifdef INCLUDE_LIBVLC
   }
#endif // INCLUDE_LIBVLC
}

/* -----------------------------------------------------------------\
|  Method: on_pushPlay_clicked
|  Begin: 19.01.2010 / 16:12:20
|  Author: Jo2003
|  Description: request stream url for play
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushPlay_clicked()
{
#ifdef INCLUDE_LIBVLC
   // play or pause functionality ...
   if ((showInfo.playState() == IncPlay::PS_PLAY)
      && showInfo.canCtrlStream())
   {
      // we're playing ... we want pause ...
      ui->player->pause();

      // update showInfo ...
      showInfo.setPlayState(IncPlay::PS_PAUSE);

      // update buttons ...
      TouchPlayCtrlBtns(true);
   }
   else if ((showInfo.playState() == IncPlay::PS_PAUSE)
      && showInfo.canCtrlStream())
   {
      // we're pausing ... want to play ...
      ui->player->play();

      // update showInfo ...
      showInfo.setPlayState(IncPlay::PS_PLAY);

      // update buttons ...
      TouchPlayCtrlBtns(true);
   }
   else
   {
#endif // INCLUDE_LIBVLC
      int cid  = getCurrentCid();

      if (chanMap.contains(cid))
      {
         if (AllowAction(IncPlay::PS_PLAY))
         {
            cparser::SChan chan = chanMap[cid];

            if (grantAdultAccess(chan.bIsProtected))
            {
               showInfo.cleanShowInfo();
               showInfo.setChanId(cid);
               showInfo.setChanName(chan.sName);
               showInfo.setShowType(ShowInfo::Live);
               showInfo.setShowName(chan.sProgramm);
               showInfo.setStartTime(chan.uiStart);
               showInfo.setEndTime(chan.uiEnd);
               showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
               showInfo.setPlayState(IncPlay::PS_PLAY);
               showInfo.setPCode(secCodeDlg.passWd());
               showInfo.setHtmlDescr((QString(TMPL_BACKCOLOR)
                                      .arg("rgb(255, 254, 212)")
                                      .arg(CShowInfo::createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd))));

               TouchPlayCtrlBtns(false);
               Trigger.TriggerRequest(Kartina::REQ_STREAM, cid, secCodeDlg.passWd());
            }
         }
      }

#ifdef INCLUDE_LIBVLC
   }
#endif // INCLUDE_LIBVLC
}

/* -----------------------------------------------------------------\
|  Method: on_cbxChannelGroup_activated
|  Begin: 19.01.2010 / 16:14:40
|  Author: Jo2003
|  Description: jump tp other channel group
|
|  Parameters: new channel group index
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_cbxChannelGroup_activated(int index)
{
   int         row = ui->cbxChannelGroup->itemData(index).toInt();
   QModelIndex idx = pModel->index(row + 1, 0);

   ui->channelList->setCurrentIndex(idx);
   ui->channelList->scrollTo(idx, QAbstractItemView::PositionAtTop);
}

/* -----------------------------------------------------------------\
|  Method: on_pushAbout_clicked
|  Begin: 19.01.2010 / 16:15:56
|  Author: Jo2003
|  Description: show about dialog
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushAbout_clicked()
{
   CAboutDialog dlg(this, accountInfo.sExpires);
   dlg.ConnectSettings(&Settings);
   dlg.exec();
}

/* -----------------------------------------------------------------\
|  Method: on_channelList_itemDoubleClicked
|  Begin: 19.01.2010 / 16:19:25
|  Author: Jo2003
|  Description: double click on channel list -> start play channel
|
|  Parameters: pointer to channelListitem ...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_channelList_doubleClicked(const QModelIndex & index)
{
   if (Settings.doubleClickToPlay())
   {
      int cid = qvariant_cast<int>(index.data(channellist::cidRole));

      if (chanMap.contains(cid))
      {
         if (AllowAction(IncPlay::PS_PLAY))
         {
            cparser::SChan chan = chanMap[cid];

            if (grantAdultAccess(chan.bIsProtected))
            {
               showInfo.cleanShowInfo();
               showInfo.setChanId(cid);
               showInfo.setChanName(chan.sName);
               showInfo.setShowType(ShowInfo::Live);
               showInfo.setShowName(chan.sProgramm);
               showInfo.setStartTime(chan.uiStart);
               showInfo.setEndTime(chan.uiEnd);
               showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
               showInfo.setPlayState(IncPlay::PS_PLAY);
               showInfo.setPCode(secCodeDlg.passWd());
               showInfo.setHtmlDescr((QString(TMPL_BACKCOLOR)
                                      .arg("rgb(255, 254, 212)")
                                      .arg(CShowInfo::createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd))));

               TouchPlayCtrlBtns(false);
               Trigger.TriggerRequest(Kartina::REQ_STREAM, cid, secCodeDlg.passWd());
            }
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: on_btnSearch_clicked
|  Begin: 21.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: search button was pressed
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnSearch_clicked()
{
   if(!ui->textEpg->find(ui->lineSearch->text()))
   {
      // not found --> set cursor to document start ...
      ui->textEpg->moveCursor(QTextCursor::Start);
   }
}

/* -----------------------------------------------------------------\
|  Method: on_lineSearch_returnPressed
|  Begin: 21.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: search line enter pressed --> search
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_lineSearch_returnPressed()
{
   if(!ui->textEpg->find(ui->lineSearch->text()))
   {
      // not found --> set cursor to document start ...
      ui->textEpg->moveCursor(QTextCursor::Start);
   }
}

/* -----------------------------------------------------------------\
|  Method: on_pushTimerRec_clicked
|  Begin: 26.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: timeRec button clicked --> show window
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushTimerRec_clicked()
{
   uint now = QDateTime::currentDateTime().toTime_t();
   int  cid = getCurrentCid();

   // timeRec.SetRecInfo(now, now, -1);
   timeRec.SetRecInfo(now, now, cid);
   timeRec.exec();
}

/* -----------------------------------------------------------------\
|  Method: on_pushStop_clicked
|  Begin: 30.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: stop button was pressed, stop vlc after request
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushStop_clicked()
{
   if (AllowAction(IncPlay::PS_STOP))
   {
      ui->labState->setHeader("");
      ui->labState->setFooter("");

      // new own downloader ...
      if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
      {
         streamLoader.stopDownload (iDwnReqId);
         iDwnReqId = -1;
      }

      vlcCtrl.stop();

      showInfo.setPlayState(IncPlay::PS_STOP);
      TouchPlayCtrlBtns(true);
   }
}

/* -----------------------------------------------------------------\
|  Method: on_btnFontSmaller_clicked
|  Begin: 02.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: reduce font size in epg view
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnFontSmaller_clicked()
{
   QFont f;
   ui->textEpg->ReduceFont();
   ui->textEpgShort->ReduceFont();
   ui->vodBrowser->ReduceFont();

   f = ui->channelList->font();
   f.setPointSize(f.pointSize() - 1);
   ui->channelList->setFont(f);

   f = ui->cbxChannelGroup->font();
   f.setPointSize(f.pointSize() - 1);
   ui->cbxChannelGroup->setFont(f);

   iFontSzChg --;
}

/* -----------------------------------------------------------------\
|  Method: on_btnFontLarger_clicked
|  Begin: 02.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: enlarge font size in epg view
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnFontLarger_clicked()
{
   QFont f;
   ui->textEpg->EnlargeFont();
   ui->textEpgShort->EnlargeFont();
   ui->vodBrowser->EnlargeFont();

   f = ui->channelList->font();
   f.setPointSize(f.pointSize() + 1);
   ui->channelList->setFont(f);

   f = ui->cbxChannelGroup->font();
   f.setPointSize(f.pointSize() + 1);
   ui->cbxChannelGroup->setFont(f);

   iFontSzChg ++;
}

#ifdef INCLUDE_LIBVLC
/* -----------------------------------------------------------------\
|  Method: on_btnBwd_clicked
|  Begin: 23.06.2010 / 12:32:12
|  Author: Jo2003
|  Description: jump backward
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushBwd_clicked()
{
   // we have minutes but need seconds --> x 60!!!
   int iJmpVal = ui->cbxTimeJumpVal->currentText().toInt() * 60;

   // jump ...
   ui->player->slotTimeJumpRelative(-iJmpVal);
}

/* -----------------------------------------------------------------\
|  Method: on_btnFwd_clicked
|  Begin: 23.06.2010 / 12:32:12
|  Author: Jo2003
|  Description: jump forward
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushFwd_clicked()
{
   // we have minutes but need seconds --> x 60!!!
   int iJmpVal = ui->cbxTimeJumpVal->currentText().toInt() * 60;

   // jump ...
   ui->player->slotTimeJumpRelative(iJmpVal);
}
#endif /* INCLUDE_LIBVLC */

/* -----------------------------------------------------------------\
|  Method: on_cbxGenre_activated [slot]
|  Begin: 20.12.2010 / 14:18
|  Author: Jo2003
|  Description: vod genre changed
|
|  Parameters: new index ...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_cbxGenre_activated(int index)
{
   // check for vod favourites ...
   QString sType = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();
   int     iGid  = ui->cbxGenre->itemData(index).toInt();
   QUrl    url;

   if (sType == "vodfav")
   {
      // set filter cbx to "last"  ...
      ui->cbxLastOrBest->setCurrentIndex(0);
      sType = "last";
   }

   url.addQueryItem("type", sType);

   if (iGid != -1)
   {
      url.addQueryItem("genre", QString::number(iGid));
   }

   Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, QString(url.encodedQuery()));
}

/* -----------------------------------------------------------------\
|  Method: on_cbxLastOrBest_activated [slot]
|  Begin: 14.09.2011 / 12:40
|  Author: Jo2003
|  Description: sort type
|
|  Parameters: new index ...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_cbxLastOrBest_activated(int index)
{
   QString sType = ui->cbxLastOrBest->itemData(index).toString();

   if (sType == "vodfav")
   {
      Trigger.TriggerRequest(Kartina::REQ_GET_VOD_FAV);
   }
   else
   {
      int  iGid  = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();
      QUrl url;

      url.addQueryItem("type", sType);

      if (iGid != -1)
      {
         url.addQueryItem("genre", QString::number(iGid));
      }

      Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, QString(url.encodedQuery()));
   }
}

/* -----------------------------------------------------------------\
|  Method: on_btnVodSearch_clicked [slot]
|  Begin: 23.12.2010 / 9:10
|  Author: Jo2003
|  Description: search in vod
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnVodSearch_clicked()
{
   int     iGid;
   QString sType;
   QUrl    url;

   if (ui->lineVodSearch->text() != "")
   {
      url.addQueryItem("type", "text");

      // when searching show up to 100 results ...
      url.addQueryItem("nums", QString::number(100));
      url.addQueryItem("query", ui->lineVodSearch->text());

      iGid = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();

      if (iGid != -1)
      {
         url.addQueryItem("genre", QString::number(iGid));
      }
   }
   else
   {
      // no text means normal list ...
      iGid  = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();
      sType = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();

      // make sure type is supported ...
      if (sType == "vodfav")
      {
         sType = "last";
         ui->cbxLastOrBest->setCurrentIndex(0);
      }

      url.addQueryItem("type", sType);

      if (iGid != -1)
      {
         url.addQueryItem("genre", QString::number(iGid));
      }
   }

   Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, QString(url.encodedQuery()));
}

/* -----------------------------------------------------------------\
|  Method: on_cbxSites_activated [slot]
|  Begin: 15.09.2011 / 8:35
|  Author: Jo2003
|  Description: sites cbx activated
|
|  Parameters: new index
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_cbxSites_activated(int index)
{
   // something changed ... ?
   if ((index + 1) != genreInfo.iPage)
   {
      QUrl    url;
      QString sType  = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();
      int     iGenre = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();

      url.addQueryItem("type", sType);
      url.addQueryItem("page", QString::number(index + 1));

      if (iGenre != -1)
      {
         url.addQueryItem("genre", QString::number(iGenre));
      }

      Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, QString(url.encodedQuery()));
   }
}

/* -----------------------------------------------------------------\
|  Method: on_btnPrevSite_clicked [slot]
|  Begin: 15.09.2011 / 9:00
|  Author: Jo2003
|  Description: prev. site button pressed
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnPrevSite_clicked()
{
   QUrl    url;
   QString sType  = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();
   int     iGenre = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();
   int     iPage  = ui->cbxSites->currentIndex() + 1;

   url.addQueryItem("type", sType);
   url.addQueryItem("page", QString::number(iPage - 1));

   if (iGenre != -1)
   {
      url.addQueryItem("genre", QString::number(iGenre));
   }

   Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, QString(url.encodedQuery()));
}

/* -----------------------------------------------------------------\
|  Method: on_btnNextSite_clicked [slot]
|  Begin: 15.09.2011 / 9:00
|  Author: Jo2003
|  Description: next site button pressed
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnNextSite_clicked()
{
   QUrl    url;
   QString sType  = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();
   int     iGenre = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();
   int     iPage  = ui->cbxSites->currentIndex() + 1;

   url.addQueryItem("type", sType);
   url.addQueryItem("page", QString::number(iPage + 1));

   if (iGenre != -1)
   {
      url.addQueryItem("genre", QString::number(iGenre));
   }

   Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, QString(url.encodedQuery()));
}

/* -----------------------------------------------------------------\
|  Method: on_pushLive_clicked [slot]
|  Begin: 23.09.2011
|  Author: Jo2003
|  Description: live button pressed
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushLive_clicked()
{
   int cid = getCurrentCid();
   if  (chanMap.contains(cid))
   {
      // set EPG offset to 0 ...
      iEpgOffset = 0;
      Trigger.TriggerRequest(Kartina::REQ_EPG, cid, iEpgOffset);

      // fake play button press ...
      if (AllowAction(IncPlay::PS_PLAY))
      {
         int cid  = getCurrentCid();

         if (chanMap.contains(cid))
         {
            if (AllowAction(IncPlay::PS_PLAY))
            {
               cparser::SChan chan = chanMap[cid];

               if (grantAdultAccess(chan.bIsProtected))
               {
                  showInfo.cleanShowInfo();
                  showInfo.setChanId(cid);
                  showInfo.setChanName(chan.sName);
                  showInfo.setShowType(ShowInfo::Live);
                  showInfo.setShowName(chan.sProgramm);
                  showInfo.setStartTime(chan.uiStart);
                  showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
                  showInfo.setEndTime(chan.uiEnd);
                  showInfo.setPCode(secCodeDlg.passWd());
                  showInfo.setPlayState(IncPlay::PS_PLAY);
                  showInfo.setHtmlDescr((QString(TMPL_BACKCOLOR)
                                         .arg("rgb(255, 254, 212)")
                                         .arg(CShowInfo::createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd))));

                  TouchPlayCtrlBtns(false);
                  Trigger.TriggerRequest(Kartina::REQ_STREAM, cid, secCodeDlg.passWd());
               }
            }
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: on_channelList_clicked [slot]
|  Begin: 23.09.2011
|  Author: Jo2003
|  Description: on channel list was clicked once
|
|  Parameters: clicked model index
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_channelList_clicked(QModelIndex index)
{
   if (!Settings.doubleClickToPlay())
   {
      int cid = qvariant_cast<int>(index.data(channellist::cidRole));

      if (chanMap.contains(cid))
      {
         if (AllowAction(IncPlay::PS_PLAY))
         {
            cparser::SChan chan = chanMap[cid];

            if (grantAdultAccess(chan.bIsProtected))
            {
               showInfo.cleanShowInfo();
               showInfo.setChanId(cid);
               showInfo.setChanName(chan.sName);
               showInfo.setShowType(ShowInfo::Live);
               showInfo.setShowName(chan.sProgramm);
               showInfo.setStartTime(chan.uiStart);
               showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
               showInfo.setEndTime(chan.uiEnd);
               showInfo.setPCode(secCodeDlg.passWd());
               showInfo.setPlayState(IncPlay::PS_PLAY);
               showInfo.setHtmlDescr((QString(TMPL_BACKCOLOR)
                                      .arg("rgb(255, 254, 212)")
                                      .arg(CShowInfo::createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd))));

               TouchPlayCtrlBtns(false);
               Trigger.TriggerRequest(Kartina::REQ_STREAM, cid, secCodeDlg.passWd());
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
//                                Slots                                       //
////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------\
|  Method: show [slot]
|  Begin: 19.01.2010 / 16:04:40
|  Author: Jo2003
|  Description: overwride show slot to init some more values ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::show()
{
   if (!(ulStartFlags & FLAG_INITDIALOG))
   {
      ulStartFlags |= FLAG_INITDIALOG;
      initDialog ();
   }

   QWidget::show();
}

/* -----------------------------------------------------------------\
|  Method: slotKartinaResponse [slot]
|  Begin: 29.05.2012
|  Author: Jo2003
|  Description: A central point to catch all http responses
|               from kartina client.
|               Please note: There is no real need for this
|               function because signals / slots can be connected
|               directly. The main goal of this function is to have
|               a central point to find out which function is called
|               when a certain response comes in.
|
|  Parameters: resp: response string
|              req: request type as defined in Kartina workspace
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotKartinaResponse(const QString& resp, int req)
{
   // helper macro to have a nice info printout ...
#define mkCase(__x__, __y__) \
      case __x__: \
         mInfo(tr("\n  --> HTTP Response '%1', calling '%2'").arg(#__x__).arg(#__y__)); \
         __y__; \
         break

   switch ((Kartina::EReq)req)
   {
   ///////////////////////////////////////////////
   // This function also grabs all settings
   // from response. After that channel list
   // will be requested.
   mkCase(Kartina::REQ_COOKIE, slotCookie(resp));

   ///////////////////////////////////////////////
   // Fills channel list as well as channel map.
   // Due to changing actual channel entry
   // slotCurrentChannelChanged() will be called
   // which requests the EPG ...
   mkCase(Kartina::REQ_CHANNELLIST, slotChanList(resp));

   ///////////////////////////////////////////////
   // Fills EPG browser and triggers the load
   // of VOD genres (if there in account info).
   mkCase(Kartina::REQ_EPG, slotEPG(resp));

   ///////////////////////////////////////////////
   // Indicates that a new timeshift value was set.
   // Triggers reload of channel list.
   mkCase(Kartina::REQ_TIMESHIFT, slotTimeShift(resp));

   ///////////////////////////////////////////////
   // Got Stream URL, start play or record
   mkCase(Kartina::REQ_STREAM, slotStreamURL(resp));

   ///////////////////////////////////////////////
   // Got requested stream url for timer record
   mkCase(Kartina::REQ_TIMERREC, timeRec.slotTimerStreamUrl(resp));

   ///////////////////////////////////////////////
   // got requested archiv url
   mkCase(Kartina::REQ_ARCHIV, slotArchivURL(resp));

   ///////////////////////////////////////////////
   // logout done
   mkCase(Kartina::REQ_LOGOUT, slotLogout(resp));

   ///////////////////////////////////////////////
   // got requested VOD genres
   mkCase(Kartina::REQ_GETVODGENRES, slotGotVodGenres(resp));

   ///////////////////////////////////////////////
   // got requested videos
   mkCase(Kartina::REQ_GETVIDEOS, slotGotVideos(resp));

   ///////////////////////////////////////////////
   // got requested video details
   mkCase(Kartina::REQ_GETVIDEOINFO, slotGotVideoInfo(resp));

   ///////////////////////////////////////////////
   // got requested vod url
   mkCase(Kartina::REQ_GETVODURL, slotVodURL(resp));

   ///////////////////////////////////////////////
   // got complete channel list
   // (used in settings dialog)
   mkCase(Kartina::REQ_CHANLIST_ALL, Settings.slotBuildChanManager(resp));

   ///////////////////////////////////////////////
   // got requested VOD management data
   // (used in settings dialog)
   mkCase(Kartina::REQ_GET_VOD_MANAGER, Settings.slotBuildVodManager(resp));

   ///////////////////////////////////////////////
   // handle vod favourites like vod genre to display
   // all videos in favourites
   mkCase(Kartina::REQ_GET_VOD_FAV, slotGotVideos(resp, true));

   ///////////////////////////////////////////////
   // response after trying to change parent code
   mkCase(Kartina::REQ_SET_PCODE, slotPCodeChangeResp(resp));

   ///////////////////////////////////////////////
   // Make sure the unused responses are listed
   // This makes it easier to understand the log.
   mkCase(Kartina::REQ_ADD_VOD_FAV, slotUnused(resp));
   mkCase(Kartina::REQ_REM_VOD_FAV, slotUnused(resp));
   mkCase(Kartina::REQ_SET_VOD_MANAGER, slotUnused(resp));
   mkCase(Kartina::REQ_SETCHAN_SHOW, slotUnused(resp));
   mkCase(Kartina::REQ_SETCHAN_HIDE, slotUnused(resp));
   mkCase(Kartina::REQ_SETBITRATE, slotUnused(resp));
   mkCase(Kartina::REQ_GETBITRATE, slotUnused(resp));
   mkCase(Kartina::REQ_GETTIMESHIFT, slotUnused(resp));
   mkCase(Kartina::REQ_GET_SERVER, slotUnused(resp));
   mkCase(Kartina::REQ_SERVER, slotUnused(resp));
   mkCase(Kartina::REQ_HTTPBUFF, slotUnused(resp));
   default:
      break;
   }
#undef mkCase
}

/* -----------------------------------------------------------------\
|  Method: slotUnused
|  Begin: 29.05.2012
|  Author: Jo2003
|  Description: an unused response was received
|
|  Parameters: unused response
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotUnused(const QString &str)
{
   Q_UNUSED(str)
}

/* -----------------------------------------------------------------\
|  Method: slotSystrayActivated
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: systray icon clicked, restore app window ...
|
|  Parameters: what kind of click ...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotSystrayActivated(QSystemTrayIcon::ActivationReason reason)
{
   switch (reason)
   {
   case QSystemTrayIcon::MiddleClick:
   case QSystemTrayIcon::DoubleClick:
   case QSystemTrayIcon::Trigger:
   case QSystemTrayIcon::Context:
      if (isHidden())
      {
         setGeometry(sizePos);
         QTimer::singleShot(300, this, SLOT(showNormal()));
      }
      break;
   default:
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: slotKartinaErr
|  Begin: 19.01.2010 / 16:08:51
|  Author: Jo2003
|  Description: display errors signaled by other threads
|
|  Parameters: error string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotKartinaErr (const QString &str, int req, int err)
{
   // special error handling for special requests ...
   switch ((Kartina::EReq)req)
   {
   case Kartina::REQ_SET_PCODE:
      Settings.slotEnablePCodeForm();
      break;
   default:
      break;
   }

   // special error handling for special errors ...
   switch ((Kartina::EErr)err)
   {
   case Kartina::ERR_WRONG_PCODE:
      showInfo.setPCode("");
      secCodeDlg.slotClearPasswd();
      break;
   default:
      break;
   }

   mErr(tr("Error %1 (%2) in request '%3'")
        .arg(err)
        .arg(metaKartina.errValToKey((Kartina::EErr)err))
        .arg(metaKartina.reqValToKey((Kartina::EReq)req)));

   QMessageBox::critical(this, tr("Error"), tr("%1 Client API Error:\n%2 (#%3)")
                         .arg(COMPANY_NAME)
                         .arg(str)
                         .arg(err));
   TouchPlayCtrlBtns();
}

/* -----------------------------------------------------------------\
|  Method: slotLogout
|  Begin: 19.01.2010 / 16:08:51
|  Author: Jo2003
|  Description: display errors signaled by other threads
|
|  Parameters: error string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotLogout(const QString &str)
{
   // no need to look for errors in response ...
   Q_UNUSED(str);

   mInfo(tr("logout done ..."));
   QDialog::accept ();
}

/* -----------------------------------------------------------------\
|  Method: slotStreamURL
|  Begin: 19.01.2010 / 16:09:54
|  Author: Jo2003
|  Description: handle requested stream url
|
|  Parameters: stream url (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotStreamURL(const QString &str)
{
   QString sChan, sShow, sUrl, sTime;

   if (!XMLParser.parseUrl(str, sUrl))
   {
      sShow = CleanShowName (showInfo.showName());
      sChan = showInfo.chanName();

      sTime = sTime = QString("%1 - %2")
                      .arg(QDateTime::fromTime_t(showInfo.starts()).toString("hh:mm"))
                      .arg(QDateTime::fromTime_t(showInfo.ends()).toString("hh:mm"));

      if (sShow == "")
      {
         sShow = sChan;
      }

      // add additional info to LCD ...
      ui->labState->setHeader(sChan);
      ui->labState->setFooter(sTime);

      if (ePlayState == IncPlay::PS_RECORD)
      {
         if (!vlcCtrl.ownDwnld())
         {
            StartVlcRec(sUrl, sShow);
         }
         else
         {
            StartStreamDownload(sUrl, sShow);
         }
      }
      else if (ePlayState == IncPlay::PS_PLAY)
      {
         StartVlcPlay(sUrl);
      }
   }

   TouchPlayCtrlBtns();
}

/* -----------------------------------------------------------------\
|  Method: slotCookie
|  Begin: 19.01.2010 / 16:10:23
|  Author: Jo2003
|  Description: authentication done, request channel list
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotCookie (const QString &str)
{
   QString sCookie;

   // parse cookie ...
   if (!XMLParser.parseCookie(str, sCookie, accountInfo))
   {
      KartinaTv.SetCookie(sCookie);

      // decide if we should enable / disable VOD stuff ...
      if (accountInfo.bHasVOD)
      {
         if (!ui->tabEpgVod->widget(vodTabWidget.iPos))
         {
            // make sure tab text is translated as needed
            QString title = (pTranslator + Translators::TRANS_OWN)->translate(objectName().toUtf8().constData(),
                                                   vodTabWidget.sText.toUtf8().constData());

            // add tab ...
            ui->tabEpgVod->addTab(vodTabWidget.pWidget, (title != "") ? title : vodTabWidget.sText);
            ui->tabEpgVod->adjustSize();
         }
      }
      else
      {
         if (ui->tabEpgVod->widget(vodTabWidget.iPos))
         {
            // make sure the widget we want to remove
            // is not the active one ...
            ui->tabEpgVod->setCurrentIndex(0);
            ui->tabEpgVod->removeTab(vodTabWidget.iPos);
         }
      }

      // ------------------------------------------------
      // parse settings (new 11.05.2012)
      // ------------------------------------------------

      // timeshift
      QVector<int> values;
      int          actVal = -1;
      if (!XMLParser.parseSetting(str, "timeshift", values, actVal))
      {
         Settings.fillTimeShiftCbx(values, actVal);

         // set timeshift ...
         ui->textEpg->SetTimeShift(actVal);
         timeRec.SetTimeShift(actVal);
         mInfo(tr("Using following timeshift: %1").arg(actVal));
      }

      // bitrate
      values.clear();
      actVal = -1;
      if (!XMLParser.parseSetting(str, "bitrate", values, actVal))
      {
         Settings.SetBitrateCbx(values, actVal);
         mInfo (tr("Using Bitrate %1 kbit/s ...").arg(actVal));
      }

      // stream server
      QVector<cparser::SSrv> vSrv;
      QString sActIp;
      if (!XMLParser.parseSServersLogin(str, vSrv, sActIp))
      {
         Settings.SetStreamServerCbx(vSrv, sActIp);
         mInfo(tr("Active stream server is %1").arg(sActIp));
      }

      // request channel list ...
      Trigger.TriggerRequest(Kartina::REQ_CHANNELLIST);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotTimeShift
|  Begin: 19.01.2010 / 16:11:30
|  Author: Jo2003
|  Description: time shift set, reload channel list
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotTimeShift (const QString &str)
{
   Q_UNUSED(str)
   Trigger.TriggerRequest(Kartina::REQ_CHANNELLIST);
}

/* -----------------------------------------------------------------\
|  Method: slotChanList
|  Begin: 19.01.2010 / 16:09:23
|  Author: Jo2003
|  Description: handle requested channel list
|
|  Parameters: channel list (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotChanList (const QString &str)
{
   QVector<cparser::SChan> chanList;

   if (!XMLParser.parseChannelList(str, chanList, Settings.FixTime()))
   {
      FillChanMap(chanList);
      FillChannelList(chanList);

      // set channel list in timeRec class ...
      timeRec.SetChanList(chanList);
      timeRec.StartTimer();
   }

   // create favourite buttons if needed ...
   if ((lFavourites.count() > 0) && (ui->gLayoutFav->count() == 0))
   {
      HandleFavourites();
   }

   TouchPlayCtrlBtns();
}

/* -----------------------------------------------------------------\
|  Method: slotEPG
|  Begin: 19.01.2010 / 16:10:49
|  Author: Jo2003
|  Description: handle requested epg info
|
|  Parameters: epg (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotEPG(const QString &str)
{
   QVector<cparser::SEpg> epg;

   QDateTime   epgTime = QDateTime::currentDateTime().addDays(iEpgOffset);
   QModelIndex idx     = ui->channelList->currentIndex();
   int         cid     = qvariant_cast<int>(idx.data(channellist::cidRole));
   QIcon       icon;

   if (!XMLParser.parseEpg(str, epg))
   {
      ui->textEpg->DisplayEpg(epg, chanMap.value(cid).sName,
                              cid, epgTime.toTime_t(),
                              accountInfo.bHasArchive ? chanMap.value(cid).bHasArchive : false);

      // fill epg control ...
      icon = qvariant_cast<QIcon>(idx.data(channellist::iconRole));
      ui->labChanIcon->setPixmap(icon.pixmap(24, 24));
      ui->labChanName->setText(chanMap.value(cid).sName);
      ui->labCurrDay->setText(epgTime.toString("dd. MMM. yyyy"));

      pEpgNavbar->setCurrentIndex(epgTime.date().dayOfWeek() - 1);

      TouchPlayCtrlBtns();
      ui->channelList->setFocus(Qt::OtherFocusReason);

      // update vod stuff only at startup ...
      if (accountInfo.bHasVOD)
      {
         if (ui->cbxGenre->count() == 0)
         {
            Trigger.TriggerRequest(Kartina::REQ_GETVODGENRES);
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotEpgAnchor
|  Begin: 19.01.2010 / 16:16:17
|  Author: Jo2003
|  Description: link in epg browser was clicked,
|               parse and handle request
|  Parameters: clicked link
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotEpgAnchor (const QUrl &link)
{
   // create request string ...
   QString action = link.encodedQueryItemValue(QByteArray("action"));
   bool    ok     = false;

   if (action == "archivrec")
   {
      if (AllowAction(IncPlay::PS_RECORD))
      {
         ok = true;
      }
   }
   else if (action == "archivplay")
   {
      if (AllowAction(IncPlay::PS_PLAY))
      {
         ok = true;
      }
   }
   else if(action == "timerrec")
   {
      uint uiStart = link.encodedQueryItemValue(QByteArray("start")).toUInt();
      uint uiEnd   = link.encodedQueryItemValue(QByteArray("end")).toUInt();
      int  iChan   = link.encodedQueryItemValue(QByteArray("cid")).toInt();

      timeRec.SetRecInfo(uiStart, uiEnd, iChan, CleanShowName(ui->textEpg->epgShow(uiStart).sShowName));
      timeRec.exec();
   }

   if (ok)
   {
      QString cid  = link.encodedQueryItemValue(QByteArray("cid"));

      if (grantAdultAccess(chanMap[cid.toInt()].bIsProtected))
      {
         TouchPlayCtrlBtns(false);

         // get program map ...
         showInfo.setEpgMap(ui->textEpg->exportProgMap());

         // new own downloader ...
         if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
         {
            streamLoader.stopDownload (iDwnReqId);
            iDwnReqId = -1;
         }

         QString    gmt  = link.encodedQueryItemValue(QByteArray("gmt"));
         QString    req  = QString("cid=%1&gmt=%2").arg(cid.toInt()).arg(gmt.toUInt());
         epg::SShow sepg = ui->textEpg->epgShow(gmt.toUInt());

         // store all info about show ...
         showInfo.cleanShowInfo();
         showInfo.setChanId(cid.toInt());
         showInfo.setChanName(chanMap.value(cid.toInt()).sName);
         showInfo.setShowName(sepg.sShowName);
         showInfo.setStartTime(gmt.toUInt());
         showInfo.setEndTime(sepg.uiEnd);
         showInfo.setShowType(ShowInfo::Archive);
         showInfo.setPlayState(ePlayState);
         showInfo.setLastJumpTime(0);
         showInfo.setPCode(secCodeDlg.passWd());

         showInfo.setHtmlDescr((QString(TMPL_BACKCOLOR)
                                .arg("rgb(255, 254, 212)")
                                .arg(CShowInfo::createTooltip(tr("%1 (Archive)").arg(showInfo.chanName()),
                                                   QString("%1 %2").arg(sepg.sShowName).arg(sepg.sShowDescr),
                                                   sepg.uiStart, sepg.uiEnd))));

         // add additional info to LCD ...
         int     iTime = (sepg.uiEnd) ? (int)((sepg.uiEnd - sepg.uiStart) / 60) : 60;
         QString sTime = tr("Length: %1 min.").arg(iTime);
         ui->labState->setHeader(showInfo.chanName() + tr(" (Ar.)"));
         ui->labState->setFooter(sTime);

         Trigger.TriggerRequest(Kartina::REQ_ARCHIV, req, secCodeDlg.passWd());
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotReloadLogos
|  Begin: 19.01.2010 / 16:17:54
|  Author: Jo2003
|  Description: trigger reload of channel logos
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotReloadLogos()
{
   QChanMap::const_iterator cit;

   // create tmp channel list with channels from channelList ...
   for (cit = chanMap.constBegin(); cit != chanMap.constEnd(); cit++)
   {
      if (!(*cit).bIsGroup)
      {
         pixCache.enqueuePic((*cit).sIcon, pFolders->getLogoDir());
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotbtnBack_clicked
|  Begin: 19.01.2010 / 16:18:30
|  Author: Jo2003
|  Description: one week backward in epg
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotbtnBack_clicked()
{
   int cid = getCurrentCid();
   if  (chanMap.contains(cid))
   {
      // set actual day in previous week to munday ...
      int iActDay  = pEpgNavbar->currentIndex();
      int iOffBack = iEpgOffset;
      iEpgOffset  -= 7 + iActDay;

      correctEpgOffset();

      if (iOffBack != iEpgOffset)
      {
         Trigger.TriggerRequest(Kartina::REQ_EPG, cid, iEpgOffset);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotbtnNext_clicked
|  Begin: 19.01.2010 / 16:18:30
|  Author: Jo2003
|  Description: one week forward in epg
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotbtnNext_clicked()
{
   int cid = getCurrentCid();

   if (chanMap.contains(cid))
   {
      // set actual day in next week to munday ...
      int iActDay  = pEpgNavbar->currentIndex();
      int iOffBack = iEpgOffset;

      iEpgOffset  += 7 - iActDay;

      correctEpgOffset();

      if (iOffBack != iEpgOffset)
      {
         Trigger.TriggerRequest(Kartina::REQ_EPG, cid, iEpgOffset);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotArchivURL
|  Begin: 19.01.2010 / 16:19:25
|  Author: Jo2003
|  Description: got requested archiv url, start play / record
|
|  Parameters: archiv url (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotArchivURL(const QString &str)
{
   QString sUrl;

   if (!XMLParser.parseUrl(str, sUrl))
   {
      if (ePlayState == IncPlay::PS_RECORD)
      {
         if (!vlcCtrl.ownDwnld())
         {
            StartVlcRec(sUrl, CleanShowName(showInfo.showName()));
         }
         else
         {
            StartStreamDownload(sUrl, CleanShowName(showInfo.showName()));
         }

         showInfo.setPlayState(IncPlay::PS_RECORD);
      }
      else if (ePlayState == IncPlay::PS_PLAY)
      {
         StartVlcPlay(sUrl);

         showInfo.setPlayState(IncPlay::PS_PLAY);
      }
   }

   TouchPlayCtrlBtns(true);
}

/* -----------------------------------------------------------------\
|  Method: slotDayTabChanged
|  Begin: 19.01.2010 / 16:19:25
|  Author: Jo2003
|  Description: day in epg navi bar changed ...
|
|  Parameters: day index (0 - 6)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotDayTabChanged(int iIdx)
{
   int cid = getCurrentCid();

   if (chanMap.contains(cid))
   {
      QDateTime epgTime  = QDateTime::currentDateTime().addDays(iEpgOffset);
      int       iDay     = epgTime.date().dayOfWeek() - 1;
      int       iOffBack = iEpgOffset;

      // earlier or later ... ?
      if (iIdx < iDay)
      {
         // earlier ...
         iEpgOffset -= iDay - iIdx;
      }
      else if (iIdx > iDay)
      {
         // later ...
         iEpgOffset += iIdx - iDay;
      }

      // get epg for requested day ...
      if (iIdx != iDay)
      {
         correctEpgOffset ();

         if(iOffBack != iEpgOffset)
         {
            Trigger.TriggerRequest(Kartina::REQ_EPG, cid, iEpgOffset);
         }
         else
         {
            // no change -> revert nav button ...
            pEpgNavbar->setCurrentIndex(iDay);
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotSetSServer
|  Begin: 21.01.2010 / 12:19:25
|  Author: Jo2003
|  Description: set stream server request
|
|  Parameters: new server number
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotSetSServer(QString sIp)
{
   Trigger.TriggerRequest(Kartina::REQ_SERVER, sIp);
}

/* -----------------------------------------------------------------\
|  Method: slotSetBitrate
|  Begin: 14.01.2011 / 14:55
|  Author: Jo2003
|  Description: set bitrate
|
|  Parameters: new bitrate
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotSetBitrate(int iRate)
{
   Trigger.TriggerRequest(Kartina::REQ_SETBITRATE, iRate);
}

/* -----------------------------------------------------------------\
|  Method: slotSetTimeShift
|  Begin: 14.09.2011 / 10:00
|  Author: Jo2003
|  Description: set timeshift
|
|  Parameters: timeshift in hours
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotSetTimeShift(int iShift)
{
   TouchPlayCtrlBtns(false);

   // set timeshift ...
   ui->textEpg->SetTimeShift(iShift);
   timeRec.SetTimeShift(iShift);

   Trigger.TriggerRequest(Kartina::REQ_TIMESHIFT, iShift);
}

/* -----------------------------------------------------------------\
|  Method: slotTimerRecordDone
|  Begin: 26.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: recording done, enable dialog items
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotTimerRecordDone()
{
   if (ePlayState == IncPlay::PS_TIMER_RECORD)
   {
      mInfo(tr("timeRec reports: record done!"));
      ePlayState = IncPlay::PS_STOP;
      TouchPlayCtrlBtns();
   }
}

/* -----------------------------------------------------------------\
|  Method: slotTimerRecActive
|  Begin: 26.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: recording starts, disable dialog items
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotTimerRecActive (int iState)
{
   mInfo(tr("timeRec reports: record active!"));
   ePlayState = (IncPlay::ePlayStates)iState;
   TouchPlayCtrlBtns();

   // reset lcd header and footer ...
   ui->labState->setHeader("");
   ui->labState->setFooter("");
}

/* -----------------------------------------------------------------\
|  Method: slotVlcEnds
|  Begin: 02.02.2010 / 13:58:25
|  Author: Jo2003
|  Description: vlc ends, enable dialog items
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotVlcEnds(int iState __UNUSED)
{
   iState = 0; // suppress warnings ...
   if (ePlayState != IncPlay::PS_STOP)
   {
      mInfo(tr("vlcCtrl reports: vlc player ended!"));
      ePlayState = IncPlay::PS_STOP;
   }
   TouchPlayCtrlBtns();
}

/* -----------------------------------------------------------------\
|  Method: slotVlcStarts
|  Begin: 02.02.2010 / 13:58:25
|  Author: Jo2003
|  Description: vlc starts, disable dialog items
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotVlcStarts(int iState)
{
   mInfo(tr("vlcCtrl reports: vlc player active!"));

   if (ePlayState != (IncPlay::ePlayStates)iState)
   {
      ePlayState = (IncPlay::ePlayStates)iState;
   }

   TouchPlayCtrlBtns();
}

/* -----------------------------------------------------------------\
|  Method: slotShutdown
|  Begin: 02.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: close vlc-record when timer record shuts down system
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotShutdown()
{
   QDialog::accept();
}

/* -----------------------------------------------------------------\
|  Method: slotChanListContext
|  Begin: 25.02.2010 / 11:35:12
|  Author: Jo2003
|  Description: channel list right click -> display context menu
|
|  Parameters: clock position
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotChanListContext(const QPoint &pt)
{
   int cid = getCurrentCid();

   if (chanMap.contains(cid)) // real channel ...
   {
      // create context menu ...
      CleanContextMenu();
      pContextAct[0] = new CFavAction (&favContext);
      QString    sLogoFile = QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(cid);

      // is channel already in favourites ... ?
      if (lFavourites.contains(cid))
      {
         // create remove menu ...
         // action.setText(tr("Remove \"%1\" from favourites").arg(pItem->GetName()));
         pContextAct[0]->setText(tr("Remove from favourites"));
         pContextAct[0]->setIcon(QIcon(sLogoFile));
         pContextAct[0]->setFavData(cid, kartinafav::FAV_DEL);
      }
      else
      {
         // create add menu ...
         pContextAct[0]->setText(tr("Add to favourites"));
         pContextAct[0]->setIcon(QIcon(sLogoFile));
         pContextAct[0]->setFavData(cid, kartinafav::FAV_ADD);
      }

      // add action to menu ...
      favContext.addAction(pContextAct[0]);

      // display menu ...
      favContext.exec(ui->channelList->mapToGlobal(pt));
   }
}

/* -----------------------------------------------------------------\
|  Method: slotChgFavourites
|  Begin: 25.02.2010 / 11:35:12
|  Author: Jo2003
|  Description: remove / add favourite
|
|  Parameters: action pointer
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotChgFavourites (QAction *pAct)
{
   CFavAction      *pAction = (CFavAction *)pAct;
   int              iCid    = 0;
   kartinafav::eAct action  = kartinafav::FAV_WHAT;

   // get action details ...
   pAction->favData(iCid, action);

   // what to do ... ?
   if (action == kartinafav::FAV_ADD)
   {
      if (lFavourites.count() < MAX_NO_FAVOURITES)
      {
         // add new favourite ...
         lFavourites.push_back(iCid);

         HandleFavourites();
      }
      else
      {
         QMessageBox::information(this, tr("Note"),
                                  tr("Max. number of favourites (%1) reached.")
                                  .arg(MAX_NO_FAVOURITES));
      }
   }
   else if (action == kartinafav::FAV_DEL)
   {
      // remove favourite ...
      lFavourites.removeOne(iCid);

      HandleFavourites();
   }
}

/* -----------------------------------------------------------------\
|  Method: slotHandleFavAction
|  Begin: 26.02.2010 / 11:35:12
|  Author: Jo2003
|  Description: favourite button pressed, mark in channel list
|
|  Parameters: action pointer
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotHandleFavAction(QAction *pAct)
{
   CFavAction      *pAction = (CFavAction *)pAct;
   int              iCid    = 0;
   kartinafav::eAct act     = kartinafav::FAV_WHAT;

   if (pAction)
   {
      pAction->favData(iCid, act);

      // search in channel list for cannel id ...
      QModelIndex idx;

      // go through channel list ...
      for (int i = 0; i < pModel->rowCount(); i++)
      {
         idx = pModel->index(i, 0);

         // check if this is favourite channel ...
         if (qvariant_cast<int>(idx.data(channellist::cidRole)) == iCid)
         {
            // found --> mark row ...
            ui->channelList->setCurrentIndex(idx);
            ui->channelList->scrollTo(idx, QAbstractItemView::PositionAtTop);
            break;
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotFavBtnContext
|  Begin: 26.02.2010 / 11:35:12
|  Author: Jo2003
|  Description: favourite button context menu requested
|
|  Parameters: position of "click" ;-)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotFavBtnContext(const QPoint &pt)
{
   QString     sLogoFile;

   CleanContextMenu();

   for (int i = 0; i < lFavourites.count(); i++)
   {
      pContextAct[i] = new CFavAction(&favContext);

      if (pContextAct[i])
      {
         sLogoFile = QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(lFavourites[i]);
         pContextAct[i]->setIcon(QIcon(sLogoFile));
         pContextAct[i]->setText(tr("Remove from favourites"));
         pContextAct[i]->setFavData(lFavourites[i], kartinafav::FAV_DEL);
         favContext.addAction(pContextAct[i]);
      }
   }

   // display menu over first button since we have no way
   // to find out on over which button we clicked ...
   favContext.exec(pFavBtn[0]->mapToGlobal(pt));
}

/* -----------------------------------------------------------------\
|  Method: slotSplashScreen
|  Begin: 08.03.2010 / 14:25:12
|  Author: Jo2003
|  Description: pop up splash screen
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotSplashScreen()
{
   CAboutDialog dlg(this, accountInfo.sExpires);
   dlg.ConnectSettings(&Settings);
   dlg.exec();
}

/* -----------------------------------------------------------------\
|  Method: slotIncPlayState
|  Begin: 10.03.2010 / 14:07:12
|  Author: Jo2003
|  Description: state change of internal player
|                  --> change state dispaly
|
|  Parameters: play state
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotIncPlayState(int iState)
{
   switch((IncPlay::ePlayStates)iState)
   {
   case IncPlay::PS_PLAY:
      // might be play, record, timer record -->
      // therefore use internal state ...
      emit sigLCDStateChange ((int)ePlayState);
      break;

   case IncPlay::PS_END:
      // display "stop" in case of "end" ...
      emit sigLCDStateChange ((int)IncPlay::PS_STOP);
      break;

   case IncPlay::PS_ERROR:
      // note about the error also in showInfo class ...
      showInfo.setPlayState((IncPlay::ePlayStates)iState);

      // update play buttons ...
      TouchPlayCtrlBtns(true);

      // fall thru here | |
      //                V V
   default:
      emit sigLCDStateChange (iState);
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: slotDownloadStarted [slot]
|  Begin: 13.12.2010 / 17:02:12
|  Author: Jo2003
|  Description: download of stream has stated
|
|  Parameters: request id, file name...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotDownloadStarted(int id, QString sFileName)
{

   Q_PID     vlcpid   = 0;
   QString   sCmdLine, fileName, sExt;
   QFileInfo info(sFileName);

   iDwnReqId = id;
   fileName  = QString ("%1/%2").arg(info.path()).arg(info.completeBaseName());
   sExt      = info.suffix();

   sCmdLine  = vlcCtrl.CreateClArgs(vlcctrl::VLC_REC_LIVE, Settings.GetVLCPath(),
                                    "", Settings.GetBufferTime(), fileName, sExt);

   // start player if we have a command line ...
   if (sCmdLine != "")
   {
      vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState);
   }

   // successfully started ?
   if (!vlcpid)
   {
      QMessageBox::critical(this, tr("Error!"), tr("Can't start VLC-Media Player!"));
      ePlayState = IncPlay::PS_ERROR;
      TouchPlayCtrlBtns();
   }
   else
   {
      mInfo(tr("Started VLC with pid #%1!").arg((uint)vlcpid));
   }
}

/* -----------------------------------------------------------------\
|  Method: slotGotVodGenres [slot]
|  Begin: 20.12.2010 / 13:30
|  Author: Jo2003
|  Description: vod genres there
|
|  Parameters: answer from kartina.tv
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotGotVodGenres(const QString &str)
{
   QVector<cparser::SGenre> vGenres;
   QVector<cparser::SGenre>::const_iterator cit;
   QString sName;

   // delete content ...
   ui->cbxGenre->clear();

   if (!XMLParser.parseGenres(str, vGenres))
   {
      // fill genres combo box ...
      ui->cbxGenre->addItem(tr("All"), QVariant((int)-1));

      for (cit = vGenres.constBegin(); cit != vGenres.constEnd(); cit ++)
      {
         // make first genre character upper case ...
         sName    = (*cit).sGName;
         sName[0] = sName[0].toUpper();
         ui->cbxGenre->addItem(sName, QVariant((int)(*cit).uiGid));
      }
   }

   ui->cbxGenre->setCurrentIndex(0);

   // trigger video load ...
   QUrl url;
   url.addQueryItem("type", ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString());
   url.addQueryItem("nums", "20");
   Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, QString(url.encodedQuery()));
}

/* -----------------------------------------------------------------\
|  Method: slotGotVideos [slot]
|  Begin: 20.12.2010 / 15:45
|  Author: Jo2003
|  Description: got videos
|
|  Parameters: answer from kartina.tv
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotGotVideos(const QString &str, bool bVodFavs)
{
   QVector<cparser::SVodVideo> vVodList;
   cparser::SGenreInfo gInfo;

   if (!XMLParser.parseVodList(str, vVodList, gInfo))
   {
      QString sGenre = bVodFavs ? ui->cbxLastOrBest->currentText() : ui->cbxGenre->currentText();
      genreInfo      = gInfo;
      touchVodNavBar(gInfo);
      ui->vodBrowser->displayVodList (vVodList, sGenre);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotVodAnchor [slot]
|  Begin: 20.12.2010 / 15:45
|  Author: Jo2003
|  Description: link in vod browser clicked
|
|  Parameters: clicked url
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotVodAnchor(const QUrl &link)
{
   QString action = link.encodedQueryItemValue(QByteArray("action"));
   bool ok        = false;
   int  id        = 0;

   // check password ...
   if (link.encodedQueryItemValue(QByteArray("pass_protect")).toInt())
   {
      // need password ... ?
      if (secCodeDlg.passWd().isEmpty())
      {
         // request password ...
         secCodeDlg.exec();
      }

      // no further error check here, API will tell
      // about a missing password ...
   }

   if (action == "vod_info")
   {
      // buffer last used site (whole code) ...
      lastVodSite.sContent      = ui->vodBrowser->toHtml();
      lastVodSite.iScrollBarVal = ui->vodBrowser->verticalScrollBar()->value();

      id = link.encodedQueryItemValue(QByteArray("vodid")).toInt();

      Trigger.TriggerRequest(Kartina::REQ_GETVIDEOINFO, id, secCodeDlg.passWd());
   }
   else if (action == "backtolist")
   {
      // restore last used site ...
      ui->vodBrowser->setHtml(lastVodSite.sContent);
      ui->vodBrowser->verticalScrollBar()->setValue(lastVodSite.iScrollBarVal);
   }
   else if (action == "play")
   {
      if (AllowAction(IncPlay::PS_PLAY))
      {
         ok = true;
      }
   }
   else if (action == "record")
   {
      if (AllowAction(IncPlay::PS_RECORD))
      {
         ok = true;
      }
   }
   else if (action == "add_fav")
   {
      id = link.encodedQueryItemValue(QByteArray("vodid")).toInt();
      Trigger.TriggerRequest(Kartina::REQ_ADD_VOD_FAV, id, secCodeDlg.passWd());
      Trigger.TriggerRequest(Kartina::REQ_GETVIDEOINFO, id, secCodeDlg.passWd());
   }
   else if (action == "del_fav")
   {
      id = link.encodedQueryItemValue(QByteArray("vodid")).toInt();
      Trigger.TriggerRequest(Kartina::REQ_REM_VOD_FAV, id, secCodeDlg.passWd());
      Trigger.TriggerRequest(Kartina::REQ_GETVIDEOINFO, id, secCodeDlg.passWd());
   }

   if (ok)
   {
      TouchPlayCtrlBtns(false);

      // new own downloader ...
      if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
      {
         streamLoader.stopDownload (iDwnReqId);
         iDwnReqId = -1;
      }

      id = link.encodedQueryItemValue(QByteArray("vid")).toInt();

      showInfo.cleanShowInfo();
      showInfo.setShowName(ui->vodBrowser->getName());
      showInfo.setShowType(ShowInfo::VOD);
      showInfo.setPlayState(ePlayState);
      showInfo.setHtmlDescr(ui->vodBrowser->getShortContent());
      showInfo.setVodId(id);

      ui->labState->setHeader(tr("Video On Demand"));
      ui->labState->setFooter(showInfo.showName());

      Trigger.TriggerRequest(Kartina::REQ_GETVODURL, id, secCodeDlg.passWd());
   }
}

/* -----------------------------------------------------------------\
|  Method: slotGotVideoInfo [slot]
|  Begin: 20.12.2010 / 15:45
|  Author: Jo2003
|  Description: got video info from kartina.tv
|
|  Parameters: video info in xml form ...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotGotVideoInfo(const QString &str)
{
   cparser::SVodVideo vodInfo;
   if (!XMLParser.parseVideoInfo(str, vodInfo))
   {
      ui->vodBrowser->displayVideoDetails(vodInfo);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotVodURL [slot]
|  Begin: 22.12.2010 / 15:30
|  Author: Jo2003
|  Description: got requested vod url, start play / record
|
|  Parameters: vod url (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotVodURL(const QString &str)
{
   QStringList sUrls;

   if (!XMLParser.parseVodUrls(str, sUrls))
   {
      if (sUrls.count() > 1)
      {
         showInfo.setAdUrl(sUrls[1]);
      }

      if (ePlayState == IncPlay::PS_RECORD)
      {
         // use own downloader ... ?
         if (!vlcCtrl.ownDwnld())
         {
            StartVlcRec(sUrls[0], CleanShowName(showInfo.showName()));
         }
         else
         {
            StartStreamDownload(sUrls[0], CleanShowName(showInfo.showName()), "m4v");
         }

         showInfo.setPlayState(IncPlay::PS_RECORD);
      }
      else if (ePlayState == IncPlay::PS_PLAY)
      {
         StartVlcPlay(sUrls[0]);

         showInfo.setPlayState(IncPlay::PS_PLAY);
      }
   }

   TouchPlayCtrlBtns(true);
}

/* -----------------------------------------------------------------\
|  Method: slotChannelUp [slot]
|  Begin: 18.03.2011 / 11:45
|  Author: Jo2003
|  Description: mark previous channel in channel list
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotChannelUp()
{
   QModelIndex idx;
   int         iRow;
   bool        bSuccess = false;
   idx  = ui->channelList->currentIndex();

   do
   {
      iRow = idx.row();

      if (!iRow)
      {
         iRow = pModel->rowCount() - 1;
      }
      else
      {
         iRow --;
      }

      idx = pModel->index(iRow, 0);

      // make sure to not mark a channel group ...
      if (qvariant_cast<int>(idx.data(channellist::cidRole)) != -1)
      {
         bSuccess = true;
      }

   } while (!bSuccess);


   ui->channelList->setCurrentIndex(idx);
   ui->channelList->scrollTo(idx);
}

/* -----------------------------------------------------------------\
|  Method: slotChannelDown [slot]
|  Begin: 18.03.2011 / 11:45
|  Author: Jo2003
|  Description: mark next channel in channel list
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotChannelDown()
{
   QModelIndex idx;
   int         iRow;
   bool        bSuccess = false;
   idx  = ui->channelList->currentIndex();

   do
   {
      iRow = idx.row();

      if (iRow == (pModel->rowCount() - 1))
      {
         iRow = 0;
      }
      else
      {
         iRow ++;
      }

      idx = pModel->index(iRow, 0);

      // make sure to not mark a channel group ...
      if (qvariant_cast<int>(idx.data(channellist::cidRole)) != -1)
      {
         bSuccess = true;
      }

   } while (!bSuccess);

   ui->channelList->setCurrentIndex(idx);
   ui->channelList->scrollTo(idx);
}

/* -----------------------------------------------------------------\
|  Method: slotToggleEpgVod [slot]
|  Begin: 18.03.2011 / 12:00
|  Author: Jo2003
|  Description: switch between EPG and VOD
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotToggleEpgVod()
{
   int iIdx = ui->tabEpgVod->currentIndex();

   iIdx ++;

   if (iIdx > (ui->tabEpgVod->count() - 1))
   {
      iIdx = 0;
   }

   ui->tabEpgVod->setCurrentIndex(iIdx);
}

/* -----------------------------------------------------------------\
|  Method: slotCurrentChannelChanged [slot]
|  Begin: 19.01.2010 / 16:13:56
|  Author: Jo2003
|  Description: channel changed, request epg if needed
|
|  Parameters: slected row index
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotCurrentChannelChanged(const QModelIndex & current)
{
   int cid = qvariant_cast<int>(current.data(channellist::cidRole));

   if (chanMap.contains(cid))
   {
      // get whole channel entry ...
      cparser::SChan entry = chanMap.value(cid);
      int iTs;

      // update short info if we're in live mode
      if ((showInfo.showType() == ShowInfo::Live)
         && (showInfo.playState() == IncPlay::PS_STOP))
      {
         ui->textEpgShort->setHtml(QString(TMPL_BACKCOLOR)
                                   .arg("rgb(255, 254, 212)")
                                   .arg(CShowInfo::createTooltip(entry.sName, entry.sProgramm, entry.uiStart, entry.uiEnd)));
      }

      // quick'n'dirty timeshift hack ...
      if (entry.vTs.count() <= 2) // no timeshift available ...
      {
         ui->textEpg->SetTimeShift(0);
      }
      else
      {
         iTs = Settings.getTimeShift();

         if (ui->textEpg->GetTimeShift() != iTs)
         {
            ui->textEpg->SetTimeShift(iTs);
         }
      }

      // was this a refresh or was channel changed ... ?
      if (cid != ui->textEpg->GetCid())
      {
         // load epg ...
         Trigger.TriggerRequest(Kartina::REQ_EPG, cid, iEpgOffset);
      }
      else // same channel ...
      {
         // refresh epg only, if we view current day in epg ...
         if (iEpgOffset == 0) // 0 means today!
         {
            Trigger.TriggerRequest(Kartina::REQ_EPG, cid);
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotPlayNextChannel [slot]
|  Begin: 23.03.2011 / 13:00
|  Author: Jo2003
|  Description: chosse and play next channel
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotPlayNextChannel()
{
   slotChannelDown();
   on_pushPlay_clicked();
}

/* -----------------------------------------------------------------\
|  Method: slotPlayPreviousChannel [slot]
|  Begin: 23.03.2011 / 13:00
|  Author: Jo2003
|  Description: chosse and play previous channel
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotPlayPreviousChannel()
{
   slotChannelUp();
   on_pushPlay_clicked();
}

/* -----------------------------------------------------------------\
|  Method: slotStartConnectionChain [slot]
|  Begin: 23.09.2011
|  Author: Jo2003
|  Description: start the whole connection stuff
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotStartConnectionChain()
{
   Trigger.TriggerRequest(Kartina::REQ_COOKIE);
}

/* -----------------------------------------------------------------\
|  Method: slotUpdateAnswer [slot]
|  Begin: 12.10.2011
|  Author: Jo2003
|  Description: got update answer
|
|  Parameters: pointer to reply
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotUpdateAnswer (QNetworkReply* pRes)
{
   if (pRes->error() == QNetworkReply::NoError)
   {
      // got update info ...
      QByteArray        ba = pRes->readAll();
      cparser::SUpdInfo updInfo;

      if (!XMLParser.parseUpdInfo(QString(ba), updInfo))
      {
         // compare version ...
         if ((updInfo.iMinor > atoi(VERSION_MINOR))
            && (updInfo.iMajor == atoi(VERSION_MAJOR))
            && (updInfo.sUrl != ""))
         {
            QString s       = HTML_SITE;
            QString content = tr("There is the new version %1 of %2 available.<br />Click %3 to download!")
                  .arg(updInfo.sVersion)
                  .arg(APP_NAME)
                  .arg(QString("<a href='%1'>%2</a>").arg(updInfo.sUrl).arg(tr("here")));

            s.replace(TMPL_CONT, content);

            QMessageBox::information(this, tr("Update available"), s);
         }
      }
   }
   else
   {
      // only tell in log about the error ...
      mInfo(pRes->errorString());
   }

   // schedule deletion ...
   pRes->deleteLater();
}

/* -----------------------------------------------------------------\
|  Method: slotCheckArchProg [slot]
|  Begin: 03.11.2011
|  Author: Jo2003
|  Description: check if short info below play is actual...
|
|  Parameters: archive timestamp
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotCheckArchProg(ulong ulArcGmt)
{
   // is actual showinfo still actual ?
   if (!CSmallHelpers::inBetween(showInfo.starts(), showInfo.ends(), (uint)ulArcGmt))
   {
      // search in archiv program map for matching entry ...
      if (!showInfo.autoUpdate(ulArcGmt))
      {
         // add additional info to LCD ...
         int     iTime = showInfo.ends() ? (int)((showInfo.ends() - showInfo.starts()) / 60) : 60;
         QString sTime = tr("Length: %1 min.").arg(iTime);
         ui->labState->setFooter(sTime);
         ui->labState->updateState(showInfo.playState());

         // set short epg info ...
         ui->textEpgShort->setHtml(showInfo.htmlDescr());

         // done ...
         emit sigShowInfoUpdated();
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotRefreshChanLogos [slot]
|  Begin: 31.05.2012
|  Author: Jo2003
|  Description: update channel logos in channel list ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotRefreshChanLogos()
{
   if (!(ulStartFlags & FLAG_CLOGO_COMPL))
   {
      QStandardItem      *pItem;
      QPixmap             icon;
      int                 cid, curCid, i;
      QString             fLogo;

      // get current selection ...
      curCid = pModel->itemFromIndex(ui->channelList->currentIndex())->data(channellist::cidRole).toInt();

      for (i = 0; i < pModel->rowCount(); i++)
      {
         pItem = pModel->item(i);
         cid   = pItem->data(channellist::cidRole).toInt();
         fLogo = QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(cid);

         if (icon.load(fLogo, "image/gif"))
         {
            pItem->setData(QIcon(icon), channellist::iconRole);

            // update channel icon on EPG browser ...
            if (cid == curCid)
            {
               ui->labChanIcon->setPixmap(QIcon(icon).pixmap(24, 24));
            }
         }
      }

      // mark logo stuff as completed ...
      ulStartFlags |= FLAG_CLOGO_COMPL;
   }
}

/* -----------------------------------------------------------------\
|  Method: slotRefreshChanLogos [slot]
|  Begin: 31.05.2012
|  Author: Jo2003
|  Description: update channel logos in channel list ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotPCodeChangeResp(const QString &str)
{
   Q_UNUSED(str)

   // clear buffered password ...
   secCodeDlg.setPasswd("");
   showInfo.setPCode("");

   Settings.slotNewPCodeSet();
}

#ifdef INCLUDE_LIBVLC
/* -----------------------------------------------------------------\
|  Method: slotToogleFullscreen [slot]
|  Begin: 27.04.2012
|  Author: Jo2003
|  Description: toggle fullscreen
|               (hopefully also Mac Snow Leopard likes this)
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotToogleFullscreen()
{
   if (!pVideoWidget)
   {
      // get videoWidget ...
      pVideoWidget = ui->player->getAndRemoveVideoWidget();

      // add videoWidget to stacked layout ...
      stackedLayout->addWidget(pVideoWidget);

      // make sure videoWidget is the one we see ...
      stackedLayout->setCurrentWidget(pVideoWidget);

      // make dialog fullscreen ...
      showFullScreen();

      // a possible bug fix -> make sure all is visible ...
      pVideoWidget->show();
      pVideoWidget->raise();
      pVideoWidget->raiseRender();
      emit sigFullScreenToggled(1);
   }
   else
   {
      // remove videoWidget from stacked layout ...
      stackedLayout->removeWidget(pVideoWidget);

      // make sure main dialog is visible ...
      stackedLayout->setCurrentWidget(ui->masterFrame);

      // put videoWidget back into player widget ...
      ui->player->addAndEmbedVideoWidget();

      // reset videoWidgets local pointer ...
      pVideoWidget = NULL;

      // show normal ...
      // show();
      showNormal();
      emit sigFullScreenToggled(0);
   }
}
#endif // INCLUDE_LIBVLC

////////////////////////////////////////////////////////////////////////////////
//                             normal functions                               //
////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------\
|  Method: fillShortCutTab
|  Begin: 25.03.2010 / 10:30
|  Author: Jo2003
|  Description: fill shortcut table with shortcut descriptos ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::fillShortCutTab()
{
   vShortCutTab.clear();
   Ui::SShortCuts *pShort;
   Ui::SShortCuts tmpTab[] = {
      // ShortCut Source Table:
      // All shortcuts used in this program will be generated from this
      // table. If you need further shortcuts, add them here.
      // Note: Make sure every object has an object name!
      // ---------------------------------------------------------------
      //  Description
      //  |                        Object Pointer (which owns slot function)
      //  |                        |           Slot function
      //  |                        |           |                                  default Shortcut
      //  |                        |           |                                  |
      //  V                        V           V                                  V
      {tr("Play / Pause"),         this,       SLOT(on_pushPlay_clicked()),       "ALT+P"},
      {tr("Stop"),                 this,       SLOT(on_pushStop_clicked()),       "ALT+S"},
      {tr("Record"),               this,       SLOT(on_pushRecord_clicked()),     "ALT+R"},
      {tr("Timer Record"),         this,       SLOT(on_pushTimerRec_clicked()),   "ALT+T"},
      {tr("Settings"),             this,       SLOT(on_pushSettings_clicked()),   "ALT+O"},
      {tr("About"),                this,       SLOT(on_pushAbout_clicked()),      "ALT+I"},
      {tr("Search EPG"),           this,       SLOT(on_btnSearch_clicked()),      "CTRL+F"},
      {tr("Text Size +"),          this,       SLOT(on_btnFontLarger_clicked()),  "ALT++"},
      {tr("Text Size -"),          this,       SLOT(on_btnFontSmaller_clicked()), "ALT+-"},
      {tr("Quit"),                 this,       SLOT(close()),                     "ALT+Q"},

#ifdef INCLUDE_LIBVLC
      {tr("Toggle Aspect Ratio"),  ui->player, SLOT(slotToggleAspectRatio()),     "ALT+A"},
      {tr("Toggle Crop Geometry"), ui->player, SLOT(slotToggleCropGeometry()),    "ALT+C"},
      {tr("Toggle Fullscreen"),    ui->player, SLOT(on_btnFullScreen_clicked()),  "ALT+F"},
      {tr("Volume +"),             ui->player, SLOT(slotMoreLoudly()),            "+"},
      {tr("Volume -"),             ui->player, SLOT(slotMoreQuietly()),           "-"},
      {tr("Toggle Mute"),          ui->player, SLOT(slotMute()),                  "M"},
      {tr("Jump Forward"),         this,       SLOT(on_pushFwd_clicked()),        "CTRL+ALT+F"},
      {tr("Jump Backward"),        this,       SLOT(on_pushBwd_clicked()),        "CTRL+ALT+B"},
#endif // INCLUDE_LIBVLC

      {tr("Next Channel"),         this,       SLOT(slotChannelDown()),           "CTRL+N"},
      {tr("Previous Channel"),     this,       SLOT(slotChannelUp()),             "CTRL+P"},
      {tr("Play Next Channel"),    this,       SLOT(slotPlayNextChannel()),       "CTRL+ALT+N"},
      {tr("Play Prev. Channel"),   this,       SLOT(slotPlayPreviousChannel()),   "CTRL+ALT+P"},
      {tr("Show EPG / VOD"),       this,       SLOT(slotToggleEpgVod()),          "CTRL+E"},
      // add further entries below ...

      // last entry, don't touch ...
      {"",                         NULL,       NULL,                              ""}
   };

   pShort = tmpTab;

   while (pShort->pObj != NULL)
   {
      vShortCutTab.push_back(*pShort);
      pShort ++;
   }
}

/* -----------------------------------------------------------------\
|  Method: initDialog
|  Begin: 31.07.2010 / 15:33:40
|  Author: Jo2003
|  Description: do first dialog init
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::initDialog ()
{
   // -------------------------------------------
   // create epg nav bar ...
   // -------------------------------------------
   TouchEpgNavi (true);

   // -------------------------------------------
   // create systray ...
   // -------------------------------------------
   CreateSystray();

   // set language as read ...
   (pTranslator + Translators::TRANS_OWN)->load(QString("lang_%1").arg(Settings.GetLanguage ()),
                     pFolders->getLangDir());
   (pTranslator + Translators::TRANS_QT)->load(QString("qt_%1").arg(Settings.GetLanguage ()),
                     pFolders->getQtLangDir());

   // get player module ...
   vlcCtrl.LoadPlayerModule(Settings.GetPlayerModule());

   // -------------------------------------------
   // set last windows size / position ...
   // -------------------------------------------
   bool ok = false;
   sizePos = Settings.GetWindowRect(&ok);

   if (ok)
   {
      setGeometry(sizePos);
   }
   else
   {
      // store default size ...
      sizePos = geometry();
   }

   // -------------------------------------------
   // maximize if it was maximized
   // -------------------------------------------
   if (Settings.IsMaximized())
   {
      setWindowState(Qt::WindowMaximized);
   }

   // -------------------------------------------
   // set font size to last used
   // -------------------------------------------
   iFontSzChg = Settings.GetCustFontSize();

   if (iFontSzChg)
   {
      QFont f;
      ui->textEpg->ChangeFontSize(iFontSzChg);
      ui->textEpgShort->ChangeFontSize(iFontSzChg);
      ui->vodBrowser->ChangeFontSize(iFontSzChg);

      f = ui->channelList->font();
      f.setPointSize(f.pointSize() + iFontSzChg);
      ui->channelList->setFont(f);

      f = ui->cbxChannelGroup->font();
      f.setPointSize(f.pointSize() + iFontSzChg);
      ui->cbxChannelGroup->setFont(f);
   }

   // -------------------------------------------
   // set splitter sizes as last used
   // -------------------------------------------
   QList<int> sSplit;
#ifndef INCLUDE_LIBVLC
   sSplit = Settings.GetSplitterSizes("spChanEpg", &ok);
   if (ok)
   {
      ui->vSplitterChanEpg->setSizes(sSplit);
   }

   sSplit = Settings.GetSplitterSizes("spChan", &ok);
   if (ok)
   {
      ui->hSplitterChannels->setSizes(sSplit);
   }
#else /* ifdef INCLUDE_LIBVLC */
   sSplit = Settings.GetSplitterSizes("spVChanEpg", &ok);
   if (ok)
   {
      ui->vSplitterChanEpg->setSizes(sSplit);
   }

   sSplit = Settings.GetSplitterSizes("spVChanEpgPlay", &ok);
   if (ok)
   {
      ui->vSplitterChanEpgPlay->setSizes(sSplit);
   }

   sSplit = Settings.GetSplitterSizes("spHPlay", &ok);
   if (ok)
   {
      ui->hSplitterPlayer ->setSizes(sSplit);
   }
#endif /* INCLUDE_LIBVLC */

   // display splash screen ...
   if (!Settings.DisableSplashScreen())
   {
      QTimer::singleShot(1500, this, SLOT(slotSplashScreen()));
   }

   // init short cuts ...
   fillShortCutTab();
   InitShortCuts ();

   // check for program updates ...
   if (Settings.checkForUpdate())
   {
      pUpdateChecker->get(QNetworkRequest(QUrl(UPD_CHECK_URL)));
   }
}

/* -----------------------------------------------------------------\
|  Method: savePositions
|  Begin: 13.06.2010 / 17:33:40
|  Author: Jo2003
|  Description: save dialog positions to db
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::savePositions()
{
   // -------------------------------------------
   // save gui settings ...
   // -------------------------------------------
   if (windowState() != Qt::WindowMaximized)
   {
      Settings.SaveWindowRect(geometry());
      Settings.SetIsMaximized(false);
   }
   else
   {
      Settings.SetIsMaximized(true);
   }

#ifndef INCLUDE_LIBVLC
   Settings.SaveSplitterSizes("spChanEpg", ui->vSplitterChanEpg->sizes());
   Settings.SaveSplitterSizes("spChan", ui->hSplitterChannels->sizes());
#else  /* ifdef INCLUDE_LIBVLC */
   Settings.SaveSplitterSizes("spVChanEpg", ui->vSplitterChanEpg->sizes());
   Settings.SaveSplitterSizes("spVChanEpgPlay", ui->vSplitterChanEpgPlay->sizes());
   Settings.SaveSplitterSizes("spHPlay", ui->hSplitterPlayer ->sizes());
#endif /* INCLUDE_LIBVLC */

   Settings.SetCustFontSize(iFontSzChg);
   Settings.SaveFavourites(lFavourites);
}

/* -----------------------------------------------------------------\
|  Method: CleanContextMenu
|  Begin: 26.02.2010 / 14:05:00
|  Author: Jo2003
|  Description: clean context menu entries ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::CleanContextMenu()
{
   for (int i = 0; i < MAX_NO_FAVOURITES; i++)
   {
      if (pContextAct[i] != NULL)
      {
         favContext.removeAction(pContextAct[i]);
         delete pContextAct[i];
         pContextAct[i] = NULL;
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: CreateSystray
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: create systray icon
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::CreateSystray()
{
   trayIcon.setIcon(QIcon(":/app/kartina"));
   trayIcon.setToolTip(tr("%1 - Click to activate!").arg(APP_NAME));
}

/* -----------------------------------------------------------------\
|  Method: touchLastOrBestCbx
|  Begin: 23.12.2010 / 10:30
|  Author: Jo2003
|  Description: create search area combo box
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::touchLastOrBestCbx ()
{
   // fill / update search area combo box ...
   if (!ui->cbxLastOrBest->count())
   {
      ui->cbxLastOrBest->addItem(tr("Newest"), "last");
      ui->cbxLastOrBest->addItem(tr("Best"), "best");
      ui->cbxLastOrBest->addItem(tr("My Favourites"), "vodfav");
      ui->cbxLastOrBest->setCurrentIndex(0);
   }
   else
   {
      int idx;

      if ((idx = ui->cbxLastOrBest->findData("last")) > -1)
      {
         ui->cbxLastOrBest->setItemText(idx, tr("Newest"));
      }

      if ((idx = ui->cbxLastOrBest->findData("best")) > -1)
      {
         ui->cbxLastOrBest->setItemText(idx, tr("Best"));
      }

      if ((idx = ui->cbxLastOrBest->findData("vodfav")) > -1)
      {
         ui->cbxLastOrBest->setItemText(idx, tr("My Favourites"));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: touchGenreCbx
|  Begin: 30.05.2012
|  Author: Jo2003
|  Description: update genre cbx
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::touchGenreCbx()
{
   if (ui->cbxGenre->count())
   {
      int idx;

      if ((idx = ui->cbxGenre->findData((int)-1)) > -1)
      {
         ui->cbxGenre->setItemText(idx, tr("All"));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: TouchEpgNavi
|  Begin: 19.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: create / translate EPG navbar
|
|  Parameters: create flag
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::TouchEpgNavi (bool bCreate)
{
   QPushButton *pBtn;

   if (bCreate)
   {
      /*
        Note: We can't create the navbar in Qt Creator or
        Designer because QTabBar isn't supported there.
        Therefore we create it manually here.

        Format navbar:

        /-----------------------------------------------------\
        | <-- |\/\/| /Mon/Tue/Wed/Thu/Fri/Sat/Sun/ |\/\/| --> |
        \-----------------------------------------------------/

      */

      // create back button and set style ...
      pBtn = new QPushButton;
      pBtn->setIcon(QIcon(":png/back"));
      pBtn->setFlat(true);
      pBtn->setAutoDefault(false);
      pBtn->setMaximumHeight(EPG_NAVBAR_HEIGHT);
      pBtn->setMaximumWidth(EPG_NAVBAR_HEIGHT);
      pBtn->setToolTip(tr("1 week backward"));

      // connect signal with slot ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotbtnBack_clicked()));

      // add button to layout ...
      ui->hLayoutEpgNavi->addWidget(pBtn);

      // create tabbar (epg navbar) and set height ...
      pEpgNavbar = new QTabBar;
      pEpgNavbar->setMaximumHeight(EPG_NAVBAR_HEIGHT);

      // set style so tabs look like whole design ...
      pEpgNavbar->setStyleSheet(QString(NAVBAR_STYLE));

      // connect signal with slot ...
      connect (pEpgNavbar, SIGNAL(currentChanged(int)), this, SLOT(slotDayTabChanged(int)));

      // add h spacer ...
      ui->hLayoutEpgNavi->addStretch();

      // add navbar ...
      ui->hLayoutEpgNavi->addWidget(pEpgNavbar);

      // add h spacer ...
      ui->hLayoutEpgNavi->addStretch();

      // create next button and set style ...
      pBtn = new QPushButton;
      pBtn->setIcon(QIcon(":png/next"));
      pBtn->setFlat(true);
      pBtn->setAutoDefault(false);
      pBtn->setMaximumHeight(EPG_NAVBAR_HEIGHT);
      pBtn->setMaximumWidth(EPG_NAVBAR_HEIGHT);
      pBtn->setToolTip(tr("1 week forward"));

      // connect signal with slot ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotbtnNext_clicked()));

      // add button to layout ...
      ui->hLayoutEpgNavi->addWidget(pBtn);

      // create day tabs ...
      pEpgNavbar->addTab(tr("Mon"));
      pEpgNavbar->addTab(tr("Tue"));
      pEpgNavbar->addTab(tr("Wed"));
      pEpgNavbar->addTab(tr("Thu"));
      pEpgNavbar->addTab(tr("Fri"));
      pEpgNavbar->addTab(tr("Sat"));

      // set color for normal week days ...
      for (int i = 0; i < 5; i++)
      {
         pEpgNavbar->setTabTextColor(i, QColor("white"));
      }

      pEpgNavbar->setTabTextColor(5, QColor("#00a"));
      pEpgNavbar->addTab(tr("Sun"));
      pEpgNavbar->setTabTextColor(6, QColor("#800"));

   }
   else
   {
      // no creation, only translation ...
      pEpgNavbar->setTabText(0, tr("Mon"));
      pEpgNavbar->setTabText(1, tr("Tue"));
      pEpgNavbar->setTabText(2, tr("Wed"));
      pEpgNavbar->setTabText(3, tr("Thu"));
      pEpgNavbar->setTabText(4, tr("Fri"));
      pEpgNavbar->setTabText(5, tr("Sat"));
      pEpgNavbar->setTabText(6, tr("Sun"));

      // fill in tooltip for navi buttons ...
      int iIdx;
      // back button ...
      iIdx = 0;
      pBtn = (QPushButton *)ui->hLayoutEpgNavi->itemAt(iIdx)->widget();
      pBtn->setToolTip(tr("1 week backward"));

      // next button ...
      iIdx = ui->hLayoutEpgNavi->count() - 1;
      pBtn = (QPushButton *)ui->hLayoutEpgNavi->itemAt(iIdx)->widget();
      pBtn->setToolTip(tr("1 week forward"));
   }
}

/* -----------------------------------------------------------------\
|  Method: touchVodNavBar
|  Begin: 15.09.2011 / 8:25
|  Author: Jo2003
|  Description: update Vod navbar
|
|  Parameters: ref. to genre info structure
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::touchVodNavBar(const cparser::SGenreInfo &gInfo)
{
   // delete sites ...
   ui->cbxSites->clear();

   // (de-)activate prev button ...
   if (gInfo.iPage == 1)
   {
      ui->btnPrevSite->setDisabled(true);
   }
   else
   {
      ui->btnPrevSite->setEnabled(true);
   }

   int iSites = gInfo.iTotal / VIDEOS_PER_SITE;

   if (gInfo.iTotal % VIDEOS_PER_SITE)
   {
      iSites ++;
   }

   for (int i = 1; i <= iSites; i++)
   {
      ui->cbxSites->addItem(QString::number(i));
   }

   ui->cbxSites->setCurrentIndex(gInfo.iPage - 1);

   if (iSites == gInfo.iPage)
   {
      ui->btnNextSite->setDisabled(true);
   }
   else
   {
      ui->btnNextSite->setEnabled(true);
   }
}

/* -----------------------------------------------------------------\
|  Method: InitShortCuts
|  Begin: 23.03.2010 / 12:05:00
|  Author: Jo2003
|  Description: init short cuts ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::InitShortCuts()
{
   CShortcutEx                             *pShortCut;
   QVector<Ui::SShortCuts>::const_iterator  cit;

   // go through table and create a shortcut for every entry ...
   for (cit = vShortCutTab.constBegin(); cit != vShortCutTab.constEnd(); cit ++)
   {
      Settings.addShortCut((*cit).sDescr, (*cit).pObj->objectName(), (*cit).pSlot, (*cit).sShortCut);

      pShortCut = new CShortcutEx (QKeySequence(Settings.shortCut((*cit).pObj->objectName(), (*cit).pSlot)), this);

      if (pShortCut)
      {
         connect (pShortCut, SIGNAL(activated()), (*cit).pObj, (*cit).pSlot);

         // save shortcut ...
         vShortcutPool.push_back(pShortCut);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: ClearShortCuts
|  Begin: 23.03.2010 / 12:05:00
|  Author: Jo2003
|  Description: clear (free) short cuts
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::ClearShortCuts()
{
   QVector<CShortcutEx *>::iterator it;

   // free all shortcuts ...
   for (it = vShortcutPool.begin(); it != vShortcutPool.end(); it++)
   {
      delete *it;
   }

   vShortcutPool.clear();
}

/* -----------------------------------------------------------------\
|  Method: FillChanMap
|  Begin: 26.02.2010 / 09:20:24
|  Author: Jo2003
|  Description: fill channel map
|
|  Parameters: vector with channel elements
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::FillChanMap(const QVector<cparser::SChan> &chanlist)
{
   QVector<cparser::SChan>::const_iterator cit;

   chanMap.clear();

   // create channel map ...
   for (cit = chanlist.constBegin(); cit != chanlist.constEnd(); cit++)
   {
      if (!(*cit).bIsGroup)
      {
         chanMap.insert((*cit).iId, *cit);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: FillChannelList
|  Begin: 19.01.2010 / 16:05:24
|  Author: Jo2003
|  Description: fill channel list
|
|  Parameters: vector with channel elements
|
|  Returns: 0
\----------------------------------------------------------------- */
int Recorder::FillChannelList (const QVector<cparser::SChan> &chanlist)
{
   QString  sLine;
   QString  sLogoFile;
   QStandardItem *pItem;
   bool      bMissingIcon = false;
   int       iRow, iRowGroup;
   QFileInfo fInfo;
   QPixmap   Pix(16, 16);
   QPixmap   icon;
   int       iChanCount =  0;
   int       iLastChan  = -1;

   iRowGroup = ui->cbxChannelGroup->currentIndex();
   iRow      = ui->channelList->currentIndex().row();
   iRow      = (iRow <= 0) ? 1 : iRow;
   iRowGroup = (iRowGroup < 0) ? 0 : iRowGroup;

   ui->cbxChannelGroup->clear();
   pModel->clear();

   // any channel stored from former session ... ?
   if (!(ulStartFlags & FLAG_CHAN_LIST))
   {
      iLastChan     = Settings.lastChannel() ? Settings.lastChannel() : -1;
      ulStartFlags |= FLAG_CHAN_LIST;
   }

   for (int i = 0; i < chanlist.size(); i++)
   {
      // check if we should display channel in channel list ...
      if (chanlist[i].bIsHidden || (chanlist[i].bIsProtected && !Settings.AllowEros()))
      {
         mInfo(tr("Exclude '%1' from channel list (hidden: %2, protected: %3).")
               .arg(chanlist[i].sName)
               .arg(chanlist[i].bIsHidden)
               .arg(chanlist[i].bIsProtected));
      }
      else
      {
         // create new item ...
         pItem = new QStandardItem;

         // is this a channel group ... ?
         if (chanlist[i].bIsGroup)
         {
            pItem->setData(-1, channellist::cidRole);
            pItem->setData(chanlist[i].sName, channellist::nameRole);
            pItem->setData(chanlist[i].sProgramm, channellist::bgcolorRole);
            pItem->setData(QIcon(":png/group"), channellist::iconRole);

            // add channel group entry ...
            Pix.fill(QColor(chanlist[i].sProgramm));
            ui->cbxChannelGroup->addItem(QIcon(Pix), chanlist[i].sName, QVariant(i));
         }
         else
         {
            fInfo.setFile(chanlist[i].sIcon);
            sLogoFile = QString("%1/%2").arg(pFolders->getLogoDir()).arg(fInfo.fileName());
            sLine     = QString("%1. %2").arg(++ iChanCount).arg(chanlist[i].sName);

            // check if file exists ...
            if (!QFile::exists(sLogoFile))
            {
               // no --> load default image ...
               icon.load(":png/no_logo");

               // enqueue pic to cache ...
               pixCache.enqueuePic(chanlist[i].sIcon, pFolders->getLogoDir());

               // mark for reload ...
               bMissingIcon = true;
            }
            else
            {
               // check if image file can be loaded ...
               if (!icon.load(sLogoFile, "image/gif"))
               {
                  // can't load --> load default image ...
                  icon.load(":png/no_logo");

                  mInfo(tr("Can't load channel image \"%1.gif\" ...").arg(chanlist[i].iId));

                  // delete logo file ...
                  QFile::remove(sLogoFile);

                  // enqueue pic to cache ...
                  pixCache.enqueuePic(chanlist[i].sIcon, pFolders->getLogoDir());

                  // mark for reload ...
                  bMissingIcon = true;
               }
            }

            // last used channel ...
            if (iLastChan != -1)
            {
               if (iLastChan == chanlist[i].iId)
               {
                  // save row with last used channel ...
                  iRow = i;
               }
            }

            pItem->setData(chanlist[i].iId, channellist::cidRole);
            pItem->setData(sLine, channellist::nameRole);
            pItem->setData(QIcon(icon), channellist::iconRole);

            if(Settings.extChanList())
            {
               pItem->setData(chanlist[i].sProgramm, channellist::progRole);
               pItem->setData(chanlist[i].uiStart, channellist::startRole);
               pItem->setData(chanlist[i].uiEnd, channellist::endRole);
            }
            else
            {
               pItem->setToolTip(CShowInfo::createTooltip(chanlist[i].sName, chanlist[i].sProgramm,
                                               chanlist[i].uiStart, chanlist[i].uiEnd));
            }
         }

         pModel->appendRow(pItem);
      } // not hidden ...
   }

   if (!bMissingIcon)
   {
      ulStartFlags |= FLAG_CLOGO_COMPL;
   }

   ui->cbxChannelGroup->setCurrentIndex(iRowGroup);
   ui->channelList->setCurrentIndex(pModel->index(iRow, 0));
   ui->channelList->scrollTo(pModel->index(iRow, 0));

   return 0;
}

/* -----------------------------------------------------------------\
|  Method: StartVlcRec
|  Begin: 19.01.2010 / 16:06:16
|  Author: Jo2003
|  Description: start VLC to record stream
|
|  Parameters: stream url, channel name, archiv flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int Recorder::StartVlcRec (const QString &sURL, const QString &sChannel)
{
   int         iRV      = -1;
   Q_PID       vlcpid   = 0;
   QDateTime   now      = QDateTime::currentDateTime();
   QString     sExt     = "ts", fileName;
   QString     sCmdLine;

   // should we ask for file name ... ?
   if (Settings.AskForRecFile())
   {
      // yes! Create file save dialog ...
      QString   sFilter;
      QString   sTarget  = QString("%1/%2(%3)").arg(Settings.GetTargetDir())
                          .arg(sChannel).arg(now.toString("yyyy-MM-dd__hh-mm"));

      fileName = QFileDialog::getSaveFileName(this, tr("Save Stream as"),
                 sTarget, QString("MPEG 4 Container (*.mp4);;Transport Stream (*.ts);;AVI File (*.avi)"),
                 &sFilter);

      if (fileName != "")
      {
         // which filter was used ... ?
         if (sFilter == "Transport Stream (*.ts)")
         {
            sExt = "ts";
         }
         else if (sFilter ==  "AVI File (*.avi)")
         {
            sExt = "avi";
         }
         else if (sFilter ==  "MPEG 4 Container (*.mp4)")
         {
            sExt = "mp4";
         }

         QFileInfo info(fileName);

         // re-create complete file name ...
         fileName = QString ("%1/%2").arg(info.path())
                    .arg(info.completeBaseName());
      }
   }
   else
   {
      // create filename as we think it's good ...
      fileName = QString("%1/%2(%3)").arg(Settings.GetTargetDir())
                 .arg(sChannel).arg(now.toString("yyyy-MM-dd__hh-mm"));
   }

   if (fileName != "")
   {
      if (showInfo.showType() == ShowInfo::Live)
      {
         // normal stream using HTTP ...
         sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_REC_LIVE,
                                         Settings.GetVLCPath(),
                                         sURL, Settings.GetBufferTime(),
                                         fileName, sExt);
      }
      else
      {
         // archiv using HTTP ...
         sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_REC_ARCH,
                                         Settings.GetVLCPath(),
                                         sURL, Settings.GetBufferTime(),
                                         fileName, sExt);
      }

      // start player if we have a command line ...
      if (sCmdLine != "")
      {
         ui->textEpgShort->setHtml(showInfo.htmlDescr());
         vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState);
      }

      // successfully started ?
      if (!vlcpid)
      {
         iRV = -1;
         QMessageBox::critical(this, tr("Error!"), tr("Can't start VLC-Media Player!"));
         ePlayState = IncPlay::PS_ERROR;
         TouchPlayCtrlBtns();
      }
      else
      {
         iRV = 0;
         mInfo(tr("Started VLC with pid #%1!").arg((uint)vlcpid));
      }
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: StartVlcPlay
|  Begin: 19.01.2010 / 16:06:16
|  Author: Jo2003
|  Description: start VLC to show stream
|
|  Parameters: stream url, archiv flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int Recorder::StartVlcPlay (const QString &sURL)
{
   int         iRV      = 0;
   Q_PID       vlcpid   = 0;
   QString     sCmdLine;

   if (showInfo.showType() == ShowInfo::Live)
   {
      // normal stream using HTTP ...
      sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_PLAY_LIVE,
                                      Settings.GetVLCPath(), sURL,
                                      Settings.GetBufferTime());
   }
   else
   {
      // archiv using HTTP ...
      sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_PLAY_ARCH,
                                      Settings.GetVLCPath(), sURL,
                                      Settings.GetBufferTime());
   }

   // start player if we have a command line ...
   if (sCmdLine != "")
   {
      ui->textEpgShort->setHtml(showInfo.htmlDescr());
      vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState);
   }

   // successfully started ?
   if (!vlcpid)
   {
      iRV = -1;
      QMessageBox::critical(this, tr("Error!"), tr("Can't start VLC-Media Player!"));
      ePlayState = IncPlay::PS_ERROR;
      TouchPlayCtrlBtns();
   }
   else
   {
      mInfo(tr("Started VLC with pid #%1!").arg((uint)vlcpid));
   }
   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: StartStreamDownload
|  Begin: 13.12.2010 / 17:05
|  Author: Jo2003
|  Description: start stream download
|
|  Parameters: stream url, show name
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::StartStreamDownload (const QString &sURL, const QString &sName, const QString &sFileExt)
{
   QString   sExt = sFileExt, fileName;
   QDateTime now  = QDateTime::currentDateTime();

   // should we ask for file name ... ?
   if (Settings.AskForRecFile())
   {
      // yes! Create file save dialog ...
      QString   sFilter;
      QString   sTarget  = QString("%1/%2(%3)").arg(Settings.GetTargetDir())
                          .arg(sName).arg(now.toString("yyyy-MM-dd__hh-mm"));

      fileName = QFileDialog::getSaveFileName(this, tr("Save Stream as"),
                 sTarget, QString("Transport Stream (*.ts);;MPEG 4 Video (*.m4v)"),
                 &sFilter);

      if (fileName != "")
      {
         QFileInfo info(fileName);

         // re-create complete file name ...
         fileName = QString ("%1/%2").arg(info.path())
                    .arg(info.completeBaseName());
      }
   }
   else
   {
      // create filename as we think it's good ...
      fileName = QString("%1/%2(%3)").arg(Settings.GetTargetDir())
                 .arg(sName).arg(now.toString("yyyy-MM-dd__hh-mm"));
   }

   if (fileName != "")
   {
      ui->textEpgShort->setHtml(showInfo.htmlDescr());

      streamLoader.downloadStream (sURL, QString("%1.%2").arg(fileName).arg(sExt),
                                   Settings.GetBufferTime ());
   }
}

/* -----------------------------------------------------------------\
|  Method: TouchPlayCtrlBtns
|  Begin: 19.01.2010 / 16:13:30
|  Author: Jo2003
|  Description: enable / disable buttons
|
|  Parameters: enable flag, archive flag
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::TouchPlayCtrlBtns (bool bEnable)
{
#ifdef INCLUDE_LIBVLC
   if (vlcCtrl.withLibVLC())
   {
      if ((showInfo.playState() == IncPlay::PS_PLAY)
         && showInfo.canCtrlStream()
         && bEnable)
      {
         ui->pushBwd->setEnabled(true);
         ui->pushFwd->setEnabled(true);
         ui->cbxTimeJumpVal->setEnabled(true);
         ui->pushPlay->setIcon(QIcon(":/app/pause"));
      }
      else
      {
         ui->pushBwd->setEnabled(false);
         ui->pushFwd->setEnabled(false);
         ui->cbxTimeJumpVal->setEnabled(false);
         ui->pushPlay->setIcon(QIcon(":/app/play"));
      }
   }
   else
   {
      ui->pushBwd->setEnabled(false);
      ui->pushFwd->setEnabled(false);
      ui->cbxTimeJumpVal->setEnabled(false);
   }
#endif /* INCLUDE_LIBVLC */

   switch (ePlayState)
   {
   case IncPlay::PS_PLAY:
      ui->pushPlay->setEnabled(bEnable);
      ui->pushRecord->setEnabled(bEnable);
      ui->pushStop->setEnabled(bEnable);
      ui->pushLive->setEnabled(bEnable);
      break;

   case IncPlay::PS_RECORD:
      ui->pushPlay->setEnabled(false);
      ui->pushRecord->setEnabled(bEnable);
      ui->pushStop->setEnabled(bEnable);
      ui->pushLive->setEnabled(false);
      break;

   case IncPlay::PS_TIMER_RECORD:
   case IncPlay::PS_TIMER_STBY:
      ui->pushPlay->setEnabled(false);
      ui->pushRecord->setEnabled(false);
      ui->pushLive->setEnabled(false);
      ui->pushStop->setEnabled(bEnable);
      break;

   default:
      ui->pushPlay->setEnabled(bEnable);
      ui->pushRecord->setEnabled(bEnable);
      ui->pushLive->setEnabled(bEnable);
      ui->pushStop->setEnabled(false);
      break;
   }

   emit sigLCDStateChange((int)ePlayState);
}

/* -----------------------------------------------------------------\
|  Method: CleanShowName
|  Begin: 26.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: try to create a nice looking program name
|
|  Parameters: ref. to org. name
|
|  Returns: cleaned name
\----------------------------------------------------------------- */
QString Recorder::CleanShowName(const QString &str)
{
   QString sName = str;

   // remove html code for ' " ' ...
   sName.replace(QString("&quot;"), QString(" "));

   // remove Windows forbidden characters
   // <>:?*|"\/ and in addition '-.
   sName.replace(QRegExp("[<>:?*/|\\\\\"'.,-]"), " ");

   // remove mutliple spaces ...
   sName = sName.simplified();

   // find space at good position ...
   int iIdx = sName.indexOf(QChar(' '), MAX_NAME_LEN - 1);

   if (iIdx <= (MAX_NAME_LEN + 5))
   {
      // found space at needed position ...
      sName = sName.left(iIdx);
   }
   else
   {
      iIdx = sName.indexOf(QChar(' '));

      if ((iIdx > 1) && (iIdx <= (MAX_NAME_LEN + 5)))
      {
         // find cut position shorter than name length ...
         sName = sName.left(iIdx);
      }
      else
      {
         // can't find good cut position ...
         sName = sName.left(MAX_NAME_LEN);
      }
   }

   return sName;
}

/* -----------------------------------------------------------------\
|  Method: WantToStopRec
|  Begin: 02.02.2010 / 10:05:00
|  Author: Jo2003
|  Description: ask if we want to stop recording
|
|  Parameters: --
|
|  Returns: true --> stop it
|          false --> don't stop me now ;-)
\----------------------------------------------------------------- */
bool Recorder::WantToStopRec()
{
   QString sText = HTML_SITE;
   sText.replace(TMPL_CONT, tr("Pending Record!"
                               "<br /> <br />"
                               "Do you really want to stop recording now?"));

   if (QMessageBox::question(this, tr("Question"), sText,
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
   {
      return true;
   }
   else
   {
      return false;
   }
}

/* -----------------------------------------------------------------\
|  Method: HandleFavourites
|  Begin: 26.02.2010 / 11:35:12
|  Author: Jo2003
|  Description: clean create favourite buttons
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::HandleFavourites()
{
   int i;
   QPixmap pic;
   QString sObj;

   // remove all favourite buttons ...
   for (i = 0; i < MAX_NO_FAVOURITES; i++)
   {
      if (pFavBtn[i] != NULL)
      {
         // delete shortcut entry from shortcut table ...
         sObj = QString("pFavAct[%1]").arg(i);
         Settings.delShortCut(sObj, SLOT(slotHandleFavAction(QAction*)));

         ui->gLayoutFav->removeWidget(pFavBtn[i]);
         delete pFavAct[i];
         delete pFavBtn[i];
         pFavBtn[i] = NULL;
         pFavAct[i] = NULL;
      }
   }

   // re-create all buttons ...
   for (i = 0; i < lFavourites.count(); i++)
   {
      pFavBtn[i] = new QToolButton (this);
      pFavAct[i] = new CFavAction (this);

      if (pFavBtn[i] && pFavAct[i])
      {
         // -------------------------
         // init action ...
         // -------------------------

         // add logo ...
         pic.load(QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(lFavourites[i]));
         pFavAct[i]->setIcon(QIcon(pic.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

         // store channel id in action ...
         pFavAct[i]->setFavData(lFavourites[i], kartinafav::FAV_WHAT);

         // add shortcut to shortcut table ...
         sObj = QString("pFavAct[%1]").arg(i);
         Settings.addShortCut(tr("Favourite %1").arg(i + 1), sObj,
                              SLOT(slotHandleFavAction(QAction*)),
                              QString("ALT+%1").arg(i));

         // set shortcut ...
         pFavAct[i]->setShortcut(QKeySequence(Settings.shortCut(sObj, SLOT(slotHandleFavAction(QAction*)))));

         // add channel name as tooltip ...
         pFavAct[i]->setToolTip(chanMap.value(lFavourites[i]).sName);

         // style the tool button ...
         pFavBtn[i]->setStyleSheet(FAVBTN_STYLE);

         // set action ...
         pFavBtn[i]->setDefaultAction(pFavAct[i]);

         // set icon size ...
         pFavBtn[i]->setIconSize(QSize(32, 32));

         // we will use own context menu ...
         pFavBtn[i]->setContextMenuPolicy(Qt::CustomContextMenu);

         // connect button trigger with slot function ...
         connect (pFavBtn[i], SIGNAL(triggered(QAction*)), this, SLOT(slotHandleFavAction(QAction*)));

         connect (pFavBtn[i], SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotFavBtnContext(QPoint)));

         // add button to layout ...
         ui->gLayoutFav->addWidget(pFavBtn[i], i / (MAX_NO_FAVOURITES / 2), i % (MAX_NO_FAVOURITES / 2), Qt::AlignCenter);
      }
      else
      {
         // memory allocation problem ... should never happen ...
         if (pFavBtn[i])
         {
            delete pFavBtn[i];
            pFavBtn[i] = NULL;
         }

         if (pFavAct[i])
         {
            delete pFavAct[i];
            pFavAct[i] = NULL;
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: AllowAction
|  Begin: 05.03.2010 / 10:25:12
|  Author: Jo2003
|  Description: check if action is allowed, ask if needed!
|
|  Parameters: requested new state
|
|  Returns: 1 --> ok, do it
|           0 --> no, don't do it
\----------------------------------------------------------------- */
int Recorder::AllowAction (IncPlay::ePlayStates newState)
{
   int iRV = 0;

   switch (ePlayState)
   {
   case IncPlay::PS_RECORD:
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      {
         // pending record ...
         switch (newState)
         {
            // requested action stop or new record ...
         case IncPlay::PS_STOP:
         case IncPlay::PS_RECORD:
            // ask for permission ...
            if (WantToStopRec ())
            {
               // permission granted ...
               iRV        = 1;

               // set new state ...
               ePlayState = newState;
            }
            break;
         default:
            // all other actions permitted ...
            break;
         }
      }
      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      break;

   case IncPlay::PS_TIMER_RECORD:
   case IncPlay::PS_TIMER_STBY:
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      if (newState == IncPlay::PS_STOP)
      {
         // ask for permission ...
         if (WantToStopRec ())
         {
            // permission granted ...
            iRV        = 1;

            // set new state ...
            ePlayState = newState;
         }
      }
      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      break;

   default:
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // don't ask for every fart.
      // Let the user decide what he wants to do!
      iRV        = 1;

      // set new state ...
      ePlayState = newState;
      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      break;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: TimeJumpAllowed
|  Begin: 18.03.2010 / 15:07:12
|  Author: Jo2003
|  Description: only allow time jump if we don't record
|
|  Parameters:
|
|  Returns: true --> allowed
|          false --> not allowed
\----------------------------------------------------------------- */
bool Recorder::TimeJumpAllowed()
{
   bool bRV = true;

   switch (ePlayState)
   {
   // make sure that any kind of record
   // works without time jump ...
   case IncPlay::PS_RECORD:
   case IncPlay::PS_TIMER_RECORD:
   case IncPlay::PS_TIMER_STBY:
      bRV = false;
   default:
      break;
   }

   return bRV;
}

/* -----------------------------------------------------------------\
|  Method: getCurrentCid
|  Begin: 22.03.2010 / 19:15
|  Author: Jo2003
|  Description: get cid of current channel
|
|  Parameters: --
|
|  Returns: channel id
\----------------------------------------------------------------- */
int Recorder::getCurrentCid()
{
   QModelIndex idx = ui->channelList->currentIndex();
   int         cid = qvariant_cast<int>(idx.data(channellist::cidRole));

   return cid;
}

/* -----------------------------------------------------------------\
|  Method: retranslateShortcutTable
|  Begin: 25.03.2010 / 11:15
|  Author: Jo2003
|  Description: translate shortcut table
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::retranslateShortcutTable()
{
   // re-translate shortcut table if there's something to
   // translate ...
   if (Settings.shortCutCount() > 0)
   {
      QVector<Ui::SShortCuts>::const_iterator cit;

      // update shortcut vector to include
      // new translation ...
      fillShortCutTab();

      // go through table and update shortcut description ...
      for (cit = vShortCutTab.constBegin(); cit != vShortCutTab.constEnd(); cit ++)
      {
         Settings.updateShortcutDescr((*cit).sDescr, (*cit).pObj->objectName(), (*cit).pSlot);
      }

      // add favourites ...
      for (int i = 0; i < MAX_NO_FAVOURITES; i++)
      {
         if (pFavAct[i] != NULL)
         {
            Settings.updateShortcutDescr(tr("Favourite %1").arg(i + 1),
                                 QString("pFavAct[%1]").arg(i),
                                 SLOT(slotHandleFavAction(QAction*)));
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: correctEpgOffset
|  Begin: 23.09.2011
|  Author: Jo2003
|  Description: check / correct epg offset
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::correctEpgOffset()
{
   if (iEpgOffset > 7)
   {
      iEpgOffset = 7;
   }
   else if (iEpgOffset < -14)
   {
      iEpgOffset = -14;
   }
}

/* -----------------------------------------------------------------\
|  Method: grantAdultAccess
|  Begin: 01.06.2012
|  Author: Jo2003
|  Description: check protected channel / password
|
|  Parameters: channel entry
|
|  Returns: 1 --> access granted
|           0 --> no access
\----------------------------------------------------------------- */
int Recorder::grantAdultAccess(bool bProtected)
{
   int iRV = 0;

   if (!bProtected)
   {
      // unprotected channel --> always grant access ...
      iRV = 1;
   }
   else
   {
      // protected channel allowed ... ?
      if (Settings.AllowEros())
      {
         // is parent code stored ... ?
         if (Settings.GetErosPasswd() != "")
         {
            // set external password to sec code dialog ...
            secCodeDlg.setPasswd(Settings.GetErosPasswd());
         }

         // no password set?
         if (secCodeDlg.passWd() == "")
         {
            // request password ...
            secCodeDlg.exec();
         }

         // finaly we only can grant access if
         // we have a password ...
         if (secCodeDlg.passWd() != "")
         {
            iRV = 1;
         }
      }
   }

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
