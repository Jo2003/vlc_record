/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/recorder.cpp $
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 16:01:09
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: recorder.cpp 1495 2015-02-20 13:48:22Z Olenka.Joerg $
\*************************************************************/
#include "recorder.h"
#include "small_helpers.h"

#include "ui_recorder_inc.h"

#include "qfusioncontrol.h"
#include "qcustparser.h"
#include "chtmlwriter.h"
#include "qoverlayicon.h"

// global customization class ...
extern QCustParser *pCustomization;

// fusion control ...
extern QFusionControl missionControl;

// for logging ...
extern CLogFile VlcLog;

// for folders ...
extern CDirStuff *pFolders;

// global showinfo class ...
extern CShowInfo showInfo;

// global rec db ...
extern CVlcRecDB *pDb;

// global client api classes ...
extern ApiClient *pApiClient;
extern ApiParser *pApiParser;

// global translaters ...
extern QTranslator *pAppTransl;
extern QTranslator *pQtTransl;

// global timeshift class ...
extern CTimeShift *pTs;

// global html writer ...
extern CHtmlWriter *pHtml;

// gloabl channel map ...
extern QChannelMap *pChanMap;

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
Recorder::Recorder(QWidget *parent)
    : QDialog(parent, Qt::Window),
    ui(new Ui::Recorder)
{
   ui->setupUi(this);
   QString sHlp;

   // build layout stack ...
   pVideoWidget  = NULL;
   stackedLayout = new QStackedLayout();
   stackedLayout->setMargin(0);
   ui->vMainLayout->removeWidget(ui->masterFrame);
   stackedLayout->addWidget(ui->masterFrame);
   ui->vMainLayout->addLayout(stackedLayout);
   eCurDMode = Ui::DM_NORMAL;
   eOldDMode = Ui::DM_NORMAL;

   // set (customized) windows title ...
   setWindowTitle(pCustomization->strVal("APP_NAME"));

#ifndef _HAS_VOD_LANG
   // hide vod language stuff if needed ...
   ui->cbxVodLang->hide();
   ui->labVodLang->hide();
#endif // _HAS_VOD_LANG

   ePlayState    =  IncPlay::PS_WTF;
   iEpgOffset    =  0;
   iFontSzChg    =  0;
   iDwnReqId     = -1;
   ulStartFlags  =  0;
   pFilterMenu   =  NULL;
   pMnLangFilter =  NULL;
   pWatchList    =  NULL;
   pHlsControl   =  NULL;
   bStayOnTop    =  false;
   mpRetryDlg    =  NULL;
   miMarkFavChan =  -1;
   mFavWidgetWidth = 0;
   miFavsInRow   =  Settings.favsInRow();

   // auto re-login timer ...
   mtAutoReLogin.setSingleShot(true);
   mtAutoReLogin.setInterval(60000);

   // feed mission control ...
   missionControl.addButton(ui->pushPlay,     QFusionControl::BTN_PLAY);
   missionControl.addButton(ui->pushStop,     QFusionControl::BTN_STOP);
   missionControl.addButton(ui->pushRecord,   QFusionControl::BTN_REC);
   missionControl.addButton(ui->pushFwd,      QFusionControl::BTN_FWD);
   missionControl.addButton(ui->pushBwd,      QFusionControl::BTN_BWD);
   missionControl.addButton(ui->pushScrnShot, QFusionControl::BTN_SCRSHOT);
   missionControl.addJumpBox(ui->cbxTimeJumpVal);

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

   // create playlist parser ...
   pHlsControl = new QHlsControl(this);

   // set channel list model and delegate ...
   pModel    = new CFavItemModel(this);
   pDelegate = new QChanListDelegate(this);

   ui->channelList->setItemDelegate(pDelegate);
   ui->channelList->setModel(pModel);

   // menu for channel list filtering ...
   pFilterMenu   = new QMenu(this);
   pFilterWidget = new QStringFilterWidgetAction(this);
   pFilterMenu->addAction(pFilterWidget);

#ifdef _TASTE_IPTV_RECORD
   pMnLangFilter = pFilterMenu->addMenu(tr("Language Filter"));
#endif // _TASTE_IPTV_RECORD

   // watch list ...
   pWatchList = new QWatchListDlg (this);

   // set this dialog as parent for settings and timerRec ...
   Settings.setParent(this, Qt::Dialog);
   secCodeDlg.setParent(this, Qt::Dialog);
   Settings.setAccountInfo(&accountInfo);
   timeRec.setParent(this, Qt::Dialog);
   trayIcon.setParent(this);
   vlcCtrl.setParent(this);
   favContext.setParent(this, Qt::Popup);
   timerWidget.setParent(this, Qt::Tool);
   updNotifier.setParent(this, Qt::Popup);
   expNotifier.setParent(this, Qt::Popup);

   // help dialog class (non modal) ...
   pHelp = new QHelpDialog(NULL);

   // set settings for vod browser ...
   ui->vodBrowser->setSettings(&Settings);

   // settings for pix loader ...
   pixCache.importSettings(&Settings);

   // set pix cache ...
   ui->vodBrowser->setPixCache(&pixCache);

   // set log level ...
   VlcLog.SetLogLevel(Settings.GetLogLevel());

   mLog(tr("Starting: %1 / Version: %2").arg(pCustomization->strVal("APP_NAME")).arg(__MY__VERSION__));
   mLog(tr("Using libVLC 0x%1").arg(ui->player->libvlcVersion(), 8, 16, QChar('0')));

   // log folder locations ...
   mInfo (tr("\ndataDir: %1\n").arg(pFolders->getDataDir())
          + tr("logoDir: %1\n").arg(pFolders->getLogoDir())
          + tr("langDir: %1\n").arg(pFolders->getLangDir())
          + tr("modDir:  %1\n").arg(pFolders->getModDir())
          + tr("appDir:  %1").arg(pFolders->getAppDir()));

   // set help file ...
   // be sure the file we want to load exists ... fallback to english help ...
   sHlp = QString("%1/help_%2.qhc").arg(pFolders->getDocDir()).arg(Settings.GetLanguage());

   if (!QFile::exists(sHlp))
   {
      sHlp = QString("%1/help_en.qhc").arg(pFolders->getDocDir());
   }

   pHelp->setHelpFile(sHlp);

   // set connection data ...
   pApiClient->SetData(Settings.GetAPIServer(), Settings.GetUser(), Settings.GetPasswd(), Settings.GetLanguage());


   // set proxy stuff ...
   if (Settings.UseProxy())
   {
      QNetworkProxy proxy(QNetworkProxy::HttpProxy,
                          Settings.GetProxyHost(), Settings.GetProxyPort(),
                          Settings.GetProxyUser(), Settings.GetProxyPasswd());

      pApiClient->setProxy(proxy);
      streamLoader.setProxy(proxy);
   }

   // give vlcCtrl needed infos ...
   vlcCtrl.SetTranslitPointer(&translit);
   vlcCtrl.SetTranslitSettings(Settings.TranslitRecFile());

   // give timerRec all needed infos ...
   timeRec.SetSettings(&Settings);
   timeRec.SetVlcCtrl(&vlcCtrl);
   timeRec.SetStreamLoader(&streamLoader);
   timeRec.setHlsControl(pHlsControl);

   // hide / remove VOD tab widget ...
   vodTabWidget.iPos    = 1;  // index of VOD tab
   vodTabWidget.icon    = ui->tabEpgVod->tabIcon(vodTabWidget.iPos);
   vodTabWidget.sText   = ui->tabEpgVod->tabText(vodTabWidget.iPos);
   vodTabWidget.pWidget = ui->tabEpgVod->widget(vodTabWidget.iPos);
   ui->tabEpgVod->removeTab(vodTabWidget.iPos);

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

   // connect vlc control with libvlc player ...
   connect (ui->player, SIGNAL(sigPlayState(int)), &vlcCtrl, SLOT(slotLibVlcStateChange(int)));
   connect (&vlcCtrl, SIGNAL(sigLibVlcPlayMedia(QString, QString)), ui->player, SLOT(playMedia(QString, QString)));
   connect (&vlcCtrl, SIGNAL(sigLibVlcStop()), ui->player, SLOT(stop()));

   // get state if libVLC player to change player state display ...
   connect (ui->player, SIGNAL(sigPlayState(int)), this, SLOT(slotIncPlayState(int)));

   // short info update on archive play ...
   connect (ui->player, SIGNAL(sigCheckArchProg(ulong)), this, SLOT(slotCheckArchProg(ulong)));
   connect (this, SIGNAL(sigShowInfoUpdated()), ui->player, SLOT(slotShowInfoUpdated()));

   connect (ui->player, SIGNAL(sigToggleFullscreen()), this, SLOT(slotToggleFullscreen()));
   connect (&missionControl, SIGNAL(sigFullScreen()), this, SLOT(slotToggleFullscreen()));
   connect (this, SIGNAL(sigFullScreenToggled(int)), ui->player, SLOT(slotFsToggled(int)));
   connect (this, SIGNAL(sigWindowed(int)), ui->player, SLOT(slotWindowed(int)));
   connect (ui->player->getVideoWidget(), SIGNAL(sigWindowed()), this, SLOT(slotWindowed()));
   connect (&missionControl, SIGNAL(sigEnterWndwd()), this, SLOT(slotWindowed()));
   connect (ui->player->getVideoWidget(), SIGNAL(sigStayOnTop(bool)), this, SLOT(slotStayOnTop(bool)));

   // connect signals and slots ...
   connect (&missionControl, SIGNAL(sigPlay()), this, SLOT(slotPlay()));
   connect (&missionControl, SIGNAL(sigStop()), this, SLOT(slotStop()));
   connect (&missionControl, SIGNAL(sigRec()), this, SLOT(slotRecord()));
   connect (&missionControl, SIGNAL(sigBwd()), this, SLOT(slotBwd()));
   connect (&missionControl, SIGNAL(sigFwd()), this, SLOT(slotFwd()));
   connect (&missionControl, SIGNAL(sigMute(bool)), ui->player, SLOT(slotMute()));

#ifdef _TASTE_IPTV_RECORD
   connect (pMnLangFilter, SIGNAL(triggered(QAction*)), this, SLOT(slotLangFilterChannelList(QAction*)));
#endif // _TASTE_IPTV_RECORD
   connect (pWatchList,    SIGNAL(sigClick(QUrl)), this, SLOT(slotWlClick(QUrl)));
   connect (pWatchList,    SIGNAL(sigUpdCount()), this, SLOT(slotUpdWatchListCount()));
   connect (pFilterWidget, SIGNAL(sigFilter(QString)), this, SLOT(slotFilterChannelList(QString)));
   connect (&pixCache,     SIGNAL(sigLoadImage(QString)), pApiClient, SLOT(slotDownImg(QString)));
   connect (pApiClient,    SIGNAL(sigImage(QByteArray)), &pixCache, SLOT(slotImage(QByteArray)));

   connect (pApiParser,    SIGNAL(sigError(int,QString,QString)), this, SLOT(slotGlobalError(int,QString,QString)));
   connect (&timerWidget,  SIGNAL(timeOut()), this, SLOT(slotRecordTimerEnded()));
   connect (this,          SIGNAL(sigWLRecEnded()), pWatchList, SLOT(slotWLRecEnded()));
   connect (&streamLoader, SIGNAL(sigStreamRequested(int)), this, SLOT(slotDownStreamRequested(int)));
   connect (&streamLoader, SIGNAL(sigBufferPercent(int)), ui->labState, SLOT(bufferPercent(int)));
   connect (ui->hFrameFav, SIGNAL(sigAddFav(int)), this, SLOT(slotAddFav(int)));
   connect (&pixCache,     SIGNAL(allDone()), this, SLOT(slotRefreshChanLogos()));
   connect (pApiClient,    SIGNAL(sigHttpResponse(QString,int)), this, SLOT(slotKartinaResponse(QString,int)));
   connect (pApiClient,    SIGNAL(sigError(QString,int,int)), this, SLOT(slotKartinaErr(QString,int,int)));
   connect (&streamLoader, SIGNAL(sigStreamDownload(int,QString)), this, SLOT(slotDownloadStarted(int,QString)));
   connect (&Refresh,      SIGNAL(timeout()), this, SLOT(slotUpdateChannelList()));
   connect (ui->textEpg,   SIGNAL(anchorClicked(QUrl)), this, SLOT(slotEpgAnchor(QUrl)));
   connect (&Settings,     SIGNAL(sigReloadLogos()), this, SLOT(slotReloadLogos()));
   connect (&Settings,     SIGNAL(sigSetServer(QString)), this, SLOT(slotSetSServer(QString)));
   connect (&Settings,     SIGNAL(sigSetBitRate(int)), this, SLOT(slotSetBitrate(int)));
   connect (&Settings,     SIGNAL(sigSetTimeShift(int)), this, SLOT(slotSetTimeShift(int)));
   connect (&timeRec,      SIGNAL(sigRecDone()), this, SLOT(slotTimerRecordDone()));
   connect (&timeRec,      SIGNAL(sigRecActive(int)), this, SLOT(slotTimerRecActive(int)));
   if (Settings.HideToSystray() && QSystemTrayIcon::isSystemTrayAvailable())
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
   connect (this,           SIGNAL(sigLockParentalManager()), &Settings, SLOT(slotLockParentalManager()));
   connect (ui->scrollArea, SIGNAL(resized(QSize,QSize)), this, SLOT(rebuildFavs(QSize,QSize)));

   connect (pModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
   connect (pModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));

   // HLS play stuff ...
   connect (pApiClient, SIGNAL(sigM3u(int,QString)), pHlsControl, SLOT(slotM3uResp(int,QString)));
   connect (pApiClient, SIGNAL(sigHls(int,QByteArray)), pHlsControl, SLOT(slotStreamTokResp(int,QByteArray)));
   connect (pHlsControl, SIGNAL(sigPlay(QString)), this, SLOT(slotPlayHls(QString)));
   connect (ui->player, SIGNAL(sigStopOnDemand()), this, SLOT(stopOnDemand()));

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

#ifdef Q_OS_MACX
   // set favourites group layout spacing for Mac
   ui->gLayoutFav->setHorizontalSpacing(12);
   ui->gLayoutFav->setVerticalSpacing(12);
#endif // Q_OS_MACX
   ui->gLayoutFav->setSizeConstraint(QLayout::SetMinAndMaxSize);

   // enable button ...
   TouchPlayCtrlBtns(false);

   // fill type combo box ...
   touchLastOrBestCbx();

   // start refresh timer, if needed ...
   Refresh.start(60000); // update chan list every minute ...
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
   // clean our mission control ...
   missionControl.disconnectCtrls();

   // pHelp isn't owned by Recorder --> delete separatly ...
   if (pHelp != NULL)
   {
      delete pHelp;
      pHelp = NULL;
   }

   delete ui;
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
   switch (e->type())
   {
   // catch minimize event ...
   case QEvent::WindowStateChange:

      // printStateChange (((QWindowStateChangeEvent *)e)->oldState());
      if (isMinimized())
      {
         // only hide window, if trayicon stuff is available ...
         if (QSystemTrayIcon::isSystemTrayAvailable () && Settings.HideToSystray())
         {
            // close help ...
            pHelp->close();

            // hide dialog ...
            QTimer::singleShot(300, this, SLOT(hide()));
         }
      }
      break;

   // language switch ...
   case QEvent::LanguageChange:
      ui->retranslateUi(this);

#ifdef _TASTE_IPTV_RECORD
      // translate language menu entry ...
      pMnLangFilter->setTitle(tr("Language Filter"));
#endif // _TASTE_IPTV_RECORD

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
      break;

   default:
      QWidget::changeEvent(e);
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

    case IncPlay::PS_PLAY:
        if (ui->player->isPlaying() && Settings.doLastPlay())
        {
            QtJson::JsonObject lastPlay;

            if (showInfo.showType() == ShowInfo::Live)
            {
                // save channel id to be restored on next start ...
                lastPlay["type"] = QVariant("live");
                lastPlay["cid"]  = QVariant(showInfo.channelId());
                lastPlay["gid"]  = QVariant(showInfo.chanGrp());
            }
            else if (showInfo.showType() == ShowInfo::Archive)
            {
                // save channel id and gmt timestamp to be restored on next start ...
                lastPlay["type"]   = QVariant("archive");
                lastPlay["cid"]    = QVariant(showInfo.channelId());
                lastPlay["gid"]    = QVariant(showInfo.chanGrp());
                lastPlay["start"]  = QVariant(showInfo.starts());
                lastPlay["end"]    = QVariant(showInfo.ends());
                lastPlay["pos"]    = QVariant(ui->player->getSilderPos());
                lastPlay["eol"]    = QVariant(showInfo.endOfLife());
                lastPlay["inf"]    = QVariant(showInfo.htmlDescr());
                lastPlay["name"]   = QVariant(showInfo.showName());
            }
            mInfo(tr("Save last play data: %1 ...").arg(QtJson::serializeStr(lastPlay).left(15)));
            Settings.setLastPlay(QtJson::serializeStr(lastPlay));
        }
        break;

    default:
        break;
    }

    if (bAccept)
    {
        // close help dialog ..
        pHelp->close();

        // We want to close program, store all needed values ...
        // Note: putting this function in destructor doesn't work!
        if (eCurDMode == Ui::DM_NORMAL)
        {
            savePositions();
        }

        // save font size and favorites ...
        Settings.SetCustFontSize(iFontSzChg);
        Settings.SaveFavourites(lFavourites);
        Settings.saveFavsInRow(miFavsInRow);

        // save channel and epg position ...
        Settings.saveChannel(getCurrentCid());
        Settings.saveChanGrp(getCurrentGid());
        Settings.saveEpgDay(iEpgOffset ? QDate::currentDate().addDays(iEpgOffset).toString("ddMMyyyy") : "");

        // clear shortcuts ...
        ClearShortCuts ();

        // clean favourites ...
        lFavourites.clear();
        HandleFavourites();

        // delete context menu stuff ...
        favContext.clear();

        // are we authenticated ... ?
        if (pApiClient->cookieSet())
        {
            // logout from kartina ...
            QTimer::singleShot(200, this, SLOT(slotTriggeredLogout()));

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
   QString sHlp;

   // in case settings will be changed we should return to current channel group ...
   Settings.saveChanGrp(getCurrentGid());

   if (Settings.exec() == QDialog::Accepted)
   {
      // if changes where saved, accept it here ...
      VlcLog.SetLogLevel(Settings.GetLogLevel());

      pModel->clear();
      // pApiClient->abort();

      // update connection data ...
      pApiClient->SetData(Settings.GetAPIServer(), Settings.GetUser(), Settings.GetPasswd(), Settings.GetLanguage());

      // set proxy ...
      if (Settings.UseProxy())
      {
         QNetworkProxy proxy(QNetworkProxy::HttpProxy,
                             Settings.GetProxyHost(), Settings.GetProxyPort(),
                             Settings.GetProxyUser(), Settings.GetProxyPasswd());

         pApiClient->setProxy(proxy);
         streamLoader.setProxy(proxy);
      }

      // do we use libVLC ?
      if (Settings.GetPlayerModule().contains("libvlc", Qt::CaseInsensitive))
      {
         vlcCtrl.UseLibVlc(true);
      }
      else
      {
         vlcCtrl.UseLibVlc(false);
      }

      // give vlcCtrl needed infos ...
      vlcCtrl.LoadPlayerModule(Settings.GetPlayerModule());
      vlcCtrl.SetTranslitSettings(Settings.TranslitRecFile());

      TouchPlayCtrlBtns(false);

      // authenticate ...
      pApiClient->queueRequest(CIptvDefs::REQ_COOKIE);
   }

   // lock parental manager ...
   emit sigLockParentalManager();

   if (Settings.HideToSystray() && QSystemTrayIcon::isSystemTrayAvailable())
   {
      connect (this, SIGNAL(sigHide()), &trayIcon, SLOT(show()));
      connect (this, SIGNAL(sigShow()), &trayIcon, SLOT(hide()));
   }
   else
   {
      disconnect(this, SIGNAL(sigHide()));
      disconnect(this, SIGNAL(sigShow()));
   }

   // set new(?) helpfile ...
   // be sure the file we want to load exists ... fallback to english help ...
   sHlp = QString("%1/help_%2.qhc").arg(pFolders->getDocDir()).arg(Settings.GetLanguage());

   if (!QFile::exists(sHlp))
   {
      sHlp = QString("%1/help_en.qhc").arg(pFolders->getDocDir());
   }

   pHelp->setHelpFile(sHlp);
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
    int gid = ui->cbxChannelGroup->itemData(index).toInt();

    if (gid == DEF_FAV_GRP)
    {
        int favGid;
        QString sName, sLogo;

        QtJson::JsonArray  pseudoList;
        QtJson::JsonObject info;
        QtJson::JsonObject root;
        QtJson::JsonObject epg;
        QtJson::JsonObject channel;
        QStringList slChans;

        foreach (int cid, lFavourites)
        {
            slChans << QString::number(cid);

            // get additional data ...
            Settings.favData(cid, favGid, sName, sLogo);

            info["id"]     = QVariant(cid);
            info["groups"] = QVariant(favGid);
            info["name"]   = QVariant(sName);

            channel["info"] = info;
            channel["epg"]  = epg;

            pseudoList.append(channel);
        }

        root["channels"] = pseudoList;

        mInfo(tr("Faked channel list: %1").arg(QtJson::serializeStr(root)));
        slotChanList(QtJson::serializeStr(root));

        // epg current for favourites ...
        pApiClient->queueRequest(CIptvDefs::REQ_EPG_CURRENT, slChans.join(","));

        // favourites can be sorted by drag'n'drop ...
        ui->channelList->setDragDropMode(QAbstractItemView::InternalMove);
    }
    else
    {
        pApiClient->queueRequest(CIptvDefs::REQ_CHANNELLIST, gid);

        // remove drag'n'drop option ...
        ui->channelList->setDragDropMode(QAbstractItemView::DragOnly);
    }
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
   CAboutDialog dlg(this, accountInfo);
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
      cparser::SChan chan;

      if (!pChanMap->entry(cid, chan))
      {
         if (AllowAction(IncPlay::PS_PLAY))
         {
            if (grantAdultAccess(chan.bIsProtected))
            {
               showInfo.cleanShowInfo();
               showInfo.setChanGrp(getCurrentGid());
               showInfo.setChanId(cid);
               showInfo.setChanName(chan.sName);
               showInfo.setShowType(ShowInfo::Live);
               showInfo.setShowName(chan.sProgramm);
               showInfo.setStartTime(chan.uiStart);
               showInfo.setEndTime(chan.uiEnd);
               showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
               showInfo.setPlayState(IncPlay::PS_PLAY);
               showInfo.setPCode(secCodeDlg.passWd());
               showInfo.setDefAStream((int)chan.uiDefAud);
               showInfo.setLangCode(chan.sLangCode);
               showInfo.setHtmlDescr(pHtml->createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd, chan.iTs));

               TouchPlayCtrlBtns(false);

               stopOnDemand();

               pApiClient->queueRequest(chan.bIsVideo ? CIptvDefs::REQ_STREAM : CIptvDefs::REQ_RADIO_STREAM,
                                      cid, secCodeDlg.passWd());
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

#ifdef _HAS_VOD_LANG
   url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

   if (iGid != -1)
   {
      url.addQueryItem("genre", QString::number(iGid));
   }

   pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
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
      pApiClient->queueRequest(CIptvDefs::REQ_GET_VOD_FAV);
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

#ifdef _HAS_VOD_LANG
      url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

      pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   VOD language was changed -> request videos
//
//! \author  Jo2003
//! \date    28.01.2014
//
//! \param   index (int) new selection index
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::on_cbxVodLang_activated(int index)
{
   int  iGid  = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();

   QUrl url;
   url.addQueryItem("type", ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString());
   url.addQueryItem("lang", ui->cbxVodLang->itemData(index).toString());
   url.addQueryItem("nums", "20");
   if (iGid != -1)
   {
      url.addQueryItem("genre", QString::number(iGid));
   }
   pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
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

#ifdef _HAS_VOD_LANG
   url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

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

   pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
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

#ifdef _HAS_VOD_LANG
      url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

      if (iGenre != -1)
      {
         url.addQueryItem("genre", QString::number(iGenre));
      }

      pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
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

#ifdef _HAS_VOD_LANG
   url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

   if (iGenre != -1)
   {
      url.addQueryItem("genre", QString::number(iGenre));
   }

   pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
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

#ifdef _HAS_VOD_LANG
   url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

   if (iGenre != -1)
   {
      url.addQueryItem("genre", QString::number(iGenre));
   }

   pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
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
   cparser::SChan chan;

   if (!pChanMap->entry(cid, chan))
   {
      // set EPG offset to 0 ...
      iEpgOffset = 0;
      pApiClient->queueRequest(CIptvDefs::REQ_EPG, cid, iEpgOffset);

      // fake play button press ...
      if (AllowAction(IncPlay::PS_PLAY))
      {
         if (grantAdultAccess(chan.bIsProtected))
         {
            showInfo.cleanShowInfo();
            showInfo.setChanGrp(getCurrentGid());
            showInfo.setChanId(cid);
            showInfo.setChanName(chan.sName);
            showInfo.setShowType(ShowInfo::Live);
            showInfo.setShowName(chan.sProgramm);
            showInfo.setStartTime(chan.uiStart);
            showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
            showInfo.setEndTime(chan.uiEnd);
            showInfo.setPCode(secCodeDlg.passWd());
            showInfo.setPlayState(IncPlay::PS_PLAY);
            showInfo.setDefAStream((int)chan.uiDefAud);
            showInfo.setLangCode(chan.sLangCode);
            showInfo.setHtmlDescr(pHtml->createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd, chan.iTs));

            TouchPlayCtrlBtns(false);

            stopOnDemand();

            pApiClient->queueRequest(chan.bIsVideo ? CIptvDefs::REQ_STREAM : CIptvDefs::REQ_RADIO_STREAM,
                                   cid, secCodeDlg.passWd());
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

      cparser::SChan chan;

      if (!pChanMap->entry(cid, chan))
      {
         if (AllowAction(IncPlay::PS_PLAY))
         {
            if (grantAdultAccess(chan.bIsProtected))
            {
               showInfo.cleanShowInfo();
               showInfo.setChanGrp(getCurrentGid());
               showInfo.setChanId(cid);
               showInfo.setChanName(chan.sName);
               showInfo.setShowType(ShowInfo::Live);
               showInfo.setShowName(chan.sProgramm);
               showInfo.setStartTime(chan.uiStart);
               showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
               showInfo.setEndTime(chan.uiEnd);
               showInfo.setPCode(secCodeDlg.passWd());
               showInfo.setPlayState(IncPlay::PS_PLAY);
               showInfo.setDefAStream((int)chan.uiDefAud);
               showInfo.setLangCode(chan.sLangCode);
               showInfo.setHtmlDescr(pHtml->createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd, chan.iTs));

               TouchPlayCtrlBtns(false);

               stopOnDemand();

               pApiClient->queueRequest(chan.bIsVideo ? CIptvDefs::REQ_STREAM : CIptvDefs::REQ_RADIO_STREAM,
                                      cid, secCodeDlg.passWd());
            }
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: on_pushHelp_clicked [slot]
|  Begin: 09.07.2012
|  Author: Jo2003
|  Description: help button (or shortcut) clicked
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushHelp_clicked()
{
   pHelp->show();
}

//---------------------------------------------------------------------------
//
//! \brief   filter button was pressed: show filter menu
//
//! \author  Jo2003
//! \date    29.07.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::on_pushFilter_clicked()
{
   pFilterWidget->lineFocus();
   pFilterMenu->exec(cursor().pos());
}

//---------------------------------------------------------------------------
//
//! \brief   watch list button was pressed
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::on_pushWatchList_clicked()
{
   pWatchList->buildWatchTab();
   pWatchList->exec();
}

//---------------------------------------------------------------------------
//
//! \brief   undo channel list filtering
//
//! \author  Jo2003
//! \date    20.01.2016
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::on_pushUndoFilter_clicked()
{
    pFilterWidget->cleanFilter();
    slotFilterChannelList("");

    ui->pushFilter->setIcon(QIcon(":/app/filter"));
}

//---------------------------------------------------------------------------
//! \brief   received settings from tvclub ..
//
//! \param[in] resp (QString) string response
//---------------------------------------------------------------------------
void Recorder::slotSettings(QString resp)
{
    pApiParser->parseSettings(resp, Settings);

    pApiClient->queueRequest(CIptvDefs::REQ_CHANNELGROUPS);
}

////////////////////////////////////////////////////////////////////////////////
//                                Slots                                       //
////////////////////////////////////////////////////////////////////////////////

/* -----------------------------------------------------------------\
|  Method: slotPlay [slot]
|  Begin: 19.01.2010 / 16:12:20
|  Author: Jo2003
|  Description: request stream url for play
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotPlay()
{
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
      int cid  = getCurrentCid();

      cparser::SChan chan;

      if (!pChanMap->entry(cid, chan))
      {
         if (AllowAction(IncPlay::PS_PLAY))
         {
            if (grantAdultAccess(chan.bIsProtected))
            {
               showInfo.cleanShowInfo();
               showInfo.setChanGrp(getCurrentGid());
               showInfo.setChanId(cid);
               showInfo.setChanName(chan.sName);
               showInfo.setShowType(ShowInfo::Live);
               showInfo.setShowName(chan.sProgramm);
               showInfo.setStartTime(chan.uiStart);
               showInfo.setEndTime(chan.uiEnd);
               showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
               showInfo.setPlayState(IncPlay::PS_PLAY);
               showInfo.setPCode(secCodeDlg.passWd());
               showInfo.setDefAStream((int)chan.uiDefAud);
               showInfo.setLangCode(chan.sLangCode);
               showInfo.setHtmlDescr(pHtml->createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd, chan.iTs));

               TouchPlayCtrlBtns(false);

               stopOnDemand();

               pApiClient->queueRequest(chan.bIsVideo ? CIptvDefs::REQ_STREAM : CIptvDefs::REQ_RADIO_STREAM,
                                      cid, secCodeDlg.passWd());
            }
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotStop [slot]
|  Begin: 30.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: stop button was pressed, stop vlc after request
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotStop()
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

      pHlsControl->stop();

      showInfo.setPlayState(IncPlay::PS_STOP);
      TouchPlayCtrlBtns(true);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotRecord [slot]
|  Begin: 19.01.2010 / 16:11:52
|  Author: Jo2003
|  Description: request stream url for record
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotRecord()
{
   // when record is active already we display the
   // record timer dialog ...
   if ((showInfo.playState () == IncPlay::PS_RECORD) && (showInfo.showType() != ShowInfo::VOD))
   {
      timerWidget.exec();
   }
   else
   {
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

            stopOnDemand();

            pApiClient->queueRequest(CIptvDefs::REQ_ARCHIV, req, showInfo.pCode());
         }
      }
      else
      {
         int cid = getCurrentCid();

         cparser::SChan chan;

         if (!pChanMap->entry(cid, chan))
         {
            if (AllowAction(IncPlay::PS_RECORD))
            {
               if (grantAdultAccess(chan.bIsProtected))
               {
                  // new own downloader ...
                  if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
                  {
                     streamLoader.stopDownload (iDwnReqId);
                     iDwnReqId = -1;
                  }

                  showInfo.cleanShowInfo();
                  showInfo.setChanGrp(getCurrentGid());
                  showInfo.setChanId(cid);
                  showInfo.setChanName(chan.sName);
                  showInfo.setShowType(ShowInfo::Live);
                  showInfo.setShowName(chan.sProgramm);
                  showInfo.setStartTime(chan.uiStart);
                  showInfo.setEndTime(chan.uiEnd);
                  showInfo.setLastJumpTime(QDateTime::currentDateTime().toTime_t());
                  showInfo.setPCode(secCodeDlg.passWd());
                  showInfo.setPlayState(IncPlay::PS_RECORD);
                  showInfo.setDefAStream((int)chan.uiDefAud);
                  showInfo.setLangCode(chan.sLangCode);
                  showInfo.setHtmlDescr(pHtml->createTooltip(chan.sName, chan.sProgramm, chan.uiStart, chan.uiEnd, chan.iTs));

                  TouchPlayCtrlBtns(false);

                  stopOnDemand();

                  pApiClient->queueRequest(chan.bIsVideo ? CIptvDefs::REQ_STREAM : CIptvDefs::REQ_RADIO_STREAM,
                                         cid, secCodeDlg.passWd());
               }
            }
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotBwd [slot]
|  Begin: 23.06.2010 / 12:32:12
|  Author: Jo2003
|  Description: jump backward
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotBwd()
{
   // we have minutes but need seconds --> x 60!!!
   int iJmpVal = missionControl.getJumpValue() * 60;

   // jump ...
   ui->player->slotTimeJumpRelative(-iJmpVal);
}

/* -----------------------------------------------------------------\
|  Method: slotFwd [slot]
|  Begin: 23.06.2010 / 12:32:12
|  Author: Jo2003
|  Description: jump forward
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotFwd()
{
   // we have minutes but need seconds --> x 60!!!
   int iJmpVal = missionControl.getJumpValue() * 60;

   // jump ...
   ui->player->slotTimeJumpRelative(iJmpVal);
}

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
void Recorder::slotKartinaResponse(QString resp, int req)
{
   // helper macro to have a nice info printout ...
#define mkCase(__x__, __y__) \
      case __x__: \
         mInfo(tr("\n  --> HTTP Response '%1', calling '%2'").arg(#__x__).arg(#__y__)); \
         __y__; \
         break

   switch ((CIptvDefs::EReq)req)
   {
   ///////////////////////////////////////////////
   // This function also grabs all settings
   // from response. After that channel list
   // will be requested.
   mkCase(CIptvDefs::REQ_COOKIE, slotCookie(resp));

   ///////////////////////////////////////////////
   // Fills channel list as well as channel map.
   // Due to changing actual channel entry
   // slotCurrentChannelChanged() will be called
   // which requests the EPG ...
   mkCase(CIptvDefs::REQ_CHANNELLIST, slotChanList(resp));

   ///////////////////////////////////////////////
   // settings received ...
   mkCase(CIptvDefs::REQ_SETTINGS, slotSettings(resp));

   /**
    * get channel group response and parse it
    * and request channel list
    */
   mkCase(CIptvDefs::REQ_CHANNELGROUPS, slotChanGroups(resp));

   ///////////////////////////////////////////////
   // Fills EPG browser and triggers the load
   // of VOD genres (if there in account info).
   mkCase(CIptvDefs::REQ_EPG, slotEPG(resp));

   ///////////////////////////////////////////////
   // update channel map with following info
   mkCase(CIptvDefs::REQ_EPG_CURRENT, slotEPGCurrent (resp));

   ///////////////////////////////////////////////
   // Indicates that a new timeshift value was set.
   // Triggers reload of channel list.
   mkCase(CIptvDefs::REQ_TIMESHIFT, slotTimeShift(resp));

   ///////////////////////////////////////////////
   // Got Stream URL, start play or record
   mkCase(CIptvDefs::REQ_STREAM, slotStreamURL(resp));

   ///////////////////////////////////////////////
   // Got requested stream url for timer record
   mkCase(CIptvDefs::REQ_TIMERREC, timeRec.slotTimerStreamUrl(resp));

   ///////////////////////////////////////////////
   // got requested archiv url
   mkCase(CIptvDefs::REQ_ARCHIV, slotArchivURL(resp));

   ///////////////////////////////////////////////
   // logout done
   mkCase(CIptvDefs::REQ_LOGOUT, slotLogout(resp));

   ///////////////////////////////////////////////
   // got requested VOD genres
   mkCase(CIptvDefs::REQ_GETVODGENRES, slotGotVodGenres(resp));

   ///////////////////////////////////////////////
   // got requested videos
   mkCase(CIptvDefs::REQ_GETVIDEOS, slotGotVideos(resp));

   ///////////////////////////////////////////////
   // got requested video details
   mkCase(CIptvDefs::REQ_GETVIDEOINFO, slotGotVideoInfo(resp));

   ///////////////////////////////////////////////
   // got requested vod url
   mkCase(CIptvDefs::REQ_GETVODURL, slotVodURL(resp));

   ///////////////////////////////////////////////
   // got complete channel list
   // (used in settings dialog)
   mkCase(CIptvDefs::REQ_CHANLIST_ALL, Settings.slotBuildChanManager(resp));

   ///////////////////////////////////////////////
   // got requested VOD management data
   // (used in settings dialog)
   mkCase(CIptvDefs::REQ_GET_VOD_MANAGER, Settings.slotBuildVodManager(resp));

   ///////////////////////////////////////////////
   // handle vod favourites like vod genre to display
   // all videos in favourites
   mkCase(CIptvDefs::REQ_GET_VOD_FAV, slotGotVideos(resp, true));

   ///////////////////////////////////////////////
   // response after trying to change parent code
   mkCase(CIptvDefs::REQ_SET_PCODE, slotPCodeChangeResp(resp));

   ///////////////////////////////////////////////
   // response for update check ...
   mkCase(CIptvDefs::REQ_UPDATE_CHECK, slotUpdateAnswer(resp));

   ///////////////////////////////////////////////
   // response for available audio streams (where supported) ...
   mkCase(CIptvDefs::REQ_GET_ALANG, slotALang(resp));

   ///////////////////////////////////////////////
   // response for available VOD languages (where supported) ...
   mkCase(CIptvDefs::REQ_VOD_LANG, slotVodLang(resp));

   ///////////////////////////////////////////////
   // relogin without starting connection chain
   mkCase(CIptvDefs::REQ_LOGIN_ONLY, loginOnly(resp));

   ///////////////////////////////////////////////
   // Make sure the unused responses are listed
   // This makes it easier to understand the log.
   mkCase(CIptvDefs::REQ_ADD_VOD_FAV, slotUnused(resp));
   mkCase(CIptvDefs::REQ_REM_VOD_FAV, slotUnused(resp));
   mkCase(CIptvDefs::REQ_SET_VOD_MANAGER, slotUnused(resp));
   mkCase(CIptvDefs::REQ_SETCHAN_SHOW, slotUnused(resp));
   mkCase(CIptvDefs::REQ_SETCHAN_HIDE, slotUnused(resp));
   mkCase(CIptvDefs::REQ_SETBITRATE, slotUnused(resp));
   mkCase(CIptvDefs::REQ_GETBITRATE, slotUnused(resp));
   mkCase(CIptvDefs::REQ_GETTIMESHIFT, slotUnused(resp));
   mkCase(CIptvDefs::REQ_GET_SERVER, slotUnused(resp));
   mkCase(CIptvDefs::REQ_SERVER, slotUnused(resp));
   mkCase(CIptvDefs::REQ_HTTPBUFF, slotUnused(resp));
   mkCase(CIptvDefs::REQ_SET_LANGUAGE, slotUnused(resp));
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
|  Method: slotKartinaErr
|  Begin: 19.01.2010 / 16:08:51
|  Author: Jo2003
|  Description: display errors signaled by other threads
|
|  Parameters: error string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotKartinaErr (QString str, int req, int err)
{
   bool bSilent = false;

#ifdef _USE_QJSON
   // Note: when using json, error isn't parsed correctly!
   // Parse now!
   int iErr = 0;
   QString sErr, sTrn;

   if (!pApiParser->parseError(str, sErr, iErr))
   {
      err = iErr;

      // try to translate string ...
      sTrn = pAppTransl->translate(pApiClient->objectName().toUtf8().constData(),
                                             sErr.toUtf8().constData());
      str = (sTrn != "") ? sTrn : sErr;
   }

#endif // _USE_QJSON

   // special error handling for special requests ...
   switch ((CIptvDefs::EReq)req)
   {
   case CIptvDefs::REQ_SET_PCODE:
      Settings.slotEnablePCodeForm();
      bSilent = true;
      break;

   // in case of not exsting images send empty byte array
   // to trigger next download
   case CIptvDefs::REQ_DOWN_IMG:
      pixCache.slotImage(QByteArray());
      // fall through here
      //  V      V     V
   case CIptvDefs::REQ_UPDATE_CHECK:
      bSilent = true;
      break;

   default:
      break;
   }

   // Since almost all error codes
   // are unusable because they differ from
   // the codes defined by Kartina.TV we wont
   // decide anything based on these error codes!
   mErr(tr("Error %1 in request '%2': %3")
        .arg(err)
        .arg(metaKartina.reqValToKey((CIptvDefs::EReq)req))
        .arg(str));

   // But ... as always there is one exception...
   if (err == 2)
   {
       // most boring error ever on rodnoe server ...
       // this error could happen always ... and no one at rodnoe.tv takes
       // care for it.

       // don't start a auto re-login loop on dump error -> use a timer
       if (!mtAutoReLogin.isActive())
       {
           mInfo(tr("Silent re-login on error #2!"));

           // silently make re-login and after that re-send last request ...
           pApiClient->reLogin();
           pApiClient->requeue(false);
           mtAutoReLogin.start();
           bSilent = true;
       }
   }

   if (!bSilent)
   {
      if (!mpRetryDlg) mpRetryDlg = new QRetryDialog(this);

      QString errStr = tr("%1 Client API Error:\n%2 (#%3)")
                         .arg(pCustomization->strVal("COMPANY_NAME"))
                         .arg(str)
                         .arg(err);

      mpRetryDlg->setMessage(errStr, style()->standardIcon(QStyle::SP_MessageBoxCritical), tr("Error"));

      if (mpRetryDlg->exec() == QDialog::Accepted)
      {
         switch(mpRetryDlg->lastRole())
         {
         case QDialogButtonBox::AcceptRole:
            pApiClient->requeue();
            break;

         case QDialogButtonBox::YesRole:
            pApiClient->reLogin();
            break;

         default:
            break;
         }
      }
   }

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

   if (vlcCtrl.withLibVLC())
   {
      ui->player->stop();
   }
   else if (vlcCtrl.IsRunning())
   {
      vlcCtrl.stop();
   }

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

   if (!pApiParser->parseUrl(str, sUrl))
   {
      if (sUrl == "protected")
      {
         // hack! We don't get an error in some
         // cases therefore we here check the "magic"
         // keyword "protected" ...
         QMessageBox::critical(this, tr("Error!"), tr("Authentication error"));
         showInfo.setPCode("");
         secCodeDlg.slotClearPasswd();

         // change the player state to error ...!
         ePlayState = IncPlay::PS_ERROR;
      }
      else
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
               if (!check4PlayList(sUrl, sShow))
               {
                  StartVlcRec(sUrl, sShow);
               }
            }
            else
            {
               if (vlcCtrl.withLibVLC())
               {
                  ui->player->silentStop();
               }
               else if (vlcCtrl.IsRunning())
               {
                  vlcCtrl.stop();
               }
               showInfo.useStreamLoader(true);
               StartStreamDownload(sUrl, sShow);
            }
         }
         else if (ePlayState == IncPlay::PS_PLAY)
         {
            if (!check4PlayList(sUrl))
            {
               StartVlcPlay(sUrl);
            }
         }
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
   QString s;

   // parse cookie ...
   if (!pApiParser->parseCookie(str, s, accountInfo))
   {
      pApiClient->SetCookie(s);

      if (accountInfo.dtExpires.isValid())
      {
         int iDaysTo = QDateTime::currentDateTime().daysTo(accountInfo.dtExpires);

         // be sure to don't tell about negative days ... !
         if ((iDaysTo >= 0) && (iDaysTo <= 7))
         {
            qint64 llCheck = pDb->stringValue("ExpNextRemind").toLongLong();

            if (QDateTime::currentDateTime().toTime_t() > llCheck)
            {
               QString content = tr("Your subscription will end in %1 day(s).").arg(iDaysTo);

               if (!pCustomization->strVal("COMPANY_LINK").isEmpty())
               {
                   content += tr("<br />Visit %1 to renew it!").arg(pCustomization->strVal("COMPANY_LINK"));
               }

               expNotifier.setNotifyContent(pHtml->htmlPage(content, "Account Info"));
               expNotifier.exec();
            }
         }
      }

      // decide if we should enable / disable VOD stuff ...
      if (accountInfo.bHasVOD)
      {
         if (!ui->tabEpgVod->widget(vodTabWidget.iPos))
         {
            // make sure tab text is translated as needed
            QString title = pAppTransl->translate(objectName().toUtf8().constData(),
                                                   vodTabWidget.sText.toUtf8().constData());

            // add tab ...
            ui->tabEpgVod->addTab(vodTabWidget.pWidget, (title != "") ? title : vodTabWidget.sText);
            ui->tabEpgVod->adjustSize();

#ifdef _TASTE_POLSKY_TV
            // we should make the tab button more visible for
            // Polsky.TV ...
            QString stSh = ui->tabEpgVod->styleSheet();

            if (!stSh.contains(":last"))
            {
               stSh += "QTabBar::tab:last:hover {\n"
                       "  border-bottom-color: #eee;\n"
                       "}\n"
                       "QTabBar::tab:last {\n"
                       "  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #FFCACA, stop:0.7 #FF6666, stop:1 #FFCECE);\n"
                       "}\n"
                       "QTabBar::tab:last:selected {\n"
                       "  background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:0, stop:0 #FFCACA, stop:0.7 #FF6666, stop:1 #FFCECE);\n"
                       "  border-bottom-color: #eee;\n"
                       "  border-top-color: #800;\n"
                       "}";

               ui->tabEpgVod->setStyleSheet(stSh);
            }
#endif // _TASTE_POLSKY_TV
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
      if (!pApiParser->parseSetting(str, "timeshift", values, actVal))
      {
         Settings.fillTimeShiftCbx(values, actVal);

         // set timeshift ...
         mInfo(tr("Using following timeshift: %1").arg(actVal));
      }

      // set timeshift to global ts class ...
      pTs->setTimeShift(Settings.getTimeShift());

      // bitrate
      values.clear();
      actVal = -1;
      if (!pApiParser->parseSetting(str, "bitrate", values, actVal))
      {
         Settings.SetBitrateCbx(values, actVal);
         mInfo (tr("Using Bitrate %1 kbit/s ...").arg(actVal));
      }

      // stream server
      QVector<cparser::SSrv> vSrv;
      QString sActIp;
      if (!pApiParser->parseSServersLogin(str, vSrv, sActIp))
      {
         Settings.SetStreamServerCbx(vSrv, sActIp);
         mInfo(tr("Active stream server is %1").arg(sActIp));
      }

#ifdef _TASTE_IPTV_RECORD
      if ((s = pApiParser->xmlElementToValue(str, "interface_lng")) != "")
      {
         Settings.setLanguage(s);
      }
#endif // _TASTE_IPTV_RECORD

#ifdef _TASTE_TV_CLUB
      pApiClient->queueRequest(CIptvDefs::REQ_SETTINGS);
#else
      // request channel list ...
      pApiClient->queueRequest(CIptvDefs::REQ_CHANNELLIST);
#endif // _TASTE_TV_CLUB
   }
}

//---------------------------------------------------------------------------
//
//! \brief   handle silent relogin
//
//! \author  Jo2003
//! \date    06.05.2014
//
//! \param   resp (const QString&) ref. to response
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::loginOnly(const QString &resp)
{
   QString s;
   cparser::SAccountInfo aInfo;

   // parse cookie ...
   if (!pApiParser->parseCookie(resp, s, aInfo))
   {
      pApiClient->SetCookie(s);
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
   pApiClient->queueRequest(CIptvDefs::REQ_CHANNELLIST);
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
    QChanList chanList;

    if (!pApiParser->parseChannelList(str, chanList, Settings.FixTime()))
    {
#ifdef __TRACE
        dumpChanList(chanList);
#endif // __TRACE

        // handle timeshift stuff if needed ...
        pApiParser->handleTsStuff(chanList);

        pChanMap->fillFromChannelList(chanList);
        // rebuild ...
        FillChannelList (chanList);

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

   if (idx.isValid())
   {
      int     cid      = qvariant_cast<int>(idx.data(channellist::cidRole));
      QIcon   icon;

      if (!pApiParser->parseEpg(str, epg))
      {
         cparser::SChan chan;

         if (!pChanMap->entry(cid, chan))
         {
            ui->textEpg->DisplayEpg(epg, chan.sName,
                                    cid, epgTime.toTime_t(),
                                    accountInfo.bHasArchive ? chan.bHasArchive : false, chan.iTs);

            // fill epg control ...
            icon = qvariant_cast<QIcon>(idx.data(channellist::iconRole));
            ui->labChanIcon->setPixmap(icon.pixmap(42, 24));
            ui->labChanName->setText(chan.sName);
            ui->labCurrDay->setText(epgTime.toString("dd. MMM. yyyy"));

            pEpgNavbar->setCurrentIndex(epgTime.date().dayOfWeek() - 1);

            TouchPlayCtrlBtns();
            ui->channelList->setFocus(Qt::OtherFocusReason);

            // update vod stuff only at startup ...
            if (accountInfo.bHasVOD)
            {
               if (ui->cbxGenre->count() == 0)
               {
                  pApiClient->queueRequest(CIptvDefs::REQ_GETVODGENRES);
               }
            }

            if (!mLastPlay.isEmpty())
            {
                QString type = mLastPlay.value("type").toString();
                if (type == "live")
                {
                    on_pushLive_clicked();
                }
                else if (type == "archive")
                {
                    // eol check ...
                    if (QDateTime::currentDateTime().toTime_t() > mLastPlay.value("eol").toUInt())
                    {
                        QMessageBox::warning(this, tr("Sorry"), tr("The show you watched last time was already removed from archive."));
                    }
                    else
                    {
                        // fake play button press ...
                        if (AllowAction(IncPlay::PS_PLAY))
                        {
                           if (grantAdultAccess(chan.bIsProtected))
                           {
                              QString req  = QString("cid=%1&gmt=%2").arg(cid).arg(mLastPlay.value("pos").toUInt());

                              showInfo.cleanShowInfo();
                              showInfo.setChanGrp(getCurrentGid());
                              showInfo.setChanId(cid);
                              showInfo.setChanName(chan.sName);
                              showInfo.setShowType(ShowInfo::Archive);
                              showInfo.setShowName(mLastPlay.value("name").toString());
                              showInfo.setStartTime(mLastPlay.value("start").toUInt());
                              showInfo.setLastJumpTime(mLastPlay.value("pos").toUInt());
                              showInfo.setEndTime(mLastPlay.value("end").toUInt());
                              showInfo.setPCode(secCodeDlg.passWd());
                              showInfo.setPlayState(IncPlay::PS_PLAY);
                              showInfo.setDefAStream((int)chan.uiDefAud);
                              showInfo.setLangCode(chan.sLangCode);
                              showInfo.setHtmlDescr(mLastPlay.value("inf").toString());

                              TouchPlayCtrlBtns(false);

                              stopOnDemand();

                              pApiClient->queueRequest(CIptvDefs::REQ_ARCHIV, req, secCodeDlg.passWd());
                           }
                        }
                    }
                }

                // was handled -> clear!
                mLastPlay.clear();
            }

#ifdef _TASTE_IPTV_RECORD
            // So good that we queue requests ...
            if (pMnLangFilter->actions().isEmpty())
            {
               pApiClient->queueRequest(CIptvDefs::REQ_GET_ALANG);
            }
#endif // _TASTE_IPTV_RECORD
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotEPGCurrent
|  Begin: 06.12.2012
|  Author: Jo2003
|  Description: update shows for different channels
|
|  Parameters: str (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotEPGCurrent (const QString &str)
{
    QCurrentMap                   currentEpg;
    QVector<cparser::SEpgCurrent> chanEntries;
    cparser::SChan                chanMapEntry;
    int i, j;

    if (!pApiParser->parseEpgCurrent(str, currentEpg))
    {
#ifdef _TASTE_TV_CLUB
        // update missing channel data with data from EPG ...
        if (getCurrentGid() == DEF_FAV_GRP)
        {
            QEpgChanInfMap addChanData;
            if (!((CTVClubParser*)pApiParser)->addChanInfo(addChanData))
            {
                pChanMap->updateChannelData(addChanData);
            }

            // make sure we update EPG browser with new updated channel data ...
            cparser::SChan cinfo;
            if (!pChanMap->entry(getCurrentCid(), cinfo, true))
            {
                ui->textEpg->recreateEpgEx(accountInfo.bHasArchive && cinfo.bHasArchive);
            }
        }
#endif
        QList<int> keyList = currentEpg.keys();

        for (i = 0; i < keyList.count(); i++)
        {
            chanEntries  = currentEpg.value(keyList.at(i));
            chanMapEntry = pChanMap->value(keyList.at(i), true);

            for (j = 0; j < chanEntries.count(); j++)
            {
                if (chanEntries.at(j).uiStart > chanMapEntry.uiStart)
                {
                    chanMapEntry.uiStart   = chanEntries.at(j).uiStart;

#ifdef _TASTE_IPTV_RECORD
                    chanMapEntry.uiEnd     = chanEntries.at(j).uiEnd;
#else
                    chanMapEntry.uiEnd     = ((j + 1) < chanEntries.count()) ? chanEntries.at(j + 1).uiStart : 0;
#endif // _TASTE_IPTV_RECORD

                    chanMapEntry.sProgramm = chanEntries.at(j).sShow;

                    pChanMap->update(keyList.at(i), chanMapEntry);

                    // leave this for loop ...
                    break;
                }
            }

            // update EPG browser if needed ...
            if ((keyList.at(i) == ui->textEpg->GetCid()) && !iEpgOffset)
            {
                if (QDateTime::fromTime_t(ui->textEpg->epgTime()).date() < QDateTime::currentDateTime().date())
                {
                    // update EPG ...
                    pApiClient->queueRequest(CIptvDefs::REQ_EPG, keyList.at(i));
                }
                else
                {
                    // reload EPG ...
                    ui->textEpg->recreateEpg();
                }
            }
        }

        if (!keyList.isEmpty())
        {
            slotUpdateChannelList(keyList);
        }
    }
}

//---------------------------------------------------------------------------
//
//! \brief   watch list link was clicked [slot]
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   url (QUrl) url of clicked link
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotWlClick(QUrl url)
{
   cparser::SChan chan;
   QString        req;
   QString        action = url.queryItemValue("action");
   QStringList    sl     = url.queryItemValue("show").split("\n");
   int            cid    = url.queryItemValue("cid").toInt();
   bool           ok     = false;
   bool           stop   = !!url.queryItemValue("stopatend").toInt();

   if (action == "wl_play")
   {
      if (AllowAction(IncPlay::PS_PLAY))
      {
         ok = true;
      }
   }
   else if (action == "wl_rec")
   {
      if (AllowAction(IncPlay::PS_RECORD))
      {
         ok  = true;
      }
   }

   if (ok)
   {
      if (!pChanMap->entry(cid, chan))
      {
         chan.uiStart   = url.queryItemValue("start").toUInt();
         chan.uiEnd     = url.queryItemValue("end").toUInt();
         chan.sProgramm = url.queryItemValue("show");

         if (grantAdultAccess(chan.bIsProtected))
         {
            TouchPlayCtrlBtns(false);

            // new own downloader ...
            if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
            {
               streamLoader.stopDownload (iDwnReqId);
               iDwnReqId = -1;
            }

            req  = QString("cid=%1&gmt=%2").arg(cid).arg(chan.uiStart);

            // store all info about show ...
            showInfo.cleanShowInfo();
            showInfo.setChanId(cid);
            showInfo.setChanName(chan.sName);
            showInfo.setShowName(sl.at(0));
            showInfo.setStartTime(chan.uiStart);
            showInfo.setEndTime(chan.uiEnd);
            showInfo.setShowType(ShowInfo::Archive);
            showInfo.setPlayState(ePlayState);
            showInfo.setLastJumpTime(0);
            showInfo.setPCode(secCodeDlg.passWd());
            showInfo.setDefAStream((int)chan.uiDefAud);
            showInfo.setLangCode(chan.sLangCode);

            showInfo.setHtmlDescr(pHtml->createTooltip(tr("%1 (Archive)").arg(showInfo.chanName()),
                                                          QString("%1\n%2").arg(sl.at(0)).arg((sl.count() > 1) ? sl.at(1) : ""),
                                                          chan.uiStart, chan.uiEnd, chan.iTs));

            // add additional info to LCD ...
            int     iTime = (chan.uiEnd) ? (int)((chan.uiEnd - chan.uiStart) / 60) : 60;
            QString sTime = tr("Length: %1 min.").arg(iTime);
            ui->labState->setHeader(showInfo.chanName() + tr(" (Ar.)"));
            ui->labState->setFooter(sTime);

            stopOnDemand();

            pApiClient->queueRequest(CIptvDefs::REQ_ARCHIV, req, secCodeDlg.passWd());

            if (stop)
            {
               timerWidget.startExtern((chan.uiEnd - chan.uiStart) + TIMER_REC_OFFSET);
            }
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
   QString action = link.queryItemValue("action");
   bool    ok     = false;
   uint    uiStart, uiEnd, uiEol;
   int            cid;
   QString        req;
   cparser::SChan sChan;
   epg::SShow     sEpg;

   if (action == "archivrec")
   {
      if (AllowAction(IncPlay::PS_RECORD))
      {
         ok    = true;
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
      uiStart = link.queryItemValue("start").toUInt();
      uiEnd   = link.queryItemValue("end").toUInt();
      cid     = link.queryItemValue("cid").toInt();

      timeRec.SetRecInfo(uiStart, uiEnd, cid, CleanShowName(ui->textEpg->epgShow(uiStart).sShowName));
      timeRec.exec();
   }
   else if(action == "remember")
   {
      sEpg  = ui->textEpg->epgShow(link.queryItemValue("gmt").toUInt());
      cid   = link.queryItemValue("cid").toInt();
      uiEol = link.queryItemValue("eol").toUInt();

      if (!pChanMap->entry(cid, sChan))
      {
         sChan.uiStart   = sEpg.uiStart;
         sChan.uiEnd     = sEpg.uiEnd;
         sChan.uiEnd     = uiEol;
         sChan.sProgramm = sEpg.sShowDescr.isEmpty() ? sEpg.sShowName : QString("%1\n%2").arg(sEpg.sShowName).arg(sEpg.sShowDescr);
         pDb->addWatchEntry(sChan);

         // update watch list count ...
         slotUpdWatchListCount();
      }
   }

   if (ok)
   {
      cid  = link.queryItemValue("cid").toInt();

      if (!pChanMap->entry(cid, sChan))
      {
         if (grantAdultAccess(sChan.bIsProtected))
         {
            TouchPlayCtrlBtns(false);

            // new own downloader ...
            if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
            {
               streamLoader.stopDownload (iDwnReqId);
               iDwnReqId = -1;
            }

            uiStart = link.queryItemValue("gmt").toUInt();
            uiEol   = link.queryItemValue("eol").toUInt();
            req     = QString("cid=%1&gmt=%2").arg(cid).arg(uiStart);
            sEpg    = ui->textEpg->epgShow(uiStart);

            // store all info about show ...
            showInfo.cleanShowInfo();
            showInfo.setEndOfLife(uiEol);
            showInfo.setChanGrp(getCurrentGid());
            showInfo.setEpgMap(ui->textEpg->exportProgMap());
            showInfo.setChanId(cid);
            showInfo.setChanName(sChan.sName);
            showInfo.setShowName(sEpg.sShowName);
            showInfo.setStartTime(uiStart);
            showInfo.setEndTime(sEpg.uiEnd);
            showInfo.setShowType(ShowInfo::Archive);
            showInfo.setPlayState(ePlayState);
            showInfo.setLastJumpTime(0);
            showInfo.setPCode(secCodeDlg.passWd());
            showInfo.setDefAStream((int)sChan.uiDefAud);
            showInfo.setLangCode(sChan.sLangCode);

            showInfo.setHtmlDescr(pHtml->createTooltip(tr("%1 (Archive)").arg(showInfo.chanName()),
                                                          QString("%1\n%2").arg(sEpg.sShowName).arg(sEpg.sShowDescr),
                                                          sEpg.uiStart, sEpg.uiEnd, sChan.iTs));

            // add additional info to LCD ...
            int     iTime = (sEpg.uiEnd) ? (int)((sEpg.uiEnd - sEpg.uiStart) / 60) : 60;
            QString sTime = tr("Length: %1 min.").arg(iTime);
            ui->labState->setHeader(showInfo.chanName() + tr(" (Ar.)"));
            ui->labState->setFooter(sTime);

            stopOnDemand();

            pApiClient->queueRequest(CIptvDefs::REQ_ARCHIV, req, secCodeDlg.passWd());
         }
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
   QChannelMap::const_iterator cit;

   // queue all icons ...
   pChanMap->lock();
   for (cit = pChanMap->constBegin(); cit != pChanMap->constEnd(); cit++)
   {
      if (!(*cit).bIsGroup)
      {
         pixCache.enqueuePic((*cit).sIcon, pFolders->getLogoDir());
      }
   }
   pChanMap->unlock();
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

   if (pChanMap->contains(cid, true))
   {
      // set actual day in previous week to munday ...
      int iActDay  = pEpgNavbar->currentIndex();
      int iOffBack = iEpgOffset;
      iEpgOffset  -= 7 + iActDay;

      correctEpgOffset();

      if (iOffBack != iEpgOffset)
      {
         pApiClient->queueRequest(CIptvDefs::REQ_EPG, cid, iEpgOffset);
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

   if (pChanMap->contains(cid, true))
   {
      // set actual day in next week to munday ...
      int iActDay  = pEpgNavbar->currentIndex();
      int iOffBack = iEpgOffset;

      iEpgOffset  += 7 - iActDay;

      correctEpgOffset();

      if (iOffBack != iEpgOffset)
      {
         pApiClient->queueRequest(CIptvDefs::REQ_EPG, cid, iEpgOffset);
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

   if (!pApiParser->parseUrl(str, sUrl))
   {
      if (ePlayState == IncPlay::PS_RECORD)
      {
         if (!vlcCtrl.ownDwnld())
         {
            if (!check4PlayList(sUrl, CleanShowName(showInfo.showName())))
            {
               StartVlcRec(sUrl, CleanShowName(showInfo.showName()));
            }
         }
         else
         {
            if (vlcCtrl.withLibVLC())
            {
               ui->player->silentStop();
            }
            else if (vlcCtrl.IsRunning())
            {
               vlcCtrl.stop();
            }
            showInfo.useStreamLoader(true);
            StartStreamDownload(sUrl, CleanShowName(showInfo.showName()));
         }

         showInfo.setPlayState(IncPlay::PS_RECORD);
      }
      else if (ePlayState == IncPlay::PS_PLAY)
      {
         if (!check4PlayList(sUrl))
         {
            StartVlcPlay(sUrl);
         }

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

   if (pChanMap->contains(cid, true))
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
            pApiClient->queueRequest(CIptvDefs::REQ_EPG, cid, iEpgOffset);
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
   pApiClient->queueRequest(CIptvDefs::REQ_SERVER, sIp);
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
   pApiClient->queueRequest(CIptvDefs::REQ_SETBITRATE, iRate);
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

   pApiClient->queueRequest(CIptvDefs::REQ_TIMESHIFT, iShift);
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

   pHlsControl->stop();
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
   int            cid = getCurrentCid();
   cparser::SChan entry;
   QFileInfo      fInfo;
   CFavAction    *pAction;

   if (!pChanMap->entry(cid, entry))
   {
      // create context menu ...
      favContext.clear();
      pAction = new CFavAction(this);

      fInfo.setFile(entry.sIcon);
      QString sLogoFile = QString("%1/%2").arg(pFolders->getLogoDir()).arg(fInfo.fileName());

      // is channel already in favourites ... ?
      if (lFavourites.contains(cid))
      {
         // create remove menu ...
         pAction->setText(tr("Remove from favourites"));
         pAction->setIcon(QIcon(sLogoFile));
         pAction->setFavData(cid, kartinafav::FAV_DEL);
      }
      else
      {
         // create add menu ...
         pAction->setText(tr("Add to favourites"));
         pAction->setIcon(QIcon(sLogoFile));
         pAction->setFavData(cid, kartinafav::FAV_ADD);
      }

      // add action to menu ...
      favContext.addAction(pAction);

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
      if (!lFavourites.contains(iCid))
      {
         cparser::SChan chan;

         if (pChanMap->entry(iCid, chan) == 0)
         {
             // add new favourite ...
             lFavourites.push_back(iCid);
             Settings.addFavData(iCid, chan.iPrimGrp, chan.sName, chan.sIcon);

             HandleFavourites();
         }
      }
   }
   else if (action == kartinafav::FAV_DEL)
   {
      slotRemoveFav(iCid);
   }
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
   CAboutDialog dlg(this, accountInfo);
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
   case IncPlay::PS_STOP:
      // display "stop" in case of "end" ...
      emit sigLCDStateChange ((int)IncPlay::PS_STOP);
      break;

   case IncPlay::PS_ERROR:
      // note about the error also in showInfo class ...
      showInfo.setPlayState((IncPlay::ePlayStates)iState);

      // new own downloader ...
      if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
      {
         streamLoader.stopDownload (iDwnReqId);
         iDwnReqId = -1;
      }

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
                                    "", Settings.GetBufferTime(),
                                    fileName, sExt);

   // start player if we have a command line ...
   if (sCmdLine != "")
   {
      vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState);
   }

   // successfully started ?
   if (!vlcpid)
   {
      QMessageBox::critical(this, tr("Error!"), tr("Can't start player!"));
      ePlayState = IncPlay::PS_ERROR;
      TouchPlayCtrlBtns();
   }
   else
   {
      mInfo(tr("Started player with pid #%1!").arg((uint)vlcpid));
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

   if (!pApiParser->parseGenres(str, vGenres))
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

#ifdef _HAS_VOD_LANG
   // before asking for vidoes get a list of available languages ...
   pApiClient->queueRequest(CIptvDefs::REQ_VOD_LANG);

#else
   // trigger video load ...
   QUrl url;
   url.addQueryItem("type", ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString());
   url.addQueryItem("nums", "20");
   pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
#endif // _HAS_VOD_LANG
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

   if (!pApiParser->parseVodList(str, vVodList, gInfo))
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
   QString action = link.queryItemValue("action");
   bool ok        = false;
   int  id        = 0;

   // check password ...
   if (link.queryItemValue("pass_protect").toInt())
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

      id = link.queryItemValue("vodid").toInt();

      pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOINFO, id, secCodeDlg.passWd());
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
      id = link.queryItemValue("vodid").toInt();
      pApiClient->queueRequest(CIptvDefs::REQ_ADD_VOD_FAV, id, secCodeDlg.passWd());
#ifndef _TASTE_IPTV_RECORD
      // due to different handling this doesn't work on rodnoe based services!
      pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOINFO, id, secCodeDlg.passWd());
#endif // _TASTE_IPTV_RECORD
   }
   else if (action == "del_fav")
   {
      id = link.queryItemValue("vodid").toInt();
      pApiClient->queueRequest(CIptvDefs::REQ_REM_VOD_FAV, id, secCodeDlg.passWd());
#ifndef _TASTE_IPTV_RECORD
      // due to different handling this doesn't work on rodnoe based services!
      pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOINFO, id, secCodeDlg.passWd());
#endif // _TASTE_IPTV_RECORD
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

      id = link.queryItemValue("vid").toInt();

      showInfo.cleanShowInfo();
      showInfo.setShowName(ui->vodBrowser->getName());
      showInfo.setShowType(ShowInfo::VOD);
      showInfo.setPlayState(ePlayState);
      showInfo.setHtmlDescr(ui->vodBrowser->getShortContent());
      showInfo.setVodId(id);

#ifdef _TASTE_IPTV_RECORD
      if (link.hasQueryItem("length"))
      {
          showInfo.setStartTime(QDateTime::currentDateTime().toTime_t());
          showInfo.setEndTime(showInfo.starts() + link.queryItemValue("length").toUInt());
      }
#endif // _TASTE_IPTV_RECORD

      ui->labState->setHeader(tr("Video On Demand"));
      ui->labState->setFooter(showInfo.showName());

      stopOnDemand();

      pApiClient->queueRequest(CIptvDefs::REQ_GETVODURL, id, secCodeDlg.passWd());
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
   if (!pApiParser->parseVideoInfo(str, vodInfo))
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

   if (!pApiParser->parseVodUrls(str, sUrls))
   {
      // show ads if there ...
      if (sUrls.count() > 1)
      {
         showInfo.setAdUrl(sUrls[1]);
      }

      if (ePlayState == IncPlay::PS_RECORD)
      {
         // use own downloader ... ?
         if (!vlcCtrl.ownDwnld())
         {
            if (!check4PlayList(sUrls[0], CleanShowName(showInfo.showName())))
            {
               StartVlcRec(sUrls[0], CleanShowName(showInfo.showName()));
            }
         }
         else
         {
            if (vlcCtrl.withLibVLC())
            {
               ui->player->silentStop();
            }
            else if (vlcCtrl.IsRunning())
            {
               vlcCtrl.stop();
            }

            // stream loader doesn't support adds so far ... ;-)
            showInfo.setAdUrl("");
            StartStreamDownload(sUrls[0], CleanShowName(showInfo.showName()), "m4v");
         }

         showInfo.setPlayState(IncPlay::PS_RECORD);
      }
      else if (ePlayState == IncPlay::PS_PLAY)
      {
         if (!check4PlayList(sUrls[0]))
         {
            StartVlcPlay(sUrls[0]);
         }

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

   if (idx.isValid())
   {
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

   if (idx.isValid())
   {
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
   cparser::SChan entry;

   if (!pChanMap->entry(cid, entry))
   {
      // update short info if we're in live mode
      if ((showInfo.showType() == ShowInfo::Live)
         && (showInfo.playState() == IncPlay::PS_STOP))
      {
         ui->textEpgShort->setHtml(pHtml->createTooltip(entry.sName, entry.sProgramm, entry.uiStart, entry.uiEnd, entry.iTs));
      }
/*
      // quick'n'dirty timeshift hack ...
      if (entry.vTs.count() <= 2) // no timeshift available ...
      {
         // temporary unset timeshift ...
         pTs->setTimeShift(0);
      }
      else
      {
         // set (back) to normal timeshift ...
         pTs->setTimeShift(Settings.getTimeShift());
      }
*/
      // was this a refresh or was channel changed ... ?
      if (cid != ui->textEpg->GetCid())
      {
         // load epg ...
         pApiClient->queueRequest(CIptvDefs::REQ_EPG, cid, iEpgOffset);
      }
      else // same channel ...
      {
         // refresh epg only, if we view current day in epg ...
         if (iEpgOffset == 0) // 0 means today!
         {
            pApiClient->queueRequest(CIptvDefs::REQ_EPG, cid);
         }
      }

      if (getCurrentGid() != DEF_FAV_GRP)
      {
         setChannelGroup(current.data(channellist::gidRole).toInt());
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
   slotPlay();
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
   slotPlay();
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
   pApiClient->queueRequest(CIptvDefs::REQ_COOKIE);
}

/* -----------------------------------------------------------------\
|  Method: slotUpdateAnswer [slot]
|  Begin: 12.10.2011
|  Author: Jo2003
|  Description: got update answer
|
|  Parameters: response string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotUpdateAnswer (const QString &str)
{
   // got update info ...
   cparser::SUpdInfo updInfo;

   if (!pApiParser->parseUpdInfo(str, updInfo))
   {
      // compare version ...
      if (    ((updInfo.iMajor > VERSION_MAJOR)
           || ((updInfo.iMajor == VERSION_MAJOR) && (updInfo.iMinor > VERSION_MINOR))
           || ((updInfo.iMajor == VERSION_MAJOR) && (updInfo.iMinor == VERSION_MINOR) && (updInfo.iBuild > VERSION_BUILD)))
         && !updInfo.sUrl.isEmpty())
      {
         int    iMinor  = pDb->intValue("UpdMinor");
         int    iMajor  = pDb->intValue("UpdMajor");
         int    iBuild  = pDb->intValue("UpdBuild");
         qint64 llCheck = pDb->stringValue("UpdNextCheck").toLongLong();
         bool   bDispl  = true;

         if ((updInfo.iMajor == iMajor) && (updInfo.iMinor == iMinor) && (updInfo.iBuild == iBuild)
             && ((llCheck == -1) || (QDateTime::currentDateTime().toTime_t() < llCheck)))
         {
            bDispl = false;
         }

         if (bDispl)
         {
            QString content = tr("There is the new version %1 of %2 available.<br />Click %3 to download!")
                  .arg(updInfo.sVersion)
                  .arg(pCustomization->strVal("APP_NAME"))
                  .arg(pHtml->link(updInfo.sUrl, tr("here")));

            updNotifier.setUpdateData(pHtml->htmlPage(content, "Update Info"), updInfo);
            updNotifier.exec();
         }
      }
      else
      {
         mInfo(tr("Program up to date!"));
      }
   }
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
   bool updDone = false;

   // is actual showinfo still actual ?
   if (!CSmallHelpers::inBetween(showInfo.starts(), showInfo.ends(), (uint)ulArcGmt))
   {
      if (showInfo.showType() == ShowInfo::Archive)
      {
         if (!showInfo.autoUpdate(ulArcGmt))
         {
            // update done ...
            updDone = true;
         }
      }
      else if (showInfo.showType() == ShowInfo::Live)
      {
         // does this channel have EPG ... ?
         if (showInfo.starts() && showInfo.ends())
         {
            cparser::SChan chanEntry;

            // get channel entry from channel map ...
            if (!pChanMap->entry(showInfo.channelId(), chanEntry))
            {
               // has chan entry already updated data ... ?
               if (CSmallHelpers::inBetween(chanEntry.uiStart, chanEntry.uiEnd, (uint)ulArcGmt))
               {
                  // channel map is updated already -> take these data to update show info ...
                  showInfo.updWithChanEntry(ulArcGmt, chanEntry);

                  // update done ...
                  updDone = true;
               }
               else
               {
                  // channel map wasn't updated so far -> request update ...
                  if ((showInfo.epgUpdTime() + EPG_UPD_TMOUT) < QDateTime::currentDateTime().toTime_t())
                  {
                     // set timestamp ...
                     showInfo.setEpgUpdTime(QDateTime::currentDateTime().toTime_t());

                     // make sure that next channel list update doesn't come to quick ...
                     Refresh.start(60000);

                     // update channel list ...
                     slotUpdateChannelList();
                  }
               }
            }
         }
      }
   }

   // update done ... ?
   if (updDone)
   {
      // add additional info to LCD ...
      int     iTime = showInfo.ends() ? (int)((showInfo.ends() - showInfo.starts()) / 60) : 60;
      QString sTime = tr("Length: %1 min.").arg(iTime);
      ui->labState->setFooter(sTime);
      ui->labState->updateState(showInfo.playState());

      // set short epg info ...
      ui->textEpgShort->setHtml(showInfo.htmlDescr());

      // update show info in overlay display ...
      missionControl.setVideoInfo(QString("<b>%1:</b> %2").arg(showInfo.chanName()).arg(showInfo.showName()));

      // done ...
      emit sigShowInfoUpdated();
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
      int                 cid, curCid = -1, i;
      QString             fLogo;
      bool                bLoaded;
      QModelIndex         modIdx = ui->channelList->currentIndex();

      if(modIdx.isValid())
      {
         // get current selection ...
         curCid = pModel->itemFromIndex(modIdx)->data(channellist::cidRole).toInt();
      }

      for (i = 0; i < pModel->rowCount(); i++)
      {
         bLoaded = false;
         pItem   = pModel->item(i);
         cid     = pItem->data(channellist::cidRole).toInt();

         // is there a logo file ... ?
         if ((fLogo = pItem->data(channellist::logoFileRole).toString()) != "")
         {
            if (icon.load(fLogo))                     // auto type detection ...
            {
               bLoaded = true;
            }
            else if (icon.load(fLogo, "image/gif"))   // force gif load ...
            {
               bLoaded = true;
            }

            if (bLoaded)
            {
               pItem->setData(QIcon(icon), channellist::iconRole);

               // update channel icon on EPG browser ...
               if ((curCid != -1) && (cid == curCid))
               {
                  ui->labChanIcon->setPixmap(QIcon(icon).pixmap(42, 24));
               }
            }
         }
      }

      // mark logo stuff as completed ...
      ulStartFlags |= FLAG_CLOGO_COMPL;
   }
}

/* -----------------------------------------------------------------\
|  Method: slotPCodeChangeResp [slot]
|  Begin: 31.05.2012
|  Author: Jo2003
|  Description: parent code was changed ...
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

/* -----------------------------------------------------------------\
|  Method: slotAddFav [slot]
|  Begin: 14.06.2012
|  Author: Jo2003
|  Description: add one favourite
|
|  Parameters: favourite to add
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotAddFav(int cid)
{
    if (!lFavourites.contains(cid))
    {
        cparser::SChan chan;

        if (pChanMap->entry(cid, chan) == 0)
        {
            // add new favourite ...
            lFavourites.append(cid);
            Settings.addFavData(cid, chan.iPrimGrp, chan.sName, chan.sIcon);

            HandleFavourites();
        }
    }
}

/* -----------------------------------------------------------------\
|  Method: slotRestoreMinimized [slot]
|  Begin: 11.07.2012
|  Author: Jo2003
|  Description: restore window from minimized state
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotRestoreMinimized()
{
   setWindowState(windowState() & ~(Qt::WindowMinimized | Qt::WindowActive));
   show();
}

/* -----------------------------------------------------------------\
|  Method: slotDownStreamRequested
|  Begin: 10.08.2012
|  Author: Jo2003
|  Description: store stream download request id ...
|
|  Parameters: request id
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotDownStreamRequested (int id)
{
   iDwnReqId = id;
}

/* -----------------------------------------------------------------\
|  Method: slotToggleFullscreen [slot]
|  Begin: 27.04.2012
|  Author: Jo2003
|  Description: toggle fullscreen
|               (hopefully also Mac Snow Leopard likes this)
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotToggleFullscreen()
{
   setDisplayMode(Ui::DM_FULLSCREEN);
}

//---------------------------------------------------------------------------
//
//! \brief   toggle windowed mode [slot]
//
//! \author  Jo2003
//! \date    13.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotWindowed()
{
   setDisplayMode(Ui::DM_WINDOWED);
}

/* -----------------------------------------------------------------\
|  Method: slotUpdateChannelList [slot]
|  Begin: 04.12.2012
|  Author: Jo2003
|  Description: update channel channel list ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotUpdateChannelList (const QList<int> &cidList)
{
   QStandardItem  *pItem;
   int             cid, i, iPos;
   cparser::SChan  chanEntry;
   QStringList     updChannels;
   uint            now = QDateTime::currentDateTime().toTime_t();

   for (i = 0; i < pModel->rowCount(); i++)
   {
      pItem = pModel->item(i);
      cid   = pItem->data(channellist::cidRole).toInt();

      // is this a channel entry ?
      if (cid != -1)
      {
         // update chan list item ...
         if (!pChanMap->entry(cid, chanEntry))
         {
            if (cidList.isEmpty())
            {
               // no new shows, simply update progress position ...
               iPos = (int)(now - (chanEntry.uiStart + chanEntry.iTs));

               // update progress ...
               pItem->setData(iPos, channellist::posRole);

               // check if this channel needs an update ...
               if ((chanEntry.uiEnd > 0) && (now > (chanEntry.uiEnd + chanEntry.iTs)))
               {
                  // when we lately tried to update EPG for this ... ?
                  if ((pItem->data(channellist::lastEpgUpd).toUInt() + EPG_UPD_TMOUT) < now)
                  {
                     // longer than 120 seconds ago ...
#ifdef RADIO_OFFSET
                     if(cid < RADIO_OFFSET)  // never update radio channels ...
#endif
                     {
                        updChannels << QString::number(cid);

                        // set last epg update request time ...
                        pItem->setData(now, channellist::lastEpgUpd);
                     }
                  }
               }
            }
            else if (cidList.contains(cid))
            {
               // update whole list entry ...
               iPos = (int)(now - chanEntry.uiStart);

               pItem->setData(iPos,                                channellist::posRole);
               pItem->setData(chanEntry.uiStart + chanEntry.iTs,   channellist::startRole);
               pItem->setData(chanEntry.uiEnd   + chanEntry.iTs,   channellist::endRole);
               pItem->setData(chanEntry.sProgramm,                 channellist::progRole);
               pItem->setData(chanEntry.sName,                     channellist::nameRole);
            }
         }
      }
   }

   if (!updChannels.isEmpty())
   {
      pApiClient->queueRequest(CIptvDefs::REQ_EPG_CURRENT, updChannels.join(","));
   }
}

/* -----------------------------------------------------------------\
|  Method: slotRecordTimerEnded [slot]
|  Begin: 10.12.2012
|  Author: Jo2003
|  Description: stop record without asking questions
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotRecordTimerEnded()
{
   // end pending record ...
   if ((showInfo.playState () == IncPlay::PS_RECORD) && (showInfo.showType() != ShowInfo::VOD))
   {
      mInfo(tr("Record timer timed out ... end pending record!"));
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

      emit sigWLRecEnded();
   }
}

/* -----------------------------------------------------------------\
|  Method: slotGlobalError [slot]
|  Begin: 17.03.2013
|  Author: Jo2003
|  Description: show a message box with info
|
|  Parameters: iType -> type of message
|              sCaption -> title of message box
|              sDescr -> text in message box
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotGlobalError (int iType, const QString& sCaption, const QString& sDescr)
{
   switch ((Msg::eMsgType)iType)
   {
   case Msg::Info:
      QMessageBox::information(this, sCaption, sDescr);
      break;
   case Msg::Warning:
      QMessageBox::warning(this, sCaption, sDescr);
      break;
   case Msg::Error:
      QMessageBox::critical(this, sCaption, sDescr);
      break;
   default:
      mInfo(tr("Unknown Message: %1").arg(sDescr));
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: slotTriggeredLogout [slot]
|  Begin: 02.07.2013
|  Author: Jo2003
|  Description: triggered / timed logout, function should be called
|               by timer
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotTriggeredLogout()
{
   // logout from kartina ...
   pApiClient->queueRequest (CIptvDefs::REQ_LOGOUT);
}

//---------------------------------------------------------------------------
//
//! \brief   filter channel list to match filter string
//
//! \author  Jo2003
//! \date    29.07.2013
//
//! \param   filter (QString) filter string
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotFilterChannelList(QString filter)
{
    pFilterMenu->hide();

    ui->pushFilter->setIcon(QIcon(filter.isEmpty() ? ":/app/filter" : ":/app/act_filter"));
    FillChannelList(pChanMap->filterChannels(filter));
}

//---------------------------------------------------------------------------
//
//! \brief   filter channel list for language
//
//! \author  Jo2003
//! \date    29.07.2013
//
//! \param   pAct (QAction *) pointer to action activated
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotLangFilterChannelList(QAction *pAct)
{
   // get a list of available actions ...
   QList<QAction *> actList = pMnLangFilter->actions();
   QList<QAction *>::const_iterator cit;

   for (cit = actList.constBegin(); cit != actList.constEnd(); cit ++)
   {
      // uncheck all non active languages ...
      if (pAct != (*cit))
      {
         (*cit)->setChecked(false);
      }
   }

   if (pAct->data().toString().isEmpty())
   {
      pApiClient->queueRequest(CIptvDefs::REQ_CHANNELLIST);
   }
   else
   {
      pApiClient->queueRequest(CIptvDefs::REQ_CL_LANG, pAct->data().toString());
   }

   // requested list is not filtered for channel names ...
   pFilterWidget->cleanFilter();

   ui->pushFilter->setIcon(QIcon(pAct->data().toString().isEmpty() ? ":/app/filter" : ":/app/act_filter"));

   pFilterMenu->hide();
}

//---------------------------------------------------------------------------
//
//! \brief   server response for available audio streams
//
//! \author  Jo2003
//! \date    29.07.2013
//
//! \param   str (QString) server response with audio streams
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotALang(const QString &str)
{
#ifdef _TASTE_IPTV_RECORD
   QStringList  sl;
   QAction     *pAct;
   int          i;

   if (!pApiParser->parseAStreams(str, sl))
   {
      pMnLangFilter->clear();

      // add "all" entry ...
      pAct = pMnLangFilter->addAction(tr("All"));

      pAct->setData(QString(""));
      pAct->setCheckable(true);
      pAct->setChecked(true);

      for (i = 0; i < sl.count(); i++)
      {
         pAct = pMnLangFilter->addAction(sl.at(i));
         pAct->setData(sl.at(i));
         pAct->setCheckable(true);
      }
   }
#else
   Q_UNUSED(str)
#endif // _TASTE_IPTV_RECORD
}

//---------------------------------------------------------------------------
//
//! \brief   server response for available VOD languages
//
//! \author  Jo2003
//! \date    28.01.2014
//
//! \param   str (QString) server response with VOD languages
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotVodLang(const QString &str)
{
   QVodLangMap lMap;

   if (!pApiParser->parseVodLang(str, lMap))
   {
      ui->cbxVodLang->clear();

      // fill vod lang combobox ...
      QVodLangMap::const_iterator cit;
      for (cit = lMap.constBegin(); cit != lMap.constEnd(); cit++)
      {
         ui->cbxVodLang->addItem(cit.key(), cit.value());
      }

      // request videos ...
      // trigger video load ...
      QUrl url;
      url.addQueryItem("type", ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString());
      url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
      url.addQueryItem("nums", "20");
      pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, QString(url.encodedQuery()));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   got channel group response; parse and request channel list [slot]
//
//
//! \param[in] str string response
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotChanGroups(const QString &str)
{
    QGrpMap chanGrps;

    if (!pApiParser->parseChannelGroups(str, chanGrps))
    {
        if (chanGrps.count() > 0)
        {
            Settings.setChanGrps(chanGrps);

            pChanMap->setGroupMap(chanGrps);

            QPixmap pix(16, 16);
            int     idx = 0, cnt = 0;
            int     lastGroup = Settings.lastChanGrp();

            // get last play info ...
            QString lastPlay  = Settings.lastPlay();

            // last play stuff ...
            if (!lastPlay.isEmpty())
            {
                mLastPlay = QtJson::parse(lastPlay).toMap();
                if (mLastPlay.value("gid").toInt() != -1)
                {
                    lastGroup = mLastPlay.value("gid").toInt();
                }
            }

            // either last group or first group in channel groups map ...
            if (!chanGrps.contains(lastGroup) && (lastGroup != DEF_FAV_GRP))
            {
                lastGroup = chanGrps.keys()[0];
            }

            ui->cbxChannelGroup->clear();

            if (Settings.favsAsChanGrp())
            {
                // add favorites ...
                pix.fill(QColor("#fc0"));
                ui->cbxChannelGroup->addItem(QIcon(pix), tr("Favourites"), DEF_FAV_GRP);

                if (lastGroup == DEF_FAV_GRP)
                {
                    idx = cnt;
                }

                cnt ++;
            }

            foreach(const cparser::SGrp& grp, chanGrps)
            {
                if (!Settings.hiddenGroup(grp.iId))
                {
                    if (lastGroup == grp.iId)
                    {
                        idx = cnt;
                    }

                    // add channel group entry ...
                    pix.fill(QColor(grp.sColor));
                    ui->cbxChannelGroup->addItem(QIcon(pix), grp.sName, grp.iId);

                    cnt ++;
                }
            }

            ui->cbxChannelGroup->setCurrentIndex(idx);
            on_cbxChannelGroup_activated(idx);

            // pApiClient->queueRequest(CIptvDefs::REQ_CHANNELLIST, lastGroup);
        }
    }
}

//---------------------------------------------------------------------------
//
//! \brief   update overlay number for watchlist button icon [slot]
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotUpdWatchListCount()
{
   QOverlayIcon ico(":/app/watchlist");
   ico.placeInt(pWatchList->count(), QColor("#f00"));
   ui->pushWatchList->setIcon(ico);
}

// ------------------------------------------------------------------
/// \brief Recorder::slotRowsInserted
/// \param parent
/// \param start inserted index start
/// \param end inserted index end
// ------------------------------------------------------------------
void Recorder::slotRowsInserted(const QModelIndex &parent, int start, int end)
{
#ifdef __TRACE
    mInfo(tr("parent: %1, start: %2, end: %3").arg(parent.row()).arg(start).arg(end));
#else
    Q_UNUSED(end)
    Q_UNUSED(parent)
#endif
    mFavDroppedIdx = start;
}

// ------------------------------------------------------------------
/// \brief Recorder::slotRowsRemoved
/// \param parent
/// \param start removed index start
/// \param end removed index end
// ------------------------------------------------------------------
void Recorder::slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
#ifdef __TRACE
    mInfo(tr("parent: %1, start: %2, end: %3").arg(parent.row()).arg(start).arg(end));
#else
    Q_UNUSED(end)
    Q_UNUSED(parent)
#endif
    mFavDragIdx = start;

    // time to make the right selection ...
    if (mFavDroppedIdx > mFavDragIdx)
    {
        // when moving down the channel, we have to reduce
        // the new row count by 1 since channel was removed
        // from above
        mFavDroppedIdx --;
    }

    QModelIndex midx = ui->channelList->model()->index(mFavDroppedIdx, 0);
    ui->channelList->setCurrentIndex(midx);

    // re-create favorites stuff ...
    lFavourites.clear();
    for (int i = 0; i < pModel->rowCount(); i++)
    {
        lFavourites.append(pModel->item(i)->data(channellist::cidRole).toInt());
    }

    HandleFavourites();
}

//---------------------------------------------------------------------------
//
//! \brief   update stay on top mode [slot]
//
//! \author  Jo2003
//! \date    29.11.2013
//
//! \param   on (bool) enable or disable stay on top
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotStayOnTop(bool on)
{
   bStayOnTop = on;

   if (eCurDMode == Ui::DM_WINDOWED)
   {
      if (bStayOnTop)
      {
         setWindowFlags(windowFlags() |  Qt::WindowStaysOnTopHint);
      }
      else
      {
         setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
      }

      show();
   }
}

//---------------------------------------------------------------------------
//! \brief   remove fav from fav list [slot]
//! \param   cid int channel id
//---------------------------------------------------------------------------
void Recorder::slotRemoveFav(int cid)
{
    if (lFavourites.contains(cid))
    {
        // remove favourite ...
        lFavourites.removeOne(cid);
        Settings.addFavData(cid, 0, "", "");

        HandleFavourites();

        if (getCurrentGid() == DEF_FAV_GRP)
        {
            for (int i = 0; i < pModel->rowCount(); i++)
            {
                if (pModel->item(i)->data(channellist::cidRole).toInt() == cid)
                {
                    pModel->removeRow(i);
                    break;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
//! \brief   activate favourite [slot]
//! \param   cid int channel id
//---------------------------------------------------------------------------
void Recorder::activateFav(int cid)
{
    int     gid;
    QString name, logo;

    Settings.favData(cid, gid, name, logo);

    // fav channel which should be marked ...
    miMarkFavChan = cid;

    // mark TV group ... triggers load of channel list as well ...
    on_cbxChannelGroup_activated(ui->cbxChannelGroup->findData(gid));
}

//---------------------------------------------------------------------------
//! \brief   open reorder dialog [slot]
//---------------------------------------------------------------------------
void Recorder::reorderFavs()
{
    CFavItemModel  model;
    QStandardItem* pItem;

    foreach (CFavButton* pBtn, vFavourites)
    {
        pItem = new QStandardItem(pBtn->icon(), pBtn->name());
        pItem->setData(pBtn->cid());
        model.appendRow(pItem);
    }

    CFavSortDlg *pSort = new CFavSortDlg(this, &model);

    if (pSort)
    {
        if (pSort->exec() == QDialog::Accepted)
        {
            // do sort ...
            lFavourites.clear();

            for (int i = 0; i < model.rowCount(); i++)
            {
                pItem = model.item(i, 0);

                lFavourites.append(pItem->data().toInt());
            }

            if (getCurrentGid() == DEF_FAV_GRP)
            {
                on_cbxChannelGroup_activated(0);
            }

            HandleFavourites();
        }
    }
}

//---------------------------------------------------------------------------
/// \brief rebuild Favs due to resizing ...
//---------------------------------------------------------------------------
void Recorder::rebuildFavs(QSize oldSz, QSize visibleSize)
{
    if (ui->gLayoutFav->count())
    {
        QSize buttonSize     = ui->gLayoutFav->itemAt(0)->widget()->size();
        int   scrollBarWidth = ui->scrollArea->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

        if (ui->scrollArea->verticalScrollBar()->isVisible())
        {
            visibleSize.setWidth(visibleSize.width() + scrollBarWidth);
        }

        // debounce ...
        if (std::abs(mFavWidgetWidth - visibleSize.width()) > 5)
        {
            mFavWidgetWidth = visibleSize.width();

            int inRow = (visibleSize.width() - 2) / (buttonSize.width() + 2);

            if (inRow < 2)
            {
                inRow = 2;
            }

            int rowCount = ui->gLayoutFav->count() / inRow;

            if (ui->gLayoutFav->count() % inRow)
            {
                rowCount++;
            }

            int minHeight = (rowCount * (buttonSize.height() + 2)) + 2;

            if (minHeight > visibleSize.height())
            {
                // we need a scrollbar ...
                inRow    = (visibleSize.width() - 2 - scrollBarWidth) / (buttonSize.width() + 2);
            }

            if (inRow < 2)
            {
                inRow = 2;
            }

            if (inRow != miFavsInRow)
            {
                // col count changed -> handel favorites ...
                miFavsInRow = inRow;
                HandleFavourites();
            }

#ifdef __TRACE
            mInfo(tr("\n\nViewport: %1px x %2px\nOld Event: %5px x %6px\nButton: %7px x %8px, in Row: %9, Row Count: %10")
                  .arg(visibleSize.width())
                  .arg(visibleSize.height())
                  .arg(oldSz.width())
                  .arg(oldSz.height())
                  .arg(buttonSize.width())
                  .arg(buttonSize.height())
                  .arg(inRow)
                  .arg(rowCount));
#else
            Q_UNUSED(oldSz)
#endif //  __TRACE
        }
    }
}

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
      {tr("Play / Pause"),         this,       SLOT(slotPlay()),                  "ALT+P"},
      {tr("Stop"),                 this,       SLOT(slotStop()),                  "ALT+S"},
      {tr("Record"),               this,       SLOT(slotRecord()),                "ALT+R"},
      {tr("Timer Record"),         this,       SLOT(on_pushTimerRec_clicked()),   "ALT+T"},
      {tr("Settings"),             this,       SLOT(on_pushSettings_clicked()),   "ALT+O"},
      {tr("About"),                this,       SLOT(on_pushAbout_clicked()),      "ALT+I"},
      {tr("Search EPG"),           this,       SLOT(on_btnSearch_clicked()),      "CTRL+F"},
      {tr("Text Size +"),          this,       SLOT(on_btnFontLarger_clicked()),  "ALT++"},
      {tr("Text Size -"),          this,       SLOT(on_btnFontSmaller_clicked()), "ALT+-"},
      {tr("Quit"),                 this,       SLOT(close()),                     "ALT+Q"},
      {tr("Toggle Aspect Ratio"),  ui->player, SLOT(slotToggleAspectRatio()),     "ALT+A"},
      {tr("Toggle Crop Geometry"), ui->player, SLOT(slotToggleCropGeometry()),    "ALT+C"},
      {tr("Toggle Fullscreen"),    this,       SLOT(slotToggleFullscreen()),      "ALT+F"},
      {tr("Volume +"),             ui->player, SLOT(slotMoreLoudly()),            "+"},
      {tr("Volume -"),             ui->player, SLOT(slotMoreQuietly()),           "-"},
      {tr("Toggle Mute"),          ui->player, SLOT(slotMute()),                  "M"},
      {tr("Jump Forward"),         this,       SLOT(slotFwd()),                   "CTRL+ALT+F"},
      {tr("Jump Backward"),        this,       SLOT(slotBwd()),                   "CTRL+ALT+B"},
      {tr("Screenshot"),           ui->player, SLOT(slotTakeScreenShot()),        "F12"},
      {tr("Minimal Interface"),    this,       SLOT(slotWindowed()),              "F11"},
      {tr("Next Channel"),         this,       SLOT(slotChannelDown()),           "CTRL+N"},
      {tr("Previous Channel"),     this,       SLOT(slotChannelUp()),             "CTRL+P"},
      {tr("Play Next Channel"),    this,       SLOT(slotPlayNextChannel()),       "CTRL+ALT+N"},
      {tr("Play Prev. Channel"),   this,       SLOT(slotPlayPreviousChannel()),   "CTRL+ALT+P"},
      {tr("Show EPG / VOD"),       this,       SLOT(slotToggleEpgVod()),          "CTRL+E"},
      {tr("Watch List"),           this,       SLOT(on_pushWatchList_clicked()),  "ALT+W"},
      {tr("Help"),                 this,       SLOT(on_pushHelp_clicked()),       "F1"},
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
   bool ok;

   // -------------------------------------------
   // create epg nav bar ...
   // -------------------------------------------
   TouchEpgNavi (true);

   // -------------------------------------------
   // create systray ...
   // -------------------------------------------
   CreateSystray();

   // set language as read ...
   //! since language change in settings triggers a reload, no need to do it here ...
   // pAppTransl->load(QString("lang_%1").arg(Settings.GetLanguage ()),
   //                   pFolders->getLangDir());
   // pQtTransl->load(QString("qt_%1").arg(Settings.GetLanguage ()),
   //                   pFolders->getQtLangDir());

   // get player module ...
   vlcCtrl.LoadPlayerModule(Settings.GetPlayerModule());

   // -------------------------------------------
   // set last windows size / position ...
   // -------------------------------------------
   if (Settings.getGeometry().size() > 0)
   {
      restoreGeometry(Settings.getGeometry());
   }
   else
   {
      // delete old values ...
      pDb->removeSetting("WndRect");
      pDb->removeSetting("WndState");
      pDb->removeSetting("IsMaximized");
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
      ui->hSplitterPlayer->setSizes(sSplit);
   }

   sSplit = Settings.GetSplitterSizes("spHFavs", &ok);
   if (ok)
   {
      ui->favSplitter->setSizes(sSplit);
   }

   // display splash screen ...
   if (!Settings.DisableSplashScreen())
   {
      QTimer::singleShot(1500, this, SLOT(slotSplashScreen()));
   }

   // init short cuts ...
   fillShortCutTab();
   InitShortCuts ();

   // check for program updates ...
   pApiClient->queueRequest(CIptvDefs::REQ_UPDATE_CHECK, pCustomization->strVal("UPD_CHECK_URL"));

   // update watch list count ...
   slotUpdWatchListCount();
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
   Settings.setGeometry(saveGeometry());
   Settings.SaveSplitterSizes("spVChanEpg", ui->vSplitterChanEpg->sizes());
   Settings.SaveSplitterSizes("spVChanEpgPlay", ui->vSplitterChanEpgPlay->sizes());
   Settings.SaveSplitterSizes("spHPlay", ui->hSplitterPlayer ->sizes());
   Settings.SaveSplitterSizes("spHFavs", ui->favSplitter ->sizes());
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
   trayIcon.setIcon(QIcon("branding:main/icon"));
   trayIcon.setToolTip(pCustomization->strVal("APP_NAME"));

   // create context menu for tray icon ...
   QMenu *pShowMenu = new QMenu (this);
   pShowMenu->addAction(QIcon(":/app/restore"),
                        tr("&restore %1").arg(pCustomization->strVal("APP_NAME")),
                        this, SLOT(slotRestoreMinimized()));

   trayIcon.setContextMenu(pShowMenu);
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
#ifdef _TASTE_IPTV_RECORD
        ui->cbxLastOrBest->addItem(tr("Newest"), "id");
        ui->cbxLastOrBest->addItem(tr("By Name"), "name");
#else
        ui->cbxLastOrBest->addItem(tr("Newest"), "last");
        ui->cbxLastOrBest->addItem(tr("Best"), "best");
#endif // _TASTE_IPTV_RECORD
        ui->cbxLastOrBest->addItem(tr("My Favourites"), "vodfav");
        ui->cbxLastOrBest->setCurrentIndex(0);
    }
    else
    {
        int idx;

#ifdef _TASTE_IPTV_RECORD
        if ((idx = ui->cbxLastOrBest->findData("id")) > -1)
        {
            ui->cbxLastOrBest->setItemText(idx, tr("Newest"));
        }

        if ((idx = ui->cbxLastOrBest->findData("name")) > -1)
        {
            ui->cbxLastOrBest->setItemText(idx, tr("By Name"));
        }
#else
        if ((idx = ui->cbxLastOrBest->findData("last")) > -1)
        {
            ui->cbxLastOrBest->setItemText(idx, tr("Newest"));
        }

        if ((idx = ui->cbxLastOrBest->findData("best")) > -1)
        {
            ui->cbxLastOrBest->setItemText(idx, tr("Best"));
        }
#endif // _TASTE_IPTV_RECORD
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
    QString   sLine;
    QString   sLogoFile;
    QStandardItem *pItem;
    bool        bMissingIcon = false;
    QFileInfo   fInfo;
    QPixmap     icon;
    int         iChanCount =  0;
    int         iPos;
    uint        now = QDateTime::currentDateTime().toTime_t();
    int         favChanIdx =  0; // default to first entry

    if ((miMarkFavChan == -1) && !mLastPlay.isEmpty())
    {
        // double use of channel marker ...
        miMarkFavChan = mLastPlay.value("cid").toInt();
    }

    // ui->cbxChannelGroup->clear();
    pModel->clear();

    // any channel stored from former session ... ?
    if (!(ulStartFlags & FLAG_CHAN_LIST))
    {
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
                if (!icon.load(sLogoFile))                   // try auto type detection ...
                {
                    // try to force load with type from file extension ...
                    if (!icon.load(sLogoFile, QString("image/%1").arg(fInfo.suffix()).toUtf8().constBegin()))
                    {
                        // can't load --> load default image ...
                        icon.load(":png/no_logo");

                        mInfo(tr("Can't load channel image \"%1\" ...").arg(sLogoFile));

                        // delete logo file ...
                        QFile::remove(sLogoFile);

                        // enqueue pic to cache ...
                        pixCache.enqueuePic(chanlist[i].sIcon, pFolders->getLogoDir());

                        // mark for reload ...
                        bMissingIcon = true;
                    }
                }
            }

            // progress position ...
            iPos = (int)(QDateTime::currentDateTime().toTime_t() - (chanlist[i].uiStart + chanlist[i].iTs));

            // insert data ...
            pItem->setData(chanlist[i].iId,                       channellist::cidRole);
            pItem->setData(sLine,                                 channellist::nameRole);
            pItem->setData(QIcon(icon),                           channellist::iconRole);
            pItem->setData(chanlist[i].sProgramm,                 channellist::progRole);
            pItem->setData(chanlist[i].uiStart + chanlist[i].iTs, channellist::startRole);
            pItem->setData(chanlist[i].uiEnd   + chanlist[i].iTs, channellist::endRole);
            pItem->setData(sLogoFile,                             channellist::logoFileRole);
            pItem->setData(iPos,                                  channellist::posRole);
            pItem->setData(now,                                   channellist::lastEpgUpd);
            pItem->setData(chanlist[i].iPrimGrp,                  channellist::gidRole);

            pModel->appendRow(pItem);

            if (miMarkFavChan != -1)
            {
                if (miMarkFavChan == chanlist[i].iId)
                {
                    favChanIdx = i;
                }
            }
        } // not hidden ...
    }

    if (!bMissingIcon)
    {
        ulStartFlags |= FLAG_CLOGO_COMPL;
    }
    else
    {
        if (ulStartFlags & FLAG_CLOGO_COMPL)
        {
            ulStartFlags ^= FLAG_CLOGO_COMPL;
        }
    }

    // reset marker ...
    miMarkFavChan = -1;

    ui->channelList->setCurrentIndex(pModel->index(favChanIdx, 0));
    ui->channelList->scrollTo(pModel->index(favChanIdx, 0));

    return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   create file name for record
//
//! \author  Jo2003
//! \date    18.12.2013
//
//! \param   name (const QString&) name to use in file name
//! \param   ext (const QString&) optional file extension
//
//! \return  record file name (incl. path)
//---------------------------------------------------------------------------
QString Recorder::recFileName (const QString& name, QString &ext)
{
   QDateTime now      = QDateTime::currentDateTime();
   QString   fileName;

   // a good default ...
   ext = "ts";

   // should we ask for file name ... ?
   if (Settings.AskForRecFile())
   {
      // yes! Create file save dialog ...
      QString   sFilter;
      QString   sTarget  = QString("%1/%2(%3)").arg(Settings.GetTargetDir())
                          .arg(vlcCtrl.doTranslit() ? translit.CyrToLat(name, true) : name)
                          .arg(now.toString("yyyy-MM-dd__hh-mm"));

      fileName = QFileDialog::getSaveFileName(this, tr("Save Stream as"),
                 sTarget, QString("MPEG 4 Video (*.m4v);;Transport Stream (*.ts);;AVI File (*.avi)"),
                 &sFilter);

      if (fileName != "")
      {
         // which filter was used ... ?
         if (sFilter == "Transport Stream (*.ts)")
         {
            ext = "ts";
         }
         else if (sFilter ==  "AVI File (*.avi)")
         {
            ext = "avi";
         }
         else if (sFilter ==  "MPEG 4 Video (*.m4v)")
         {
            ext = "m4v";
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
                 .arg(vlcCtrl.doTranslit() ? translit.CyrToLat(name, true) : name)
                 .arg(now.toString("yyyy-MM-dd__hh-mm"));
   }

   return fileName;
}

//---------------------------------------------------------------------------
//
//! \brief   set group cbx
//
//! \author  Jo2003
//! \date    20.01.2016
//
//! \param   [in] grpIdx (int) group index
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::setChannelGroup (int grpIdx)
{
    if (grpIdx > -1)
    {
        for (int i = 0; i < ui->cbxChannelGroup->count(); i++)
        {
            if (ui->cbxChannelGroup->itemData(i).toInt() == grpIdx)
            {
                ui->cbxChannelGroup->setCurrentIndex(i);
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------
//
//! \brief   dump channel list
//
//! \author  Jo2003
//! \date    16.02.2016
//
//! \param   [in] cl (const QChanList &) ref. to channel list
//
//---------------------------------------------------------------------------
void Recorder::dumpChanList(const QChanList &cl)
{
    QString     sLog;
    QTextStream ts(&sLog);
    foreach (const cparser::SChan& chan, cl)
    {
        if (chan.bIsGroup)
        {
            ts << "============ " << chan.sName << " ============" << endl;
        }
        else
        {
            ts << chan.sName << " (" << chan.iId << "), Video: "
               << chan.bIsVideo << ", hidden: " << chan.bIsHidden
               << ", protected: " << chan.bIsProtected << endl;
        }
    }

    mInfo(tr("Parsed Channel List:\n===8<===\n%1===8<===").arg(sLog));
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
   QString     sExt;
   QString     fileName = recFileName(sChannel, sExt);
   QString     sCmdLine;

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

         // update show info in overlay display ...
         if (showInfo.showType() == ShowInfo::VOD)
         {
             missionControl.setVideoInfo(QString("<b>%1:</b> %2").arg(tr("Video On Demand")).arg(showInfo.showName()));
         }
         else
         {
            missionControl.setVideoInfo(QString("<b>%1:</b> %2").arg(showInfo.chanName()).arg(showInfo.showName()));
         }

         vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState);
      }

      // successfully started ?
      if (!vlcpid)
      {
         iRV = -1;
         QMessageBox::critical(this, tr("Error!"), tr("Can't start player!"));
         ePlayState = IncPlay::PS_ERROR;
         TouchPlayCtrlBtns();
      }
      else
      {
         iRV = 0;
         mInfo(tr("Started player with pid #%1!").arg((uint)vlcpid));
      }
   }
   else
   {
      // no filename given -> set to stop ...
      ePlayState = IncPlay::PS_STOP;
      TouchPlayCtrlBtns();
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

      // update show info in overlay display ...
      if (showInfo.showType() == ShowInfo::VOD)
      {
          missionControl.setVideoInfo(QString("<b>%1:</b> %2").arg(tr("Video On Demand")).arg(showInfo.showName()));
      }
      else
      {
         missionControl.setVideoInfo(QString("<b>%1:</b> %2").arg(showInfo.chanName()).arg(showInfo.showName()));
      }

      vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState);
   }

   // successfully started ?
   if (!vlcpid)
   {
      iRV = -1;
      QMessageBox::critical(this, tr("Error!"), tr("Can't start player!"));
      ePlayState = IncPlay::PS_ERROR;
      TouchPlayCtrlBtns();
   }
   else
   {
      mInfo(tr("Started player with pid #%1!").arg((uint)vlcpid));
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
   QString sExt;
   QString fileName = recFileName(sName, sExt);

   if (fileName != "")
   {
      ui->textEpgShort->setHtml(showInfo.htmlDescr());

      // update show info in overlay display ...
      if (showInfo.showType() == ShowInfo::VOD)
      {
         missionControl.setVideoInfo(QString("<b>%1:</b> %2").arg(tr("Video On Demand")).arg(showInfo.showName()));
      }
      else
      {
         missionControl.setVideoInfo(QString("<b>%1:</b> %2").arg(showInfo.chanName()).arg(showInfo.showName()));
      }

      streamLoader.downloadStream (sURL, QString("%1.%2").arg(fileName).arg(sFileExt),
                                   Settings.GetBufferTime ());
   }
   else
   {
      // no filename given -> set to stop ...
      ePlayState = IncPlay::PS_STOP;
      TouchPlayCtrlBtns();
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
   if (vlcCtrl.withLibVLC())
   {
      if ((showInfo.playState() == IncPlay::PS_PLAY)
         && showInfo.canCtrlStream()
         && bEnable)
      {
         missionControl.enableBtn(true, QFusionControl::BTN_BWD);
         missionControl.enableBtn(true, QFusionControl::BTN_FWD);
         missionControl.enableJumpBox(true);
         missionControl.btnSetIcon(QIcon(":/app/pause"), QFusionControl::BTN_PLAY);
      }
      else
      {
         missionControl.enableBtn(false, QFusionControl::BTN_BWD);
         missionControl.enableBtn(false, QFusionControl::BTN_FWD);
         missionControl.enableJumpBox(false);
         missionControl.btnSetIcon(QIcon(":/app/play"), QFusionControl::BTN_PLAY);
      }
   }
   else
   {
      missionControl.enableBtn(false, QFusionControl::BTN_BWD);
      missionControl.enableBtn(false, QFusionControl::BTN_FWD);
      missionControl.enableJumpBox(false);
   }

   // switch icon of record button to make visible record timer stuff ...
   if ((showInfo.playState() == IncPlay::PS_RECORD) && (showInfo.showType() != ShowInfo::VOD))
   {
      missionControl.btnSetIcon(QIcon(":/app/record_timer"), QFusionControl::BTN_REC);
   }
   else
   {
      missionControl.btnSetIcon(QIcon(":/app/record"), QFusionControl::BTN_REC);
   }

   switch (ePlayState)
   {
   case IncPlay::PS_PLAY:
      missionControl.enableBtn(bEnable, QFusionControl::BTN_PLAY);
      missionControl.enableBtn(bEnable, QFusionControl::BTN_REC);
      missionControl.enableBtn(bEnable, QFusionControl::BTN_STOP);
      ui->pushLive->setEnabled(bEnable);
      break;

   case IncPlay::PS_RECORD:
      missionControl.enableBtn(false, QFusionControl::BTN_PLAY);
      missionControl.enableBtn(bEnable, QFusionControl::BTN_REC);
      missionControl.enableBtn(bEnable, QFusionControl::BTN_STOP);
      ui->pushLive->setEnabled(false);
      break;

   case IncPlay::PS_TIMER_RECORD:
   case IncPlay::PS_TIMER_STBY:
      missionControl.enableBtn(false, QFusionControl::BTN_PLAY);
      missionControl.enableBtn(false, QFusionControl::BTN_REC);
      ui->pushLive->setEnabled(false);
      missionControl.enableBtn(bEnable, QFusionControl::BTN_STOP);
      break;

   default:
      missionControl.enableBtn(bEnable, QFusionControl::BTN_PLAY);
      missionControl.enableBtn(bEnable, QFusionControl::BTN_REC);
      ui->pushLive->setEnabled(bEnable);
      missionControl.enableBtn(false, QFusionControl::BTN_STOP);
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
   if (QMessageBox::question(this, tr("Question"),
                             pHtml->htmlPage(tr("Pending Record!<br /> <br />Do you really want to stop recording now?")),
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
    int            i, gid;
    QPixmap        pic;
    QIcon          ico;
    QFileInfo      fInfo;
    QString        sLogo, sName;
    CFavButton*    pBtn;

    Ui::FavVector_t::iterator it;

    for (it = vFavourites.begin(); it != vFavourites.end();)
    {
        pBtn = *it;
        ui->gLayoutFav->removeWidget(pBtn);
        disconnect(pBtn, SIGNAL(clicked(int)) , this, SLOT(activateFav(int)));
        disconnect(pBtn, SIGNAL(deleteMe(int)), this, SLOT(slotRemoveFav(int)));
        disconnect(pBtn, SIGNAL(reorder())    , this, SLOT(reorderFavs()));

        // mark for deletion ...
        pBtn->deleteLater();
        it = vFavourites.erase(it);
    }

    // re-create all buttons ...
    for (i = 0; i < lFavourites.count(); i++)
    {
        Settings.favData(lFavourites[i], gid, sName, sLogo);
        fInfo.setFile(sLogo);

        // logo ...
        pic.load(QString("%1/%2").arg(pFolders->getLogoDir()).arg(fInfo.fileName()));
        ico = QIcon(pic); // .scaled(34, 34, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        if ((pBtn = new CFavButton(ico, sName, this, lFavourites[i])) != NULL)
        {
            connect(pBtn, SIGNAL(clicked(int)) , this, SLOT(activateFav(int)));
            connect(pBtn, SIGNAL(deleteMe(int)), this, SLOT(slotRemoveFav(int)));
            connect(pBtn, SIGNAL(reorder())    , this, SLOT(reorderFavs()));

            pBtn->setToolTip(sName);
            pBtn->setStyleSheet(FAVBTN_STYLE);
            pBtn->setIconSize(QSize(38, 38));

            ui->gLayoutFav->addWidget(pBtn, i / miFavsInRow, i % miFavsInRow, Qt::AlignCenter);
            vFavourites.append(pBtn);
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

               // stop record timer ...
               timerWidget.stop();

               // make sure we stop record all as well ...
               pWatchList->slotStopRecAll();
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
   int         cid = idx.isValid() ? qvariant_cast<int>(idx.data(channellist::cidRole)) : -1;

   return cid;
}

//------------------------------------------------------------------
/// \brief get current group id
/// \return group id
//------------------------------------------------------------------
int Recorder::getCurrentGid()
{
    return ui->cbxChannelGroup->itemData(ui->cbxChannelGroup->currentIndex()).toInt();
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
   // check how man days archive is available ...
   int iDays = -((MAX_ARCHIV_AGE / (24 * 3600 )) + 1);

   // up to 1 week into future ...
   if (iEpgOffset > 7)
   {
      iEpgOffset = 7;
   }
   else if (iEpgOffset < iDays)
   {
      iEpgOffset = iDays;
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

/* -----------------------------------------------------------------\
|  Method: toggleFullscreen
|  Begin: 11.07.2012
|  Author: Jo2003
|  Description: toggle fullscreen
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::toggleFullscreen()
{
    setWindowState(windowState() ^ Qt::WindowFullScreen);
    show();
}

//---------------------------------------------------------------------------
//
//! \brief   check if media url is a playlist (means HLS streaming)
//
//! \author  Jo2003
//! \date    13.12.2013
//
//! \param   sUrl (const QString&) url to check
//
//! \return  0 -> normal handling; 1 -> playlist / hls handling
//---------------------------------------------------------------------------
int Recorder::check4PlayList(const QString& sUrl, const QString &sName)
{
#ifndef __VLC_FOR_HLS ///< this should be defined in case VLC should handle HLS!
   if (sUrl.contains("m3u", Qt::CaseInsensitive))
   {
      // HLS .. Oops!

      // make sure to remove the ts which might be present ...

      QString s = sUrl;

      // there shouldn't ... but there is ...
      s.replace("http/ts://", "http://");

      if (sName.isEmpty())
      {
         // normal play ...
         pHlsControl->startHls(s, Settings.GetBufferTime() / 1000);
      }
      else
      {
         // record ...
         QString sExt;
         QString sFileName = recFileName(sName, sExt) + ".ts";
         pHlsControl->startHls(s, Settings.GetBufferTime() / 1000, sFileName);
      }

      showInfo.useHls(true);

      return 1;
   }
   else
#else
   // avoid warnings ...
   Q_UNUSED(sUrl)
   Q_UNUSED(sName)
#endif // __VLC_FOR_HLS
   {
      return 0;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   play or record from hls stream buffer
//
//! \author  Jo2003
//! \date    17.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::slotPlayHls(const QString &s)
{
   if (!timeRec.silentRec())
   {
      mInfo(tr("Playing file '%1' ...").arg(s));

      StartVlcPlay(s);
   }
   else
   {
      mInfo(tr("Don't show file '%1'' due to silent timer record ...").arg(s));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   switch between supported display modes
//!          (normal, fullscreen, windowed)
//
//! \author  Jo2003
//! \date    13.11.2013
//
//! \param   newMode (Ui::EDisplayMode) new display mode (toggle supported)
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::setDisplayMode(Ui::EDisplayMode newMode)
{
   bool touchFavs = false;

   // take care for toggle mode ...
   if (newMode == eCurDMode)
   {
      if (eCurDMode == Ui::DM_FULLSCREEN)
      {
         // fullscreen toggle -> restore previous mode ...
         newMode = eOldDMode;
      }
      else
      {
         // windowed toggle -> go into normal mode ...
         newMode = Ui::DM_NORMAL;
      }
   }
   else if (newMode == Ui::DM_FULLSCREEN)
   {
      // save mode before going to fullscreen to have the chance
      // to restore this mode ...
      eOldDMode = eCurDMode;
   }

   // something to do ... ?
   if (newMode != eCurDMode)
   {
      switch(eCurDMode)
      {
      //////////////////////////////////////////////////////////////////////////////
      /// go from fullscreen to normal or windowed mode ...
      //////////////////////////////////////////////////////////////////////////////
      case Ui::DM_FULLSCREEN:
         if (newMode == Ui::DM_NORMAL)
         {
            // remove videoWidget from stacked layout ...
            stackedLayout->removeWidget(pVideoWidget);

            // put mainframe back to stacked layout ...
            stackedLayout->addWidget(ui->masterFrame);
            ui->masterFrame->setVisible(true);

            // make sure main dialog is visible ...
            stackedLayout->setCurrentWidget(ui->masterFrame);

            // put videoWidget back into player widget ...
            ui->player->addAndEmbedVideoWidget();

            // reset videoWidgets local pointer ...
            pVideoWidget = NULL;

            // show normal ...
            toggleFullscreen();

            // hide overlay control ...
            emit sigFullScreenToggled(0);

            // make sure we restore the initial geometry ...
            setGeometry(rectBackup);

            touchFavs = true;
         }
         else if (newMode == Ui::DM_WINDOWED)
         {
            // show normal ...
            toggleFullscreen();

            // hide overlay control ...
            emit sigFullScreenToggled(0);

            // resize to initial player widget size ...
            setGeometry(playWndRect);

            // we reach windowed mode ...
            emit sigWindowed(1);

            if (bStayOnTop)
            {
               setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
               show();
            }
         }
         break;

      //////////////////////////////////////////////////////////////////////////////
      /// go from normal to fullscreen or windowed mode ...
      //////////////////////////////////////////////////////////////////////////////
      case Ui::DM_NORMAL:

         // collect old and new size ...
         rectBackup  = geometry();

         // not yet initialized ...
         if (playWndRect.isNull())
         {
            // try to let the player widget at its position ...
            QPoint topLeft(ui->player->getVideoWidget()->mapToGlobal(ui->player->getVideoWidget()->geometry().topLeft()));
            int xOffset = mapToGlobal(geometry().topLeft()).x() - mapToGlobal(frameGeometry().topLeft()).x();
            int yOffSet = mapToGlobal(geometry().topLeft()).y() - mapToGlobal(frameGeometry().topLeft()).y();

            // take care of window border ...
            topLeft.setY(topLeft.y() - yOffSet);
            topLeft.setX(topLeft.x() - xOffset);

            playWndRect.setTopLeft(topLeft);
            playWndRect.setSize(ui->player->getVideoWidget()->size());
         }

         // get videoWidget ...
         pVideoWidget = ui->player->getAndRemoveVideoWidget();

         // remove and hide master frame ...
         stackedLayout->removeWidget(ui->masterFrame);
         ui->masterFrame->setVisible(false);

         // add videoWidget to stacked layout ...
         stackedLayout->addWidget(pVideoWidget);

         // make sure videoWidget is the one we see ...
         stackedLayout->setCurrentWidget(pVideoWidget);

         if (newMode == Ui::DM_FULLSCREEN)
         {
            // make dialog fullscreen ...
            toggleFullscreen();

            // tell about ...
            emit sigFullScreenToggled(1);
         }
         else if (newMode == Ui::DM_WINDOWED)
         {
            // resize to initial player widget size ...
            setGeometry(playWndRect);

            // we reach windowed mode ...
            emit sigWindowed(1);

            if (bStayOnTop)
            {
               setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
               show();
            }
         }
         break;

      //////////////////////////////////////////////////////////////////////////////
      /// go from windowed mode to fullscreen or normal mode ...
      //////////////////////////////////////////////////////////////////////////////
      case Ui::DM_WINDOWED:

         // store current position and size ...
         playWndRect = geometry();

         // we leave windowed mode ...
         emit sigWindowed(0);

         // make sure we clear the stay on top stuff ...
         if (bStayOnTop)
         {
            setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
         }

         if (newMode == Ui::DM_NORMAL)
         {
            // remove videoWidget from stacked layout ...
            stackedLayout->removeWidget(pVideoWidget);

            // put mainframe back to stacked layout ...
            stackedLayout->addWidget(ui->masterFrame);
            ui->masterFrame->setVisible(true);

            // make sure main dialog is visible ...
            stackedLayout->setCurrentWidget(ui->masterFrame);

            // put videoWidget back into player widget ...
            ui->player->addAndEmbedVideoWidget();

            // reset videoWidgets local pointer ...
            pVideoWidget = NULL;

            // make sure we restore the initial geometry ...
            setGeometry(rectBackup);

            touchFavs = true;
         }
         else if (newMode == Ui::DM_FULLSCREEN)
         {
            // make dialog fullscreen ...
            toggleFullscreen();

            // tell about ...
            emit sigFullScreenToggled(1);
         }

         // make sure to show window after clearing the stay on top stuff ...
         if (bStayOnTop)
         {
            show();
         }
         break;

      default:
         break;
      }

      eCurDMode = newMode;
   }

   if (touchFavs)
   {
      // check fav area ...
      QTimer::singleShot(300, this, SLOT(rebuildFavs()));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   make sure to stop playback in case hls download is active
//
//! \author  Jo2003
//! \date    20.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void Recorder::stopOnDemand()
{
   if (pHlsControl->isActive())
   {
      if (vlcCtrl.withLibVLC())
      {
         ui->player->silentStop();
      }
      else
      {
         vlcCtrl.stop();
      }

      pHlsControl->stop();
   }
}

/************************* History ***************************\
| $Log$
\*************************************************************/
