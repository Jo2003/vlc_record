/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/recorder.cpp $
| 
| Author: Joerg Neubert
|
| Begin: 19.01.2010 / 16:01:09
| 
| Last edited by: $Author: joergn $
| 
| $Id: recorder.cpp 176 2010-01-19 15:29:52Z joergn $
\*************************************************************/
#include "recorder.h"
#include "ui_recorder.h"
#include "chanlistwidgetitem.h"

/* -----------------------------------------------------------------\
|  Method: Recorder / constructor
|  Begin: 19.01.2010 / 16:01:44
|  Author: Joerg Neubert
|  Description: create recorder object, init values,
|               make connections
|
|  Parameters: pointer to translator, pointer to parent widget
|
|  Returns: --
\----------------------------------------------------------------- */
Recorder::Recorder(QTranslator *trans, QWidget *parent)
    : QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint),
    ui(new Ui::Recorder)
{
   ui->setupUi(this);

   bRecord      = true;
   bLogosReady  = false;
   pTranslator  = trans;
   iEpgOffset   = 0;
   pSettings    = new CSettingsDlg(pTranslator, this);
   sLogoPath    = dwnLogos.GetLogoPath();

   ui->textEpg->SetLogoDir(sLogoPath);

   VlcLog.SetLogFile(QString(INI_DIR).arg(getenv(APPDATA)).toLocal8Bit().data(), "vlc-record.log");

   // if main settings aren't done, start settings dialog ...
   if (pSettings &&
       ((pSettings->GetPasswd() == "")
        || (pSettings->GetVLCPath() == "")
        || (pSettings->GetUser() == "")))
   {
      pSettings->exec();
   }

   // set log level ...
   VlcLog.SetLogLevel(pSettings->GetLogLevel());

   // set connection data ...
   KartinaTv.SetData(KARTINA_HOST, pSettings->GetUser(), pSettings->GetPasswd(), pSettings->AllowEros());


   // set proxy stuff ...
   if (pSettings->UseProxy())
   {
      KartinaTv.setProxy(pSettings->GetProxyHost(), pSettings->GetProxyPort(),
                         pSettings->GetProxyUser(), pSettings->GetProxyPasswd());

      dwnLogos.setProxy(pSettings->GetProxyHost(), pSettings->GetProxyPort(),
                        pSettings->GetProxyUser(), pSettings->GetProxyPasswd());
   }

   // configure trigger and start it ...
   Trigger.SetKartinaClient(&KartinaTv);
   Trigger.start();

   // connect signals and slots ...
   connect (&KartinaTv, SIGNAL(sigError(QString)), this, SLOT(slotErr(QString)));
   connect (&KartinaTv, SIGNAL(sigGotChannelList(QString)), this, SLOT(slotChanList(QString)));
   connect (&KartinaTv, SIGNAL(sigGotStreamURL(QString)), this, SLOT(slotStreamURL(QString)));
   connect (&KartinaTv, SIGNAL(sigGotCookie()), this, SLOT(slotCookie()));
   connect (&KartinaTv, SIGNAL(sigGotEPG(QString)), this, SLOT(slotEPG(QString)));
   connect (&KartinaTv, SIGNAL(sigTimeShiftSet()), this, SLOT(slotTimeShift()));
   connect (&Refresh,   SIGNAL(timeout()), &Trigger, SLOT(slotReqChanList()));
   connect (ui->textEpg, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotArchivAnchor(QUrl)));
   connect (&dwnLogos, SIGNAL(sigLogosReady()), this, SLOT(slotLogosReady()));
   connect (pSettings, SIGNAL(sigReloadLogos()), this, SLOT(slotReloadLogos()));
   connect (&KartinaTv, SIGNAL(sigGotArchivURL(QString)), this, SLOT(slotArchivURL(QString)));

   // enebale button ...
   EnableDisableDlg(false);

   // request authorisation ...
   Trigger.TriggerRequest(Kartina::REQ_COOKIE);

   // start refresh timer, if needed ...
   if (pSettings->DoRefresh())
   {
      Refresh.start(pSettings->GetRefrInt() * 60000); // 1 minutes: (60 * 1000 msec) ...
   }
}

