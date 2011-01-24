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

#ifndef INCLUDE_LIBVLC
   #include "ui_recorder.h"
#else
   #include "ui_recorder_inc.h"
#endif

#include "chanlistwidgetitem.h"

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

   // set (customized) windows title ...
   setWindowTitle(QString("VLC-Recorder - %1").arg(COMPANY_NAME));

   ePlayState     = IncPlay::PS_WTF;
   bLogosReady    = false;
   pTranslator    = trans;
   iEpgOffset     = 0;
   iFontSzChg     = 0;
   iDwnReqId      = -1;
   bDoInitDlg     = true;
   bFirstConnect  = true;

   // init favourite buttons ...
   for (int i = 0; i < MAX_NO_FAVOURITES; i++)
   {
      pFavBtn[i]     = NULL;
      pFavAct[i]     = NULL;
      pContextAct[i] = NULL;
   }

   VlcLog.SetLogFile(pFolders->getDataDir(), APP_LOG_FILE);

   // set logo dir and host for chan logo downloader ...
   dwnLogos.setHostAndFolder(KARTINA_HOST, pFolders->getLogoDir());
   dwnVodPics.setHostAndFolder(KARTINA_HOST, pFolders->getVodPixDir());

   // set this dialog as parent for settings and timerRec ...
   Settings.setParent(this, Qt::Dialog);
   timeRec.setParent(this, Qt::Dialog);
   trayIcon.setParent(this);
   vlcCtrl.setParent(this);
   favContext.setParent(this, Qt::Popup);

   // if main settings aren't done, start settings dialog ...
   if ((Settings.GetPasswd()      == "")
#ifndef INCLUDE_LIBVLC
        || (Settings.GetVLCPath() == "")
#endif // INCLUDE_LIBVLC
        || (Settings.GetUser()    == ""))
   {
      Settings.exec();
   }

   // set settings for vod browser ...
   ui->vodBrowser->setSettings(&Settings);

   // set log level ...
   VlcLog.SetLogLevel(Settings.GetLogLevel());

   // log folder locations ...
   mInfo (tr("\ndataDir: %1\n").arg(pFolders->getDataDir())
          + tr("logoDir: %1\n").arg(pFolders->getLogoDir())
          + tr("langDir: %1\n").arg(pFolders->getLangDir())
          + tr("modDir:  %1\n").arg(pFolders->getModDir())
          + tr("appDir:  %1").arg(pFolders->getAppDir()));

   // set connection data ...
   KartinaTv.SetData(KARTINA_HOST, Settings.GetUser(), Settings.GetPasswd(),
                     Settings.GetErosPasswd(), Settings.AllowEros());


   // set proxy stuff ...
   if (Settings.UseProxy())
   {
      KartinaTv.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                         Settings.GetProxyUser(), Settings.GetProxyPasswd());

      dwnLogos.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                        Settings.GetProxyUser(), Settings.GetProxyPasswd());

      dwnVodPics.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                          Settings.GetProxyUser(), Settings.GetProxyPasswd());

      streamLoader.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                            Settings.GetProxyUser(), Settings.GetProxyPasswd());
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
   connect (&vlcCtrl, SIGNAL(sigLibVlcPlayMedia(QString, bool)), ui->player, SLOT(playMedia(QString, bool)));
   connect (&vlcCtrl, SIGNAL(sigLibVlcStop()), ui->player, SLOT(stop()));

   // aspect ratio, crop and full screen ...
   connect (this, SIGNAL(sigToggleFullscreen()), ui->player, SLOT(slotToggleFullScreen()));
   connect (this, SIGNAL(sigToggleAspectRatio()), ui->player, SLOT(slotToggleAspectRatio()));
   connect (this, SIGNAL(sigToggleCropGeometry()), ui->player, SLOT(slotToggleCropGeometry()));

   // get state if libVLC player to change player state display ...
   connect (ui->player, SIGNAL(sigPlayState(int)), this, SLOT(slotIncPlayState(int)));

