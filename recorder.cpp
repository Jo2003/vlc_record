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
#include "ui_recorder.h"
#include "chanlistwidgetitem.h"

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
   bRecord        = true;
   bLogosReady    = false;
   bPendingRecord = false;
   bVlcRuns       = false;
   pTranslator    = trans;
   iEpgOffset     = 0;
   uiArchivGmt    = 0;
   sLogoPath      = dwnLogos.GetLogoPath();

   VlcLog.SetLogFile(QString(INI_DIR).arg(getenv(APPDATA)).toLocal8Bit().data(), LOG_FILE_NAME);

   // set this dialog as parent for settings and timerRec ...
   Settings.setParent(this, Qt::Dialog);
   timeRec.setParent(this, Qt::Dialog);

   // if main settings aren't done, start settings dialog ...
   if ((Settings.GetPasswd()      == "")
        || (Settings.GetVLCPath() == "")
        || (Settings.GetUser()    == ""))
   {
      Settings.exec();
   }

   // set log level ...
   VlcLog.SetLogLevel(Settings.GetLogLevel());

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
   }

   // set language as read ...
   pTranslator->load(QString("lang_%1").arg(Settings.GetLanguage ()), QApplication::applicationDirPath());

   // configure trigger and start it ...
   Trigger.SetKartinaClient(&KartinaTv);
   Trigger.start();

   // give vlcCtrl needed infos ...
   vlcCtrl.setParent(this);
   vlcCtrl.LoadPlayerModule(Settings.GetPlayerModule());
   vlcCtrl.SetTranslitPointer(&translit);
   vlcCtrl.SetTranslitSettings(Settings.TranslitRecFile());

   // give timerRec all needed infos ...
   timeRec.SetXmlParser(&XMLParser);
   timeRec.SetKartinaTrigger(&Trigger);
   timeRec.SetSettings(&Settings);
   timeRec.SetVlcCtrl(&vlcCtrl);

   // connect signals and slots ...
   connect (&KartinaTv,  SIGNAL(sigError(QString)), this, SLOT(slotErr(QString)));
   connect (&KartinaTv,  SIGNAL(sigGotChannelList(QString)), this, SLOT(slotChanList(QString)));
   connect (&KartinaTv,  SIGNAL(sigGotStreamURL(QString)), this, SLOT(slotStreamURL(QString)));
   connect (&KartinaTv,  SIGNAL(sigGotCookie()), this, SLOT(slotCookie()));
   connect (&KartinaTv,  SIGNAL(sigGotEPG(QString)), this, SLOT(slotEPG(QString)));
   connect (&KartinaTv,  SIGNAL(sigTimeShiftSet()), this, SLOT(slotTimeShift()));
   connect (&Refresh,    SIGNAL(timeout()), &Trigger, SLOT(slotReqChanList()));
   connect (ui->textEpg, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotEpgAnchor(QUrl)));
   connect (&dwnLogos,   SIGNAL(sigLogosReady()), this, SLOT(slotLogosReady()));
   connect (&Settings,   SIGNAL(sigReloadLogos()), this, SLOT(slotReloadLogos()));
   connect (&KartinaTv,  SIGNAL(sigGotArchivURL(QString)), this, SLOT(slotArchivURL(QString)));
   connect (&Settings,   SIGNAL(sigSetServer(int)), this, SLOT(slotSetSServer(int)));
   connect (&KartinaTv,  SIGNAL(sigGotTimerStreamURL(QString)), &timeRec, SLOT(slotTimerStreamUrl(QString)));
   connect (&timeRec,    SIGNAL(sigRecDone()), this, SLOT(slotTimerRecordDone()));
   connect (&timeRec,    SIGNAL(sigRecActive()), this, SLOT(slotTimerRecActive()));
   connect (&timeRec,    SIGNAL(sigSendStatusMsg(QString,QString)), this, SLOT(slotTimerStatusMsg(QString,QString)));
   connect (&trayIcon,   SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotSystrayActivated(QSystemTrayIcon::ActivationReason)));
   connect (this,        SIGNAL(sigHide()), &trayIcon, SLOT(show()));
   connect (this,        SIGNAL(sigShow()), &trayIcon, SLOT(hide()));
   connect (&vlcCtrl,    SIGNAL(sigVlcStarts()), this, SLOT(slotVlcStarts()));
   connect (&vlcCtrl,    SIGNAL(sigVlcEnds()), this, SLOT(slotVlcEnds()));
   connect (&timeRec,    SIGNAL(sigShutdown()), this, SLOT(slotShutdown()));

   // -------------------------------------------
   // create epg nav bar ...
   // -------------------------------------------
   TouchEpgNavi (true);

   // -------------------------------------------
   // create systray ...
   // -------------------------------------------
   CreateSystray();

   // set logo path for epg browser and timer record ...
   ui->textEpg->SetLogoDir(sLogoPath);
   timeRec.SetLogoPath(sLogoPath);

   // enable button ...
   EnableDisableDlg(false);

   // set last windows size / position ...
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

   // request authorisation ...
   Trigger.TriggerRequest(Kartina::REQ_COOKIE);

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
   // save last windows position / size
   // only, if window wasn't maximized ...
   if (windowState() != Qt::WindowMaximized)
   {
      Settings.SaveWindowRect(geometry());
   }

   delete ui;
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
   trayIcon.setParent(this);
   trayIcon.setIcon(QIcon(":/app/tv"));
   trayIcon.setToolTip(tr("vlc-record - Click to activate!"));
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
   // if vlc is running, ask if we want
   // to close it ...
   if (vlcCtrl.IsRunning())
   {
      if (WantToClose())
      {
         event->accept();
      }
      else
      {
         event->ignore();
      }
   }
   else
   {
      event->accept();
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
   if (event->key() == Qt::Key_Escape)
   {
      // ignore escape key ...
      event->ignore();
   }
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
      if ((chanlist[i].iIdx == -1) && (chanlist[i].iId == -1))
      {
         pItem = new CChanListWidgetItem (chanlist[i].sName, -1, ui->listWidget);
         pItem->setBackgroundColor(QColor(chanlist[i].sProgramm));

         // add channel group entry ...
         Pix.fill(QColor(chanlist[i].sProgramm));
         ui->cbxChannelGroup->addItem(QIcon(Pix), chanlist[i].sName, QVariant(i));
      }
      else
      {
         sLogoFile = QString("%1/%2.gif").arg(sLogoPath).arg(chanlist[i].iId);
         sLine     = QString("%1. %2").arg(chanlist[i].iIdx).arg(chanlist[i].sName);

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
                            .arg(chanlist[i].sStart).arg(chanlist[i].sEnd);

         pItem->setToolTip(sToolTip);

         pItem->SetStartEnd(chanlist[i].sStart, chanlist[i].sEnd);
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
   int       iRV      = -1;
   QDateTime now      = QDateTime::currentDateTime();
   QString   sExt     = "ts", fileName, sShowName;
   QString   sCmdLine;

   // should we ask for file name ... ?
   if (Settings.AskForRecFile())
   {
      // yes! Create file save dialog ...
      QString   sFilter;
      QString   sTarget  = QString("%1/%2(%3)").arg(Settings.GetTargetDir())
                          .arg(sChannel).arg(now.toString("yyyy-MM-dd__hh-mm"));

      fileName = QFileDialog::getSaveFileName(this, tr("Save Stream as"),
                 sTarget, QString("Transport Stream (*.ts);;AVI File (*.avi)"),
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

         QFileInfo info(fileName);

         // re-create complete file name ...
         fileName = QString ("%1/%2.%3").arg(info.path())
                    .arg(info.completeBaseName()).arg(sExt);
      }
   }
   else
   {
      // create filename as we think it's good ...
      fileName = QString("%1/%2(%3).%4").arg(Settings.GetTargetDir())
                 .arg(sChannel).arg(now.toString("yyyy-MM-dd__hh-mm")).arg(sExt);
   }

   if (fileName != "")
   {
      Q_PID vlcpid = 0;

      if (bArchiv)
      {
         // archiv using RTSP ...
         sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_REC_RTSP,
                                         Settings.GetVLCPath(),
                                         sURL, Settings.GetBufferTime(),
                                         fileName, sExt);
      }
      else
      {
         // normal stream using HTTP ...
         sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_REC_HTTP,
                                         Settings.GetVLCPath(),
                                         sURL, Settings.GetBufferTime(),
                                         fileName, sExt);
      }

      if (sCmdLine != "")
      {
         if (Settings.DetachPlayer())
         {
            mInfo(tr("Start player using folling command line:\n  --> %1").arg(sCmdLine));

            if (QProcess::startDetached(sCmdLine))
            {
               vlcpid = (Q_PID)99; // any value != 0 ...
            }
         }
         else
         {
            vlcpid = vlcCtrl.start(sCmdLine);
         }
      }

      // successfully started ?
      if (!vlcpid)
      {
         iRV = -1;
         QMessageBox::critical(this, tr("Error!"), tr("Can't start VLC-Media Player!"));
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
   int     iRV      = 0;
   Q_PID   vlcpid   = 0;
   QString sCmdLine;

   if (bArchiv)
   {
      // archiv using RTSP ...
      sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_PLAY_RTSP,
                                      Settings.GetVLCPath(), sURL,
                                      Settings.GetBufferTime());
   }
   else
   {
      // normal stream using HTTP ...
      sCmdLine = vlcCtrl.CreateClArgs(vlcctrl::VLC_PLAY_HTTP,
                                      Settings.GetVLCPath(), sURL,
                                      Settings.GetBufferTime());
   }

   if (sCmdLine != "")
   {
      if (Settings.DetachPlayer())
      {
         mInfo(tr("Start player using folling command line:\n  --> %1").arg(sCmdLine));

         if(QProcess::startDetached(sCmdLine))
         {
            vlcpid = (Q_PID)99; // any value != 0 ...
         }
      }
      else
      {
         vlcpid = vlcCtrl.start(sCmdLine);
      }
   }

   // successfully started ?
   if (!vlcpid)
   {
      iRV = -1;
      QMessageBox::critical(this, tr("Error!"), tr("Can't start VLC-Media Player!"));
   }
   else
   {
      mInfo(tr("Started VLC with pid #%1!").arg((uint)vlcpid));
   }

   return iRV;
}

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
      }

      // set language as read ...
      pTranslator->load(QString("lang_%1").arg(Settings.GetLanguage ()), QApplication::applicationDirPath());

      // give vlcCtrl needed infos ...
      vlcCtrl.LoadPlayerModule(Settings.GetPlayerModule());
      vlcCtrl.SetTranslitSettings(Settings.TranslitRecFile());

      EnableDisableDlg(false);

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
   EnableDisableDlg();
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
   XMLParser.SetByteArray(str.toUtf8());

   chanList = XMLParser.ParseChannelList(Settings.FixTime());

   // set timeshift in epg browser ...
   ui->textEpg->SetTimeShift(XMLParser.GetTimeShift());

   FillChannelList(chanList);

   // set channel list in timeRec class ...
   timeRec.SetTimeShift(XMLParser.GetTimeShift());
   timeRec.SetChanList(chanList);
   timeRec.StartTimer();

   // only download channel logos, if they aren't there ...
   if (!dwnLogos.IsRunning() && !bLogosReady)
   {
      dwnLogos.SetChanList(chanList);
   }

   // mark current timeshift value ...
   int iIdx = ui->cbxTimeShift->findText(QString::number(XMLParser.GetTimeShift()));
   ui->cbxTimeShift->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   EnableDisableDlg();
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
   QString              sChan, sUrl;

   mInfo(tr("%1 sends following url:\n  --> %2").arg(COMPANY_NAME).arg(str));

   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   XMLParser.SetByteArray(str.toUtf8());

   sUrl = XMLParser.ParseURL();

   if (pItem)
   {
      sChan = CleanShowName(pItem->GetProgram());

      if (sChan == "")
      {
         sChan = pItem->text();
      }
   }

   if (bRecord)
   {
      StartVlcRec(sUrl, sChan);
   }
   else
   {
      StartVlcPlay(sUrl);
   }

   EnableDisableDlg();
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
void Recorder::slotCookie()
{
   Trigger.TriggerRequest(Kartina::REQ_CHANNELLIST);
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
   QString                sDlgTitle;
   QVector<cparser::SEpg> epg;
   int                    cid     = 0;
   uint                   uiGmt   = 0;
   bool                   bArchiv = false;
   QDateTime              epgTime = QDateTime::currentDateTime().addDays(iEpgOffset);


   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   if (pItem)
   {
      sDlgTitle = pItem->GetName();
   }

   XMLParser.SetByteArray(str.toUtf8());

   epg = XMLParser.ParseEpg(cid, uiGmt, bArchiv);

   ui->textEpg->DisplayEpg(epg, sDlgTitle, cid, epgTime.toTime_t(), bArchiv);

   // fill epg controll ...
   if (QFile::exists(QString("%1/%2.gif").arg(sLogoPath).arg(cid)))
   {
      QPixmap pic;
      pic.load(QString("%1/%2.gif").arg(sLogoPath).arg(cid));
      ui->labChanIcon->setPixmap(pic);
   }

   ui->labChanName->setText(sDlgTitle);
   ui->labCurrDay->setText(epgTime.toString("dd. MMM. yyyy"));

   pEpgNavbar->setCurrentIndex(epgTime.date().dayOfWeek() - 1);

   EnableDisableDlg();
   ui->listWidget->setFocus(Qt::OtherFocusReason);
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
void Recorder::slotTimeShift ()
{
   Trigger.TriggerRequest(Kartina::REQ_CHANNELLIST);
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
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   if (pItem && (pItem->GetId() != -1))
   {
      EnableDisableDlg(false);
      bRecord = true;
      Trigger.TriggerRequest(Kartina::REQ_STREAM, pItem->GetId());
   }
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
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   if (pItem && (pItem->GetId() != -1))
   {
      EnableDisableDlg(false);
      bRecord = false;
      Trigger.TriggerRequest(Kartina::REQ_STREAM, pItem->GetId());
   }
}

/* -----------------------------------------------------------------\
|  Method: on_cbxTimeShift_currentIndexChanged
|  Begin: 19.01.2010 / 16:13:03
|  Author: Jo2003
|  Description: set new timeshift
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_cbxTimeShift_currentIndexChanged(QString str)
{
   EnableDisableDlg(false);
   Trigger.TriggerRequest(Kartina::REQ_TIMESHIFT, str.toInt());
}

/* -----------------------------------------------------------------\
|  Method: EnableDisableDlg
|  Begin: 19.01.2010 / 16:13:30
|  Author: Jo2003
|  Description: enable / disable buttons
|
|  Parameters: enable flag
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::EnableDisableDlg (bool bEnable)
{
   if (bEnable && (bPendingRecord || bVlcRuns))
   {
      bEnable = false;
   }

   ui->cbxTimeShift->setEnabled(bEnable);
   ui->pushPlay->setEnabled(bEnable);
   ui->pushRecord->setEnabled(bEnable);

   ui->pushStop->setDisabled(bEnable);
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
   if (pItem && (pItem->GetId() != -1))
   {
      ui->textEpgShort->setHtml(QString(TMPL_BACKCOLOR).arg("rgb(255, 254, 212)").arg(pItem->toolTip()));
      SetProgress (pItem->GetStartTime(), pItem->GetEndTime());

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
|  Method: SetProgress
|  Begin: 19.01.2010 / 16:15:17
|  Author: Jo2003
|  Description: set progress bar (program time)
|
|  Parameters: start and stop time as string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::SetProgress(const QString &start, const QString &end)
{
   int iPercent = 0;

   if ((start != "") && (end != ""))
   {
      QDateTime dtStart  = QDateTime::fromString(start, DEF_TIME_FORMAT);
      QDateTime dtEnd    = QDateTime::fromString(end, DEF_TIME_FORMAT);
      QDateTime dtNow    = QDateTime::currentDateTime();

      int       iLength  = (int)(dtEnd.toTime_t() - dtStart.toTime_t());
      int       iNow     = (int)(dtNow.toTime_t() - dtStart.toTime_t());

      // error check (div / 0 PC doesn't like ;-) ) ...
      if ((iNow > 0) && (iLength > 0))
      {
         // get percent ...
         iPercent        = (int)((iNow * 100) / iLength);
      }
   }

   ui->progressBar->setValue(iPercent);
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
   CAboutDialog dlg;
   dlg.setParent(this, Qt::Dialog);
   dlg.exec();
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
      ok      = true;
      bRecord = true;
   }
   else if (action == "archivplay")
   {
      ok      = true;
      bRecord = false;
   }
   else if(action == "timerrec")
   {
      uint uiStart = link.encodedQueryItemValue(QByteArray("start")).toUInt();
      uint uiEnd   = link.encodedQueryItemValue(QByteArray("end")).toUInt();
      int  iChan   = link.encodedQueryItemValue(QByteArray("cid")).toInt();

      timeRec.SetRecInfo(uiStart, uiEnd, iChan, CleanShowName(ui->textEpg->ShowName(uiStart)));
      timeRec.exec();
   }

   if (ok)
   {
      if (!bPendingRecord)
      {
         QString cid    = link.encodedQueryItemValue(QByteArray("cid"));
         QString gmt    = link.encodedQueryItemValue(QByteArray("gmt"));
         QString req    = QString("m=channels&act=get_stream_url&cid=%1&gmt=%2")
                          .arg(cid.toInt()).arg(gmt.toUInt());

         uiArchivGmt    = gmt.toUInt();

         Trigger.TriggerRequest(Kartina::REQ_ARCHIV, req);
      }
      else
      {
         QMessageBox::warning(this, tr("Warning!"), tr("Timer Record active!"));
      }
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
      QVector<cparser::SChan> tmpChanList;
      cparser::SChan          listEntry;
      CChanListWidgetItem    *pItem = NULL;

      // create tmp channel list with channels from listWidget ...
      for (int i = 0; i < ui->listWidget->count(); i++)
      {
         pItem = (CChanListWidgetItem *)ui->listWidget->item(i);

         if (pItem->GetId() > -1)
         {
            listEntry.iId  = pItem->GetId();
            listEntry.iIdx = 0;

            tmpChanList.push_back(listEntry);
         }
      }

      dwnLogos.SetChanList(tmpChanList);
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
   if (pItem && (pItem->GetId() != -1))
   {
      // set actual day in previous week to munday ...
      int iActDay  = pEpgNavbar->currentIndex();
      iEpgOffset  -= 7 + iActDay;
      Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
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
   if (pItem && (pItem->GetId() != -1))
   {
      // set actual day in next week to munday ...
      int iActDay  = pEpgNavbar->currentIndex();
      iEpgOffset  += 7 - iActDay;
      Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
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
   QString              sChan, sUrl;

   mInfo(tr("%1 sends following url:\n  --> %2").arg(COMPANY_NAME).arg(str));

   sChan = CleanShowName(ui->textEpg->ShowName(uiArchivGmt));

   if (sChan == "")
   {
      // if we can't get name from show, use channel name ...
      CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

      if (pItem)
      {
         sChan = pItem->text();
      }
   }

   XMLParser.SetByteArray(str.toUtf8());

   sUrl = XMLParser.ParseArchivURL();

   if (bRecord)
   {
      StartVlcRec(sUrl, sChan, true);
   }
   else
   {
      StartVlcPlay(sUrl, true);
   }

   EnableDisableDlg();
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
   if (pItem && (pItem->GetId() != -1))
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
   if (!bPendingRecord)
   {
      CChanListWidgetItem *pItem = (CChanListWidgetItem *)item;

      if (pItem && (pItem->GetId() != -1))
      {
         bRecord = false;
         EnableDisableDlg(false);
         Trigger.TriggerRequest(Kartina::REQ_STREAM, pItem->GetId());
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
void Recorder::slotSetSServer(int iSrv)
{
   Trigger.TriggerRequest(Kartina::REQ_SERVER, iSrv);
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
   timeRec.SetRecInfo(now, now, -1);
   timeRec.exec();
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
   if (bPendingRecord)
   {
      mInfo(tr("timeRec reports: record done!"));
      bPendingRecord = false;
      EnableDisableDlg();
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
void Recorder::slotTimerRecActive()
{
   mInfo(tr("timeRec reports: record active!"));
   bPendingRecord = true;
   EnableDisableDlg(false);
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
void Recorder::slotVlcEnds()
{
   if (bVlcRuns)
   {
      mInfo(tr("vlcCtrl reports: vlc player ended!"));
      bVlcRuns = false;
      EnableDisableDlg();
   }
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
void Recorder::slotVlcStarts()
{
   mInfo(tr("vlcCtrl reports: vlc player active!"));
   bVlcRuns = true;
   EnableDisableDlg(false);
}

/* -----------------------------------------------------------------\
|  Method: slotTimerStatusMsg
|  Begin: 26.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: change record timer status message
|
|  Parameters: message, color as string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotTimerStatusMsg(const QString &sMsg, const QString &sColor)
{
   if (ui->labTimerInfo->text() != sMsg)
   {
      ui->labTimerInfo->setText(sMsg);
      ui->labTimerInfo->setStyleSheet(QString(LABEL_STYLE).arg("labTimerInfo").arg(sColor));
   }
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
   if (vlcCtrl.IsRunning())
   {
      if (WantToQuitVlc())
      {
         vlcCtrl.stop();
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: accept
|  Begin: 30.01.2010 / 13:58:25
|  Author: Jo2003
|  Description: override accept slot to ask if we want to close
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::accept()
{
   // if vlc is running, ask if we want
   // to close it ...
   if (vlcCtrl.IsRunning())
   {
      if (WantToClose())
      {
         QDialog::accept();
      }
   }
   else
   {
      QDialog::accept();
   }
}

/* -----------------------------------------------------------------\
|  Method: WantToQuitVlc
|  Begin: 02.02.2010 / 10:05:00
|  Author: Jo2003
|  Description: ask if we want to clse vlc
|
|  Parameters: --
|
|  Returns: true --> close
|          false --> don't close
\----------------------------------------------------------------- */
bool Recorder::WantToQuitVlc()
{
   QString sText = HTML_SITE;
   sText.replace(TMPL_CONT, tr("VLC is still running."
                               "<br /> <br />"
                               "Do you really want to close the VLC Player now?"));

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
   ui->textEpg->ReduceFont();
   ui->textEpgShort->ReduceFont();
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
   ui->textEpg->EnlargeFont();
   ui->textEpgShort->EnlargeFont();
}

/************************* History ***************************\
| $Log$
\*************************************************************/