/* -----------------------------------------------------------------\
|  Method: ~Recorder / dstructor
|  Begin: 19.01.2010 / 16:04:40
|  Author: Joerg Neubert
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
Recorder::~Recorder()
{
   if (pSettings)
   {
      delete pSettings;
      pSettings = NULL;
   }

   delete ui;
}

/* -----------------------------------------------------------------\
|  Method: changeEvent
|  Begin: 19.01.2010 / 16:05:00
|  Author: Joerg Neubert
|  Description: catch language change event
|
|  Parameters: pointer to event
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/* -----------------------------------------------------------------\
|  Method: FillChannelList
|  Begin: 19.01.2010 / 16:05:24
|  Author: Joerg Neubert
|  Description: fill channel list
|
|  Parameters: vector with channel elements
|
|  Returns: 0
\----------------------------------------------------------------- */
int Recorder::FillChannelList (QVector<cparser::SChan> chanlist)
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

         // create tool tip with programm info ...
         sToolTip = tr("<b style='color: red;'>%1</b><br>\n"
                       "<b>Programm:</b> %2<br>\n"
                       "<b>Start:</b> %3<br>\n<b>End:</b> %4")
                       .arg(chanlist[i].sName).arg(chanlist[i].sProgramm)
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
|  Author: Joerg Neubert
|  Description: start VLC to record stream
|
|  Parameters: stream url, channel name, archiv flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int Recorder::StartVlcRec (const QString &sURL, const QString &sChannel, bool bArchiv)
{
   QDateTime now    = QDateTime::currentDateTime();
   QString sTarget  = QString("%1\\%2_%3").arg(pSettings->GetTargetDir()).arg(sChannel).arg(now.toString("yyyy-MM-dd_hh-mm-ss"));
   QString fileName = QFileDialog::getSaveFileName(this, tr("Save Stream as"),
                       sTarget, tr("Transport Stream (*.ts);;AVI File (*.avi)"));
   QRegExp rx("^.*[.]{1}([a-zA-Z]*)$");
   if(rx.indexIn(fileName) > -1)
   {
      QString sCmdLine = VLC_REC_TEMPL;
      sCmdLine.replace(TMPL_VLC, pSettings->GetVLCPath());
      sCmdLine.replace(TMPL_URL, sURL);
      sCmdLine.replace(TMPL_MUX, rx.cap(1));
      sCmdLine.replace(TMPL_DST, fileName);

      if (bArchiv)
      {
         sCmdLine += " --rtsp-tcp";
      }

      // Start the QProcess instance.
      QProcess::startDetached(sCmdLine);
   }
   else
   {
      QMessageBox::critical(this, tr("Error!"), tr("Can't recognice file extension!"));
   }

   /* O.k., everything is fine now, leave the Qt application. The external program
    * will continue running.
    */
   return 0;
}

/* -----------------------------------------------------------------\
|  Method: StartVlcPlay
|  Begin: 19.01.2010 / 16:06:16
|  Author: Joerg Neubert
|  Description: start VLC to show stream
|
|  Parameters: stream url, archiv flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int Recorder::StartVlcPlay (const QString &sURL, bool bArchiv)
{
   QString sCmdLine = VLC_PLAY_TEMPL;
   sCmdLine.replace(TMPL_VLC, pSettings->GetVLCPath());
   sCmdLine.replace(TMPL_URL, sURL);

   if (bArchiv)
   {
      sCmdLine += " --rtsp-tcp";
   }

   // Start the QProcess instance.
   QProcess::startDetached(sCmdLine);

   /* O.k., everything is fine now, leave the Qt application. The external program
    * will continue running.
    */
   return 0;
}