#endif /* INCLUDE_LIBVLC */

   // connect signals and slots ...
   connect (&KartinaTv,    SIGNAL(sigLogout(QString)), this, SLOT(slotLogout(QString)));
   connect (&KartinaTv,    SIGNAL(sigError(QString)), this, SLOT(slotErr(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotTimeShift(QString)), this, SLOT(slotGotTimeShift(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotChannelList(QString)), this, SLOT(slotChanList(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotStreamURL(QString)), this, SLOT(slotStreamURL(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotCookie(QString)), this, SLOT(slotCookie(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotEPG(QString)), this, SLOT(slotEPG(QString)));
   connect (&KartinaTv,    SIGNAL(sigTimeShiftSet(QString)), this, SLOT(slotTimeShift(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotBitRate(QString)), this, SLOT(slotGotBitrate(QString)));
   connect (&streamLoader, SIGNAL(sigStreamDownload(int,QString)), this, SLOT(slotDownloadStarted(int,QString)));
   connect (&Refresh,      SIGNAL(timeout()), &Trigger, SLOT(slotReqChanList()));
   connect (ui->textEpg,   SIGNAL(anchorClicked(QUrl)), this, SLOT(slotEpgAnchor(QUrl)));
   connect (&dwnLogos,     SIGNAL(sigPixReady()), this, SLOT(slotLogosReady()));
   connect (&Settings,     SIGNAL(sigReloadLogos()), this, SLOT(slotReloadLogos()));
   connect (&KartinaTv,    SIGNAL(sigGotArchivURL(QString)), this, SLOT(slotArchivURL(QString)));
   connect (&Settings,     SIGNAL(sigSetServer(QString)), this, SLOT(slotSetSServer(QString)));
   connect (&Settings,     SIGNAL(sigSetBitRate(int)), this, SLOT(slotSetBitrate(int)));
   connect (&KartinaTv,    SIGNAL(sigGotTimerStreamURL(QString)), &timeRec, SLOT(slotTimerStreamUrl(QString)));
   connect (&KartinaTv,    SIGNAL(sigSrvForm(QString)), this, SLOT(slotServerForm(QString)));
   connect (&timeRec,      SIGNAL(sigRecDone()), this, SLOT(slotTimerRecordDone()));
   connect (&timeRec,      SIGNAL(sigRecActive(int)), this, SLOT(slotTimerRecActive(int)));
   connect (&trayIcon,     SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotSystrayActivated(QSystemTrayIcon::ActivationReason)));
   connect (this,          SIGNAL(sigHide()), &trayIcon, SLOT(show()));
   connect (this,          SIGNAL(sigShow()), &trayIcon, SLOT(hide()));
   connect (&vlcCtrl,      SIGNAL(sigVlcStarts(int)), this, SLOT(slotVlcStarts(int)));
   connect (&vlcCtrl,      SIGNAL(sigVlcEnds(int)), this, SLOT(slotVlcEnds(int)));
   connect (&timeRec,      SIGNAL(sigShutdown()), this, SLOT(slotShutdown()));
   connect (ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotChanListContext(QPoint)));
   connect (&favContext,   SIGNAL(triggered(QAction*)), this, SLOT(slotChgFavourites(QAction*)));
   connect (this,          SIGNAL(sigLCDStateChange(int)), ui->labState, SLOT(updateState(int)));
   connect (&KartinaTv,    SIGNAL(sigGotVodGenres(QString)), this, SLOT(slotGotVodGenres(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotVideos(QString)), this, SLOT(slotGotVideos(QString)));
   connect (ui->vodBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotVodAnchor(QUrl)));
   connect (&KartinaTv,    SIGNAL(sigGotVideoInfo(QString)), this, SLOT(slotGotVideoInfo(QString)));
   connect (&KartinaTv,    SIGNAL(sigGotVodUrl(QString)), this, SLOT(slotVodURL(QString)));

   // init short cuts ...
   InitShortCuts ();

   // trigger read of saved timer records ...
   timeRec.ReadRecordList();

   // -------------------------------------------
   // get favourites ...
   // -------------------------------------------
   lFavourites = Settings.GetFavourites();

   // enable button ...
   TouchPlayCtrlBtns(false);

   // fill search area combo box ...
   touchSearchAreaCbx();

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

            // store last position only, if it wasn't maximized ...
            if (pEvent->oldState() != Qt::WindowMaximized)
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

      // translate search are cbx ...
      touchSearchAreaCbx();
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

#ifdef INCLUDE_LIBVLC
      // end player ...
      ui->player->cleanExit();
#endif /* INCLUDE_LIBVLC */

      // clear shortcuts ...
      ClearShortCuts ();

      // clean favourites ...
      lFavourites.clear();
      HandleFavourites();

      // delete context menu stuff ...
      CleanContextMenu();

      // cancel any running kartina request ...
      KartinaTv.abort();

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

   if (bFirstConnect)
   {
      bFirstConnect = false;

      // start authenitcate chain ...
      Trigger.TriggerRequest(Kartina::REQ_COOKIE);
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
   if (Settings.exec() == QDialog::Accepted)
   {
      // if changes where saved, accept it here ...
      VlcLog.SetLogLevel(Settings.GetLogLevel());

      ui->listWidget->clear();
      KartinaTv.abort();

      // update connection data ...
      KartinaTv.SetData(KARTINA_HOST, Settings.GetUser(), Settings.GetPasswd(),
                        Settings.GetErosPasswd(), Settings.AllowEros());

      // set proxy ...
      if (Settings.UseProxy())
      {
         KartinaTv.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                            Settings.GetProxyUser(), Settings.GetProxyPasswd());

         dwnLogos.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                           Settings.GetProxyUser(), Settings.GetProxyPasswd());

         dwnVodPics.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                             Settings.GetProxyUser(), Settings.GetProxyPasswd());

         streamLoader.setProxy(Settings.GetProxyHost(), Settings.GetProxyPort(),
                               Settings.GetProxyUser(), Settings.GetProxyPasswd());
      }

      // set language as read ...
      pTranslator->load(QString("lang_%1").arg(Settings.GetLanguage ()),
                        pFolders->getLangDir());

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
   if (showInfo.archive () && (showInfo.playState () == IncPlay::PS_PLAY))
   {
      if (AllowAction(IncPlay::PS_RECORD))
      {
         // archive play active ...
         uint    gmt = ui->player->getSilderPos ();
         QString req = QString("cid=%1&gmt=%2").arg(showInfo.channelId()).arg(gmt);

         showInfo.setPlayState(IncPlay::PS_RECORD);

         TouchPlayCtrlBtns(false);
         Trigger.TriggerRequest(Kartina::REQ_ARCHIV, req);
      }
   }
   else
   {

#endif // INCLUDE_LIBVLC
      CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

      if (pItem)
      {
         if (pItem->GetId() != -1)
         {
            if (AllowAction(IncPlay::PS_RECORD))
            {
               // new own downloader ...
               if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
               {
                  streamLoader.stopDownload (iDwnReqId);
                  iDwnReqId = -1;
               }

               showInfo.setChanId(pItem->GetId());
               showInfo.setChanName(pItem->GetName());
               showInfo.setArchive(false);
               showInfo.setShowName(pItem->GetProgram());
               showInfo.setStartTime(pItem->GetStartTime());
               showInfo.setEndTime(pItem->GetEndTime());
               showInfo.setPlayState(IncPlay::PS_RECORD);

               TouchPlayCtrlBtns(false);
               Trigger.TriggerRequest(Kartina::REQ_STREAM, pItem->GetId());
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
      && showInfo.archive())
   {
      // we're playing ... we want pause ...
      ui->player->pause();

      // update showInfo ...
      showInfo.setPlayState(IncPlay::PS_PAUSE);

      // update buttons ...
      TouchPlayCtrlBtns(true);
   }
   else if ((showInfo.playState() == IncPlay::PS_PAUSE)
      && showInfo.archive())
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
      CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

      if (pItem)
      {
         if (pItem->GetId() != -1)
         {
            if (AllowAction(IncPlay::PS_PLAY))
            {
               showInfo.setChanId(pItem->GetId());
               showInfo.setChanName(pItem->GetName());
               showInfo.setArchive(false);
               showInfo.setShowName(pItem->GetProgram());
               showInfo.setStartTime(pItem->GetStartTime());
               showInfo.setEndTime(pItem->GetEndTime());
               showInfo.setPlayState(IncPlay::PS_PLAY);

               TouchPlayCtrlBtns(false);
               Trigger.TriggerRequest(Kartina::REQ_STREAM, pItem->GetId());
            }
         }
      }
#ifdef INCLUDE_LIBVLC
   }
#endif // INCLUDE_LIBVLC
}

/* -----------------------------------------------------------------\
|  Method: on_listWidget_currentRowChanged
|  Begin: 19.01.2010 / 16:13:56
|  Author: Jo2003
|  Description: channel changed, request epg if needed
|
|  Parameters: slected row index
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_listWidget_currentRowChanged(int currentRow)
{
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->item(currentRow);
   if (pItem)
   {
      if(pItem->GetId() != -1)
      {
         // get whole channel entry ...
         cparser::SChan entry = chanMap.value(pItem->GetId());
         int iTs;

         ui->textEpgShort->setHtml(QString(TMPL_BACKCOLOR).arg("rgb(255, 254, 212)").arg(pItem->toolTip()));
         SetProgress (pItem->GetStartTime(), pItem->GetEndTime());

         // quick'n'dirty timeshift hack ...
         if (entry.vTs.count() <= 2) // no timeshift available ...
         {
            ui->textEpg->SetTimeShift(0);
         }
         else
         {
            iTs = ui->cbxTimeShift->currentText().toInt();

            if (ui->textEpg->GetTimeShift() != iTs)
            {
               ui->textEpg->SetTimeShift(iTs);
            }
         }

         // was this a refresh or was channel changed ... ?
         if (pItem->GetId() != ui->textEpg->GetCid())
         {
            // load epg ...
            Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
         }
         else // same channel ...
         {
            // refresh epg only, if we view current day in epg ...
            if (iEpgOffset == 0) // 0 means today!
            {
               Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId());
            }
         }
      }
   }
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
   int iListIdx = ui->cbxChannelGroup->itemData(index).toInt();

   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->item(iListIdx);

   ui->listWidget->scrollToItem(pItem, QAbstractItemView::PositionAtTop);
   ui->listWidget->setCurrentRow(iListIdx + 1);
   ui->listWidget->setFocus(Qt::OtherFocusReason);
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
   CAboutDialog dlg(this);
   dlg.ConnectSettings(&Settings);
   dlg.exec();
}

/* -----------------------------------------------------------------\
|  Method: on_listWidget_itemDoubleClicked
|  Begin: 19.01.2010 / 16:19:25
|  Author: Jo2003
|  Description: double click on channel list -> start play channel
|
|  Parameters: pointer to listwidgetitem ...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_listWidget_itemDoubleClicked(QListWidgetItem* item)
{
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)item;

   if (pItem)
   {
      if (pItem->GetId() != -1)
      {
         if (AllowAction(IncPlay::PS_PLAY))
         {
            showInfo.setChanId(pItem->GetId());
            showInfo.setChanName(pItem->GetName());
            showInfo.setArchive(false);
            showInfo.setShowName(pItem->GetProgram());
            showInfo.setStartTime(pItem->GetStartTime());
            showInfo.setEndTime(pItem->GetEndTime());
            showInfo.setPlayState(IncPlay::PS_PLAY);

            TouchPlayCtrlBtns(false);
            Trigger.TriggerRequest(Kartina::REQ_STREAM, pItem->GetId());
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
   int                  iCid  = 0;
   uint                 now   = QDateTime::currentDateTime().toTime_t();
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   if (pItem)
   {
      iCid = pItem->GetId();
   }

   // timeRec.SetRecInfo(now, now, -1);
   timeRec.SetRecInfo(now, now, iCid);
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
   QFont font;
   ui->textEpg->ReduceFont();
   ui->textEpgShort->ReduceFont();
   ui->vodBrowser->ReduceFont();

   font = ui->listWidget->font();
   font.setPointSize(font.pointSize() - 1);
   ui->listWidget->setFont(font);

   font = ui->cbxChannelGroup->font();
   font.setPointSize(font.pointSize() - 1);
   ui->cbxChannelGroup->setFont(font);

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
   QFont font;
   ui->textEpg->EnlargeFont();
   ui->textEpgShort->EnlargeFont();
   ui->vodBrowser->EnlargeFont();

   font = ui->listWidget->font();
   font.setPointSize(font.pointSize() + 1);
   ui->listWidget->setFont(font);

   font = ui->cbxChannelGroup->font();
   font.setPointSize(font.pointSize() + 1);
   ui->cbxChannelGroup->setFont(font);

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
|  Method: on_cbxGenre_currentIndexChanged [slot]
|  Begin: 20.12.2010 / 14:18
|  Author: Jo2003
|  Description: vod genre changed
|
|  Parameters: new index ...
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_cbxGenre_currentIndexChanged(int index)
{
   int iGid = ui->cbxGenre->itemData(index).toInt();

   Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, iGid);
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
   int iIdx = ui->cbxSearchArea->currentIndex();
   vodbrowser::eSearchArea eArea = (vodbrowser::eSearchArea)ui->cbxSearchArea->itemData(iIdx).toUInt();
   ui->vodBrowser->findVideos(ui->lineVodSearch->text(), eArea);
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
   if (bDoInitDlg)
   {
      bDoInitDlg = false;
      initDialog ();
   }

   QWidget::show();
}

/* -----------------------------------------------------------------\
|  Method: slotTimeShiftChanged
|  Begin: 19.01.2010 / 16:13:03
|  Author: Jo2003
|  Description: set new timeshift
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotTimeShiftChanged(const QString& str)
{
   TouchPlayCtrlBtns(false);

   // set timeshift ...
   ui->textEpg->SetTimeShift(str.toInt());
   timeRec.SetTimeShift(str.toInt());

   Trigger.TriggerRequest(Kartina::REQ_TIMESHIFT, str.toInt());
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
|  Method: slotErr
|  Begin: 19.01.2010 / 16:08:51
|  Author: Jo2003
|  Description: display errors signaled by other threads
|
|  Parameters: error string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotErr(QString str)
{
   QMessageBox::critical(this, tr("Error"),
                         tr("%1 Client API reports some errors: %2")
                         .arg(COMPANY_NAME).arg(str));
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
void Recorder::slotLogout(QString str)
{

   XMLParser.checkResponse(str, __FUNCTION__, __LINE__);

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
void Recorder::slotStreamURL(QString str)
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
void Recorder::slotCookie (QString str)
{
   QString sCookie;

   // parse cookie ...
   if (!XMLParser.parseCookie(str, sCookie))
   {
      KartinaTv.SetCookie(sCookie);

      // request streamserver ...
      Trigger.TriggerRequest(Kartina::REQ_GET_SERVER);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotServerForm
|  Begin: 27.02.2010 / 18:35:12
|  Author: Jo2003
|  Description: got server list form
|
|  Parameters: html form
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotServerForm(QString str)
{
   QVector<cparser::SSrv> vSrv;
   QString                sActSrv;

   if (!XMLParser.parseSServers(str, vSrv, sActSrv))
   {
      Settings.SetStreamServerCbx(vSrv, sActSrv);

      mInfo(tr("Active stream server is %1").arg(sActSrv));

      Trigger.TriggerRequest(Kartina::REQ_GETBITRATE);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotGotTimeShift
|  Begin: 29.10.2010 / 16:10:23
|  Author: Jo2003
|  Description: parse timeshift response
|
|  Parameters: response string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotGotTimeShift(QString str)
{
   // parse timeshift ...
   QVector<int> vValues;
   int          iShift;
   QString      sName;

   if (!XMLParser.parseSettings(str, vValues, iShift, sName))
   {
      QVector<int>::const_iterator cit;

      // set timeshift ...
      ui->textEpg->SetTimeShift(iShift);
      timeRec.SetTimeShift(iShift);

      // clear timeshift cbx ...
      ui->cbxTimeShift->clear();

      // fill timeshift cbx ...
      for (cit = vValues.constBegin(); cit != vValues.constEnd(); cit ++)
      {
         ui->cbxTimeShift->addItem(QString::number(*cit));
      }

      // mark active timeshift ...
      ui->cbxTimeShift->setCurrentIndex(ui->cbxTimeShift->findText(QString::number(iShift)));

      // request channel list ...
      Trigger.TriggerRequest(Kartina::REQ_CHANNELLIST);

   }

   // Former there was a "on_cbxTimeShift_currentIndexChanged" slot. But because
   // we have to change the timeshift cbx now also when changing the
   // channel, we need the possibility to disconnect the signal from the slot!
   connect (ui->cbxTimeShift, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotTimeShiftChanged(QString)));
}

/* -----------------------------------------------------------------\
|  Method: slotGotBitrate
|  Begin: 14.01.2011 / 14:20
|  Author: Jo2003
|  Description: parse bitrate response
|
|  Parameters: response string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotGotBitrate(QString str)
{
   QVector<int> vValues;
   int          iActVal = 0;
   QString      sName;

   if (!XMLParser.parseSettings(str, vValues, iActVal, sName))
   {
      Settings.SetBitrateCbx(vValues, iActVal);
      mInfo (tr("Using Bitrate %1 kbit/s ...").arg(iActVal));

      Trigger.TriggerRequest(Kartina::REQ_GETTIMESHIFT);
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
void Recorder::slotTimeShift (QString str)
{
   if(!XMLParser.checkResponse(str, __FUNCTION__, __LINE__))
   {
      Trigger.TriggerRequest(Kartina::REQ_CHANNELLIST);
   }
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
void Recorder::slotChanList (QString str)
{
   QVector<cparser::SChan> chanList;
   QVector<cparser::SChan>::const_iterator cit;

   if (!XMLParser.parseChannelList(str, chanList, Settings.FixTime()))
   {
      FillChanMap(chanList);
      FillChannelList(chanList);

      // set channel list in timeRec class ...
      timeRec.SetChanList(chanList);
      timeRec.StartTimer();
   }

   // only download channel logos, if they aren't there ...
   if (!dwnLogos.IsRunning() && !bLogosReady)
   {
      QStringList lLogos;

      for (cit = chanList.constBegin(); cit != chanList.constEnd(); cit ++)
      {
         if(!(*cit).bIsGroup)
         {
            lLogos.push_back((*cit).sIcon);
         }
      }

      dwnLogos.setPictureList(lLogos);
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
void Recorder::slotEPG(QString str)
{
   QVector<cparser::SEpg> epg;
   int                    cid     = 0;
   QDateTime              epgTime = QDateTime::currentDateTime().addDays(iEpgOffset);

   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   if (pItem)
   {
      cid = pItem->GetId();
   }

   if (!XMLParser.parseEpg(str, epg))
   {
      ui->textEpg->DisplayEpg(epg, chanMap.value(cid).sName,
                              cid, epgTime.toTime_t(),
                              chanMap.value(cid).bHasArchive);

      // fill epg control ...
      if (QFile::exists(QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(cid)))
      {
         QPixmap pic;
         pic.load(QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(cid));
         ui->labChanIcon->setPixmap(pic.scaled(24, 24, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));
      }

      ui->labChanName->setText(chanMap.value(cid).sName);
      ui->labCurrDay->setText(epgTime.toString("dd. MMM. yyyy"));

      pEpgNavbar->setCurrentIndex(epgTime.date().dayOfWeek() - 1);

      TouchPlayCtrlBtns();
      ui->listWidget->setFocus(Qt::OtherFocusReason);

      // update vod stuff only at startup ...
      if (ui->cbxGenre->count() == 0)
      {
         Trigger.TriggerRequest(Kartina::REQ_GETVODGENRES);
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
      TouchPlayCtrlBtns(false);

      // new own downloader ...
      if (vlcCtrl.ownDwnld() && (iDwnReqId != -1))
      {
         streamLoader.stopDownload (iDwnReqId);
         iDwnReqId = -1;
      }

      QString    cid  = link.encodedQueryItemValue(QByteArray("cid"));
      QString    gmt  = link.encodedQueryItemValue(QByteArray("gmt"));
      QString    req  = QString("cid=%1&gmt=%2").arg(cid.toInt()).arg(gmt.toUInt());
      epg::SShow sepg = ui->textEpg->epgShow(gmt.toUInt());

      // store all info about show ...
      showInfo.setChanId(cid.toInt());
      showInfo.setChanName(chanMap.value(cid.toInt()).sName);
      showInfo.setShowName(sepg.sShowName);
      showInfo.setStartTime(gmt.toUInt());
      showInfo.setEndTime(sepg.uiEnd);
      showInfo.setArchive(true);
      showInfo.setPlayState(ePlayState);
      showInfo.setLastJumpTime(0);

      // add additional info to LCD ...
      int     iTime = (sepg.uiEnd) ? (int)((sepg.uiEnd - sepg.uiStart) / 60) : 60;
      QString sTime = tr("Length: %1 min.").arg(iTime);
      ui->labState->setHeader(showInfo.chanName() + tr(" (Ar.)"));
      ui->labState->setFooter(sTime);

      Trigger.TriggerRequest(Kartina::REQ_ARCHIV, req);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotLogosReady
|  Begin: 19.01.2010 / 16:17:23
|  Author: Jo2003
|  Description: logo downloader told us that logos are ready ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotLogosReady()
{
   // downloader sayd ... logos are there ...
   bLogosReady = true;
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
   bLogosReady = false;

   if (!dwnLogos.IsRunning())
   {
      QStringList lLogos;
      QMap<int, cparser::SChan>::const_iterator cit;

      // create tmp channel list with channels from listWidget ...
      for (cit = chanMap.constBegin(); cit != chanMap.constEnd(); cit++)
      {
         if (!(*cit).bIsGroup)
         {
            lLogos.push_back((*cit).sIcon);
         }
      }

      dwnLogos.setPictureList(lLogos);
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
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();
   if (pItem)
   {
      if  (pItem->GetId() != -1)
      {
         // set actual day in previous week to munday ...
         int iActDay  = pEpgNavbar->currentIndex();
         iEpgOffset  -= 7 + iActDay;
         Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
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
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();
   if (pItem)
   {
      if (pItem->GetId() != -1)
      {
         // set actual day in next week to munday ...
         int iActDay  = pEpgNavbar->currentIndex();
         iEpgOffset  += 7 - iActDay;
         Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
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
void Recorder::slotArchivURL(QString str)
{
   QString sUrl;

   if (!XMLParser.parseUrl(str, sUrl))
   {
      if (ePlayState == IncPlay::PS_RECORD)
      {
         if (!vlcCtrl.ownDwnld())
         {
            StartVlcRec(sUrl, CleanShowName(showInfo.showName()), true);
         }
         else
         {
            StartStreamDownload(sUrl, CleanShowName(showInfo.showName()));
         }

         showInfo.setPlayState(IncPlay::PS_RECORD);
      }
      else if (ePlayState == IncPlay::PS_PLAY)
      {
         StartVlcPlay(sUrl, true);

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
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();
   if (pItem)
   {
      if (pItem->GetId() != -1)
      {
         QDateTime epgTime = QDateTime::currentDateTime().addDays(iEpgOffset);
         int       iDay    = epgTime.date().dayOfWeek() - 1;

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
            Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
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
void Recorder::slotVlcEnds(int iState)
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
   // get channel item under mouse pointer ...
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->itemAt(pt);

   // do we have an item ... ?
   if (pItem)
   {
      if (pItem->GetId() != -1) // real channel ...
      {
         // create context menu ...
         CleanContextMenu();
         pContextAct[0] = new CFavAction (&favContext);
         QString    sLogoFile = QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(pItem->GetId());

         // is channel already in favourites ... ?
         if (lFavourites.contains(pItem->GetId()))
         {
            // create remove menu ...
            // action.setText(tr("Remove \"%1\" from favourites").arg(pItem->GetName()));
            pContextAct[0]->setText(tr("Remove from favourites"));
            pContextAct[0]->setIcon(QIcon(sLogoFile));
            pContextAct[0]->setFavData(pItem->GetId(), kartinafav::FAV_DEL);
         }
         else
         {
            // create add menu ...
            // action.setText(tr("Add \"%1\" to favourites").arg(pItem->GetName()));
            pContextAct[0]->setText(tr("Add to favourites"));
            pContextAct[0]->setIcon(QIcon(sLogoFile));
            pContextAct[0]->setFavData(pItem->GetId(), kartinafav::FAV_ADD);
         }

         // add action to menu ...
         favContext.addAction(pContextAct[0]);

         // display menu ...
         favContext.exec(ui->listWidget->mapToGlobal(pt));
      }
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
      CChanListWidgetItem *pItem;

      // go through channel list ...
      for (int i = 0; i < ui->listWidget->count(); i++)
      {
         pItem = (CChanListWidgetItem *)ui->listWidget->item(i);

         if (pItem)
         {
            // check if this is favourite channel ...
            if (pItem->GetId() == iCid)
            {
               // found --> mark row ...
               ui->listWidget->setCurrentRow (i);
               ui->listWidget->scrollToItem(pItem);
               ui->listWidget->setFocus(Qt::OtherFocusReason);
               break;
            }
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
   CAboutDialog dlg(this);
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
void Recorder::slotGotVodGenres(QString str)
{
   QVector<cparser::SGenre> vGenres;
   QVector<cparser::SGenre>::const_iterator cit;

   // delete content ...
   ui->cbxGenre->clear();

   if (!XMLParser.parseGenres(str, vGenres))
   {
      // fill genres combo box ...
      ui->cbxGenre->addItem(tr("All"), QVariant((int)-1));

      for (cit = vGenres.constBegin(); cit != vGenres.constEnd(); cit ++)
      {
         ui->cbxGenre->addItem((*cit).sGName, QVariant((int)(*cit).uiGid));
      }
   }

   // changing combo box will trigger all vod request ...
   ui->cbxGenre->setCurrentIndex(0);
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
void Recorder::slotGotVideos(QString str)
{
   QVector<cparser::SVodVideo> vVodList;
   QVector<cparser::SVodVideo>::const_iterator cit;

   if (!XMLParser.parseVodList(str, vVodList))
   {
      if (!dwnVodPics.IsRunning())
      {
         // download pictures ...
         QStringList lPix;

         for (cit = vVodList.constBegin(); cit != vVodList.constEnd(); cit ++)
         {
            lPix.push_back((*cit).sImg);
         }

         dwnVodPics.setPictureList(lPix);
      }

      ui->vodBrowser->displayVodList (vVodList, ui->cbxGenre->currentText());
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

   if (action == "vod_info")
   {
      id = link.encodedQueryItemValue(QByteArray("vodid")).toInt();
      Trigger.TriggerRequest(Kartina::REQ_GETVIDEOINFO, id);
   }
   else if (action == "backtolist")
   {
      id = ui->cbxGenre->currentIndex();

      id = ui->cbxGenre->itemData(id).toInt();

      Trigger.TriggerRequest(Kartina::REQ_GETVIDEOS, id);
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

      showInfo.setChanId(-1);
      showInfo.setChanName("");
      showInfo.setShowName(ui->vodBrowser->getName());
      showInfo.setStartTime(0);
      showInfo.setEndTime(0);
      showInfo.setArchive(true); // enable spooling ...
      showInfo.setPlayState(ePlayState);
      showInfo.setLastJumpTime(0);

      ui->labState->setHeader(tr("Video On Demand"));
      ui->labState->setFooter(showInfo.showName());

      Trigger.TriggerRequest(Kartina::REQ_GETVODURL, id);
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
void Recorder::slotGotVideoInfo(QString str)
{
   cparser::SVodVideo vodInfo;
   if (!XMLParser.parseVideoInfo(str, vodInfo))
   {
      ui->vodBrowser->displayVideoDetails(vodInfo);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotVodURL
|  Begin: 22.12.2010 / 15:30
|  Author: Jo2003
|  Description: got requested vod url, start play / record
|
|  Parameters: vod url (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotVodURL(QString str)
{
   QString sUrl;

   if (!XMLParser.parseUrl(str, sUrl))
   {
      if (ePlayState == IncPlay::PS_RECORD)
      {
         // use own downloader ... ?
         if (!vlcCtrl.ownDwnld())
         {
            StartVlcRec(sUrl, CleanShowName(showInfo.showName()));
         }
         else
         {
            StartStreamDownload(sUrl, CleanShowName(showInfo.showName()), "m4v");
         }

         showInfo.setPlayState(IncPlay::PS_RECORD);
      }
      else if (ePlayState == IncPlay::PS_PLAY)
      {
         StartVlcPlay(sUrl, true);

         showInfo.setPlayState(IncPlay::PS_PLAY);
      }
   }

   TouchPlayCtrlBtns(true);
}


////////////////////////////////////////////////////////////////////////////////
//                             normal functions                               //
////////////////////////////////////////////////////////////////////////////////

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
   pTranslator->load(QString("lang_%1").arg(Settings.GetLanguage ()),
                     pFolders->getLangDir());

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
      QFont font;
      ui->textEpg->ChangeFontSize(iFontSzChg);
      ui->textEpgShort->ChangeFontSize(iFontSzChg);
      ui->vodBrowser->ChangeFontSize(iFontSzChg);

      font = ui->listWidget->font();
      font.setPointSize(font.pointSize() + iFontSzChg);
      ui->listWidget->setFont(font);

      font = ui->cbxChannelGroup->font();
      font.setPointSize(font.pointSize() + iFontSzChg);
      ui->cbxChannelGroup->setFont(font);
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
   trayIcon.setIcon(QIcon(":/app/tv"));
   trayIcon.setToolTip(tr("vlc-record - Click to activate!"));
}

/* -----------------------------------------------------------------\
|  Method: touchSearchAreaCbx
|  Begin: 23.12.2010 / 10:30
|  Author: Jo2003
|  Description: create search area combo box
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::touchSearchAreaCbx ()
{
   // fill search area combo box ...
   ui->cbxSearchArea->clear();
   ui->cbxSearchArea->addItem(tr("Title"), QVariant((int)vodbrowser::IN_TITLE));
   ui->cbxSearchArea->addItem(tr("Description"), QVariant((int)vodbrowser::IN_DESCRIPTION));
   ui->cbxSearchArea->addItem(tr("Year"), QVariant((int)vodbrowser::IN_YEAR));
   ui->cbxSearchArea->addItem(tr("Everywhere"), QVariant((int)vodbrowser::IN_EVERYWHERE));
   ui->cbxSearchArea->setCurrentIndex(0);
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
   QToolButton *pBtn;

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
      pBtn = new QToolButton;
      pBtn->setIcon(QIcon(":png/back"));
      pBtn->setAutoRaise(true);
      pBtn->setMaximumHeight(EPG_NAVBAR_HEIGHT);
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
      pBtn = new QToolButton;
      pBtn->setIcon(QIcon(":png/next"));
      pBtn->setAutoRaise(true);
      pBtn->setMaximumHeight(EPG_NAVBAR_HEIGHT);
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
      pBtn = (QToolButton *)ui->hLayoutEpgNavi->itemAt(iIdx)->widget();
      pBtn->setToolTip(tr("1 week backward"));

      // next button ...
      iIdx = ui->hLayoutEpgNavi->count() - 1;
      pBtn = (QToolButton *)ui->hLayoutEpgNavi->itemAt(iIdx)->widget();
      pBtn->setToolTip(tr("1 week forward"));
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
   CShortcutEx *pShortCut;
   pShortCut = NULL;

#ifdef INCLUDE_LIBVLC
   // aspect ratio ...
   pShortCut = new CShortcutEx (QKeySequence("ALT+A"), this);
   if (pShortCut)
   {
      connect (pShortCut, SIGNAL(activated()), ui->player, SLOT(slotToggleAspectRatio()));

      // save shortcut ...
      vShortcutPool.push_back(pShortCut);
   }

   // crop geometry ...
   pShortCut = new CShortcutEx (QKeySequence("ALT+C"), this);
   if (pShortCut)
   {
      connect (pShortCut, SIGNAL(activated()), ui->player, SLOT(slotToggleCropGeometry()));

      // save shortcut ...
      vShortcutPool.push_back(pShortCut);
   }

   // fullscreen ...
   pShortCut = new CShortcutEx (QKeySequence("ALT+F"), this);
   if (pShortCut)
   {
      connect (pShortCut, SIGNAL(activated()), ui->player, SLOT(slotToggleFullScreen()));

      // save shortcut ...
      vShortcutPool.push_back(pShortCut);
   }

   // timejump forward ...
   pShortCut = new CShortcutEx (QKeySequence("CTRL+ALT+F"), this);
   if (pShortCut)
   {
      connect (pShortCut, SIGNAL(activated()), this, SLOT(on_pushFwd_clicked()));

      // save shortcut ...
      vShortcutPool.push_back(pShortCut);
   }

   // timejump backward ...
   pShortCut = new CShortcutEx (QKeySequence("CTRL+ALT+B"), this);
   if (pShortCut)
   {
      connect (pShortCut, SIGNAL(activated()), this, SLOT(on_pushBwd_clicked()));

      // save shortcut ...
      vShortcutPool.push_back(pShortCut);
   }

   // pause ...
   pShortCut = new CShortcutEx (QKeySequence("CTRL+ALT+P"), this);
   if (pShortCut)
   {
      // connect (pShortCut, SIGNAL(activated()), ui->player, SLOT(pause()));
      connect (pShortCut, SIGNAL(activated()), this, SLOT(on_pushPlay_clicked()));

      // save shortcut ...
      vShortcutPool.push_back(pShortCut);
   }

#endif // INCLUDE_LIBVLC
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
|  Method: WantToClose
|  Begin: 01.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: ask if we want to clse vlc-record
|
|  Parameters: --
|
|  Returns: true --> close
|          false --> don't close
\----------------------------------------------------------------- */
bool Recorder::WantToClose()
{
   QString sText = HTML_SITE;
   sText.replace(TMPL_CONT, tr("VLC is still running.<br />"
                               "<b>Closing VLC record will also close the started VLC-Player.</b>"
                               "<br /> <br />"
                               "Do you really want to close VLC Record now?"));

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
   QString  sToolTip;
   QString  sLogoFile;
   CChanListWidgetItem *pItem;
   int      iRow, iRowGroup;
   QPixmap  Pix(16, 16);
   int      iChanCount = 0;

   // get current item ...
   iRow      = ui->listWidget->currentRow();
   iRow      = (iRow <= 0) ? 1 : iRow;
   iRowGroup = ui->cbxChannelGroup->currentIndex();
   iRowGroup = (iRowGroup < 0) ? 0 : iRowGroup;

   // clear channel list ...
   ui->listWidget->clear();

   // clear channel group list ...
   ui->cbxChannelGroup->clear();

   for (int i = 0; i < chanlist.size(); i++)
   {
      // is this a channel group ... ?
      if (chanlist[i].bIsGroup)
      {
         pItem = new CChanListWidgetItem (chanlist[i].sName, -1, ui->listWidget);
         pItem->setBackgroundColor(QColor(chanlist[i].sProgramm));

         // add channel group entry ...
         Pix.fill(QColor(chanlist[i].sProgramm));
         ui->cbxChannelGroup->addItem(QIcon(Pix), chanlist[i].sName, QVariant(i));
      }
      else
      {
         sLogoFile = QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(chanlist[i].iId);
         sLine     = QString("%1. %2").arg(++ iChanCount).arg(chanlist[i].sName);

         if (QFile::exists(sLogoFile))
         {
            pItem = new CChanListWidgetItem (QIcon(sLogoFile), sLine, chanlist[i].iId, chanlist[i].sName, ui->listWidget);
         }
         else
         {
            pItem = new CChanListWidgetItem (sLine, chanlist[i].iId, chanlist[i].sName, ui->listWidget);
         }

         // store program ...
         pItem->SetProgram(chanlist[i].sProgramm);

         // create tool tip with programm info ...
         sToolTip = PROG_INFO_TOOL_TIP;
         sToolTip.replace(TMPL_PROG, tr("Program:"));
         sToolTip.replace(TMPL_START, tr("Start:"));
         sToolTip.replace(TMPL_END, tr("End:"));

         sToolTip = sToolTip.arg(chanlist[i].sName).arg(chanlist[i].sProgramm)
                     .arg(QDateTime::fromTime_t(chanlist[i].uiStart).toString(DEF_TIME_FORMAT))
                     .arg(QDateTime::fromTime_t(chanlist[i].uiEnd).toString(DEF_TIME_FORMAT));

         pItem->setToolTip(sToolTip);

         pItem->SetStartEnd(chanlist[i].uiStart, chanlist[i].uiEnd);
      }
   }

   ui->cbxChannelGroup->setCurrentIndex(iRowGroup);
   ui->listWidget->setCurrentRow (iRow);
   ui->listWidget->setFocus(Qt::OtherFocusReason);

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
int Recorder::StartVlcRec (const QString &sURL, const QString &sChannel, bool bArchiv)
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
      if (bArchiv)
      {
         // archiv using RTSP ...
         sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_REC_ARCH,
                                         Settings.GetVLCPath(),
                                         sURL, Settings.GetBufferTime(),
                                         fileName, sExt);
      }
      else
      {
         // normal stream using HTTP ...
         sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_REC_LIVE,
                                         Settings.GetVLCPath(),
                                         sURL, Settings.GetBufferTime(),
                                         fileName, sExt);
      }

      // start player if we have a command line ...
      if (sCmdLine != "")
      {
         vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState, bArchiv);
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
int Recorder::StartVlcPlay (const QString &sURL, bool bArchiv)
{
   int         iRV      = 0;
   Q_PID       vlcpid   = 0;
   QString     sCmdLine;

   if (bArchiv)
   {
      // archiv using RTSP ...
      sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_PLAY_ARCH,
                                      Settings.GetVLCPath(), sURL,
                                      Settings.GetBufferTime());
   }
   else
   {
      // normal stream using HTTP ...
      sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_PLAY_LIVE,
                                      Settings.GetVLCPath(), sURL,
                                      Settings.GetBufferTime());
   }

   // start player if we have a command line ...
   if (sCmdLine != "")
   {
      vlcpid = vlcCtrl.start(sCmdLine, -1, Settings.DetachPlayer(), ePlayState, bArchiv);
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
      if (bEnable && (showInfo.playState() == IncPlay::PS_PLAY)
         && showInfo.archive())
      {
         ui->pushBwd->setEnabled(true);
         ui->pushFwd->setEnabled(true);
         ui->cbxTimeJumpVal->setEnabled(true);
      }
      else
      {
         ui->pushBwd->setEnabled(false);
         ui->pushFwd->setEnabled(false);
         ui->cbxTimeJumpVal->setEnabled(false);
      }

      if (bEnable && (showInfo.playState() == IncPlay::PS_PLAY)
         && showInfo.archive()
         && bEnable)
      {
         ui->pushPlay->setIcon(QIcon(":/app/pause"));
      }
      else
      {
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
      ui->cbxTimeShift->setEnabled(bEnable);
      ui->pushPlay->setEnabled(bEnable);
      ui->pushRecord->setEnabled(bEnable);
      ui->pushStop->setEnabled(bEnable);
      break;

   case IncPlay::PS_RECORD:
      ui->cbxTimeShift->setEnabled(false);
      ui->pushPlay->setEnabled(false);
      ui->pushRecord->setEnabled(bEnable);
      ui->pushStop->setEnabled(bEnable);
      break;

   case IncPlay::PS_TIMER_RECORD:
   case IncPlay::PS_TIMER_STBY:
      ui->cbxTimeShift->setEnabled(false);
      ui->pushPlay->setEnabled(false);
      ui->pushRecord->setEnabled(false);
      ui->pushStop->setEnabled(bEnable);
      break;

   default:
      ui->cbxTimeShift->setEnabled(bEnable);
      ui->pushPlay->setEnabled(bEnable);
      ui->pushRecord->setEnabled(bEnable);
      ui->pushStop->setEnabled(false);
      break;
   }

   emit sigLCDStateChange((int)ePlayState);
}

/* -----------------------------------------------------------------\
|  Method: SetProgress
|  Begin: 19.01.2010 / 16:15:17
|  Author: Jo2003
|  Description: set progress bar (program time)
|
|  Parameters: start and end timestamp
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::SetProgress (const uint &start, const uint &end)
{
   int iPercent = 0;

   if (start && end)
   {
      int iLength  = (int)(end - start);
      int iNow     = (int)(QDateTime::currentDateTime().toTime_t() - start);

      // error check (div / 0 PC doesn't like ;-) ) ...
      if ((iNow > 0) && (iLength > 0))
      {
         // get percent ...
         iPercent  = (int)((iNow * 100) / iLength);
      }
   }

   ui->progressBar->setValue(iPercent);
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
   sName.replace(QString("&quot;"), QString(" "));
   sName.replace(QString("\""), QString(" "));
   sName.replace(QString("'"), QString(" "));
   sName.replace(QString("-"), QString(" "));
   sName.replace(QString("."), QString(" "));
   sName.replace(QString(","), QString(" "));
   sName.replace(QString("/"), QString("-"));
   sName.replace(QString("\\"), QString(""));
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

   // remove all favourite buttons ...
   for (i = 0; i < MAX_NO_FAVOURITES; i++)
   {
      if (pFavBtn[i] != NULL)
      {
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

         // set shortcut ...
         pFavAct[i]->setShortcut(QKeySequence(QString("ALT+%1").arg(i)));

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

/************************* History ***************************\
| $Log$
\*************************************************************/