/* -----------------------------------------------------------------\
|  Method: on_pushSettings_clicked
|  Begin: 19.01.2010 / 16:07:19
|  Author: Joerg Neubert
|  Description: show settings dialog
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushSettings_clicked()
{
   if (pSettings->exec() == QDialog::Accepted)
   {
      // if changes where saved, accept it here ...
      VlcLog.SetLogLevel(pSettings->GetLogLevel());

      ui->listWidget->clear();
      KartinaTv.abort();

      // update connection data ...
      KartinaTv.SetData(KARTINA_HOST, pSettings->GetUser(), pSettings->GetPasswd(), pSettings->AllowEros());

      // set proxy ...
      if (pSettings->UseProxy())
      {
         KartinaTv.setProxy(pSettings->GetProxyHost(), pSettings->GetProxyPort(),
                            pSettings->GetProxyUser(), pSettings->GetProxyPasswd());

         dwnLogos.setProxy(pSettings->GetProxyHost(), pSettings->GetProxyPort(),
                           pSettings->GetProxyUser(), pSettings->GetProxyPasswd());
      }

      EnableDisableDlg(false);

      // authenticate ...
      Trigger.TriggerRequest(Kartina::REQ_COOKIE);

      // set refresh timer ...
      if (pSettings->DoRefresh())
      {
         if (!Refresh.isActive())
         {
            Refresh.start(pSettings->GetRefrInt() * 60000); // 1 minutes: (60 * 1000 msec) ...
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
|  Author: Joerg Neubert
|  Description: display errors signaled by other threads
|
|  Parameters: error string
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotErr(QString str)
{
   QMessageBox::critical(this, tr("Error"),
                         tr("Kartina.tv Client API reports some errors: %1").arg(str));
   EnableDisableDlg();
}

/* -----------------------------------------------------------------\
|  Method: slotChanList
|  Begin: 19.01.2010 / 16:09:23
|  Author: Joerg Neubert
|  Description: handle requested channel list
|
|  Parameters: channel list (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotChanList (QString str)
{
   VlcLog.LogInfo(QString("%1\n").arg(__FUNCTION__));
   QVector<cparser::SChan> chanList;
   XMLParser.SetByteArray(str.toUtf8());

   chanList = XMLParser.ParseChannelList(pSettings->FixTime());

   // set timeshift in epg browser ...
   ui->textEpg->SetTimeShift(XMLParser.GetTimeShift());

   FillChannelList(chanList);

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
|  Author: Joerg Neubert
|  Description: handle requested stream url
|
|  Parameters: stream url (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotStreamURL(QString str)
{
   QString              sChan, sUrl;
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   XMLParser.SetByteArray(str.toUtf8());

   sUrl = XMLParser.ParseURL();

   if (pItem)
   {
      sChan = pItem->text();
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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

   EnableDisableDlg();
   ui->listWidget->setFocus(Qt::OtherFocusReason);
}

/* -----------------------------------------------------------------\
|  Method: slotTimeShift
|  Begin: 19.01.2010 / 16:11:30
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
|  Description: enable / disable buttons
|
|  Parameters: enable flag
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::EnableDisableDlg (bool bEnable)
{
   ui->cbxTimeShift->setEnabled(bEnable);
   ui->listWidget->setEnabled(bEnable);
   ui->pushPlay->setEnabled(bEnable);
   ui->pushRecord->setEnabled(bEnable);
}

/* -----------------------------------------------------------------\
|  Method: on_listWidget_currentRowChanged
|  Begin: 19.01.2010 / 16:13:56
|  Author: Joerg Neubert
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
         // channel changed ...
         iEpgOffset = 0;

         // load epg ...
         Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId());
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
|  Description: show about dialog 
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_pushAbout_clicked()
{
   CAboutDialog dlg;
   dlg.exec();
}

/* -----------------------------------------------------------------\
|  Method: slotArchivAnchor
|  Begin: 19.01.2010 / 16:16:17
|  Author: Joerg Neubert
|  Description: archiv link in epg browser was clicked,
|               request archiv stream url for play / record
|  Parameters: clicked link
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotArchivAnchor (const QUrl &link)
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

   if (ok)
   {
      QString cid    = link.encodedQueryItemValue(QByteArray("cid"));
      QString gmt    = link.encodedQueryItemValue(QByteArray("gmt"));
      QString req    = QString("m=channels&act=get_stream_url&cid=%1&gmt=%2")
                       .arg(cid.toInt()).arg(gmt.toUInt());

      Trigger.TriggerRequest(Kartina::REQ_ARCHIV, req);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotLogosReady
|  Begin: 19.01.2010 / 16:17:23
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Method: on_btnBack_clicked
|  Begin: 19.01.2010 / 16:18:30
|  Author: Joerg Neubert
|  Description: one day backward in epg 
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnBack_clicked()
{
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();
   if (pItem && (pItem->GetId() != -1))
   {
      iEpgOffset --;
      Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
   }
}

/* -----------------------------------------------------------------\
|  Method: on_btnNext_clicked
|  Begin: 19.01.2010 / 16:18:30
|  Author: Joerg Neubert
|  Description: one day forward in epg 
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::on_btnNext_clicked()
{
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();
   if (pItem && (pItem->GetId() != -1))
   {
      iEpgOffset ++;
      Trigger.TriggerRequest(Kartina::REQ_EPG, pItem->GetId(), iEpgOffset);
   }
}

/* -----------------------------------------------------------------\
|  Method: slotArchivURL
|  Begin: 19.01.2010 / 16:19:25
|  Author: Joerg Neubert
|  Description: got requested archiv url, start play / record
|
|  Parameters: archiv url (xml)
|
|  Returns: --
\----------------------------------------------------------------- */
void Recorder::slotArchivURL(QString str)
{
   QString              sChan, sUrl;
   CChanListWidgetItem *pItem = (CChanListWidgetItem *)ui->listWidget->currentItem();

   XMLParser.SetByteArray(str.toUtf8());

   sUrl = XMLParser.ParseArchivURL();

   if (pItem)
   {
      sChan = pItem->text();
   }

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

/************************* History ***************************\
| $Log$
\*************************************************************/

