/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:43:08
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "csettingsdlg.h"
#include "ui_csettingsdlg.h"

// log file functions ...
extern CLogFile VlcLog;

// for folders ...
extern CDirStuff *pFolders;

// storage db ...
extern CVlcRecDB *pDb;

/* -----------------------------------------------------------------\
|  Method: CSettingsDlg / constructor
|  Begin: 19.01.2010 / 15:43:38
|  Author: Jo2003
|  Description: construct dialog, init values
|
|  Parameters: pointer to translater, pointer to parent widget
|
|  Returns: --
\----------------------------------------------------------------- */
CSettingsDlg::CSettingsDlg(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CSettingsDlg)
{
   m_ui->setupUi(this);

   pShortApiServer = new CShortcutEx(QKeySequence("CTRL+ALT+A"), this);
   pShortVerbLevel = new CShortcutEx(QKeySequence("CTRL+ALT+V"), this);

   if (pShortApiServer)
   {
      connect(pShortApiServer, SIGNAL(activated()), this, SLOT(slotEnableApiServer()));
   }

   if (pShortVerbLevel)
   {
      connect(pShortVerbLevel, SIGNAL(activated()), this, SLOT(slotEnableVlcVerbLine()));
   }


   // set company name for login data ...
   QString s = m_ui->groupAccount->title();
   m_ui->groupAccount->setTitle(s.arg(COMPANY_NAME));

   // fill in buffer values ...
   vBuffs << 1.5 << 3 << 5 << 8 << 15 << 20 << 30 << 45 << 60 << 90;
   qSort(vBuffs);

#ifdef ENABLE_AD_SWITCH
   m_ui->checkAds->setEnabled(true);
#endif // ENABLE_AD_SWITCH

   // fill in values ...
   readSettings();
}

/* -----------------------------------------------------------------\
|  Method: ~CSettingsDlg / dstructor
|  Begin: 19.01.2010 / 15:45:48
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CSettingsDlg::~CSettingsDlg()
{
   if (pShortApiServer)
   {
      delete pShortApiServer;
      pShortApiServer = NULL;
   }

   delete m_ui;
}

/* -----------------------------------------------------------------\
|  Method: readSettings
|  Begin: 13.09.2011 / 10:20
|  Author: Jo2003
|  Description: read settings from database
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::readSettings()
{
   QString     s;
   int         iErr;
   QDir        folder;
   QStringList sl;

   // line edits ...
   m_ui->lineVLC->setText (pDb->stringValue("VLCPath"));
   m_ui->lineDir->setText (pDb->stringValue("TargetDir"));
   m_ui->lineUsr->setText (pDb->stringValue("User"));
   m_ui->linePass->setText (pDb->stringValue("Passwd"));
   m_ui->lineErosPass->setText(pDb->stringValue("ErosPasswd"));
   m_ui->lineShutdown->setText(pDb->stringValue("ShutdwnCmd"));
   m_ui->lineApiServer->setText(pDb->stringValue ("APIServer"));
   m_ui->lineVlcVerbose->setText(pDb->stringValue ("libVlcLogLevel", &iErr));

   if (iErr)
   {
      // default log level ...
      m_ui->lineVlcVerbose->setText("0");
   }

#ifdef Q_OS_WIN32
   if (m_ui->lineShutdown->text() == "")
   {
      m_ui->lineShutdown->setText ("shutdown.exe -s -f -t 5");
   }
#endif

   if (m_ui->lineApiServer->text() == "")
   {
      m_ui->lineApiServer->setText (KARTINA_HOST);
   }

   // in case we use customization we should hide the API server name stuff ...
#ifdef _IS_OEM
   m_ui->lineApiServer->setVisible(false);
#endif // _IS_OEM

   m_ui->lineProxyHost->setText(pDb->stringValue("ProxyHost"));
   m_ui->lineProxyPort->setText(pDb->stringValue("ProxyPort"));
   m_ui->lineProxyUser->setText(pDb->stringValue("ProxyUser"));
   m_ui->lineProxyPassword->setText(pDb->stringValue("ProxyPasswd"));

   // check boxes ...
   m_ui->useProxy->setCheckState((Qt::CheckState)pDb->intValue("UseProxy"));
   m_ui->checkAdult->setCheckState((Qt::CheckState)pDb->intValue("AllowAdult"));
   m_ui->checkFixTime->setCheckState((Qt::CheckState)pDb->intValue("FixTime"));
   m_ui->checkRefresh->setCheckState((Qt::CheckState)pDb->intValue("Refresh"));
   m_ui->checkHideToSystray->setCheckState((Qt::CheckState)pDb->intValue("TrayHide"));
   m_ui->checkAskForName->setCheckState((Qt::CheckState)pDb->intValue("AskRecFile"));
   m_ui->checkTranslit->setCheckState((Qt::CheckState)pDb->intValue("TranslitRecFile"));
   m_ui->checkDetach->setCheckState((Qt::CheckState)pDb->intValue("DetachPlayer"));
   m_ui->checkExtChanInfo->setCheckState((Qt::CheckState)pDb->intValue("ExtChanList"));
   m_ui->checkAdvanced->setCheckState((Qt::CheckState)pDb->intValue("AdvSet"));
   m_ui->checkGPUAcc->setCheckState((Qt::CheckState)pDb->intValue("GPUAcc"));
   m_ui->checkAds->setCheckState((Qt::CheckState)pDb->intValue("AdsEnabled", &iErr));

   // value doesn't exist in database ...
   if (iErr)
   {
      // enable by default ...
      m_ui->checkAds->setCheckState(Qt::Checked);
   }

   m_ui->check2ClicksToPlay->setCheckState((Qt::CheckState)pDb->intValue("2ClickPlay", &iErr));

   // value doesn't exist in database ...
   if (iErr)
   {
      // enable by default ...
      m_ui->check2ClicksToPlay->setCheckState(Qt::Checked);
   }

   m_ui->checkUpdate->setCheckState((Qt::CheckState)pDb->intValue("UpdateCheck", &iErr));

   // value doesn't exist in database ...
   if (iErr)
   {
      // enable by default ...
      m_ui->checkUpdate->setCheckState(Qt::Checked);
   }

   m_ui->tabWidget->setTabEnabled(2, pDb->intValue("AdvSet") ? true : false);

   if (m_ui->checkAdvanced->isChecked())
   {
      vBuffs.prepend(0.5);
   }

   // on update the password for adult channels may not be given ...
   if (m_ui->checkAdult->isChecked() && (m_ui->lineErosPass->text() == ""))
   {
      m_ui->lineErosPass->setText(m_ui->linePass->text());
   }

   // fill player module box with available modules ...
   folder.setPath(pFolders->getModDir());
   m_ui->cbxPlayerMod->addItems(folder.entryList(QStringList("*.mod"), QDir::Files, QDir::Name));

   // fill language box ...
   folder.setPath(pFolders->getLangDir());
   sl = folder.entryList(QStringList("lang_*.qm"), QDir::Files, QDir::Name);

   // make sure english is part of list ...
   sl.push_front("lang_en.qm");

   QRegExp rx("^lang_([a-zA-Z]+).qm$");
   for (int i = 0; i < sl.size(); i++)
   {
      // get out language from file name ...
      if (sl.at(i).indexOf(rx) > -1)
      {
         m_ui->cbxLanguage->addItem(QIcon(QString(":/flags/%1").arg(rx.cap(1))), rx.cap(1));
      }
   }

   // combo boxes ...
   int iIdx;
   iIdx = m_ui->cbxLanguage->findText(pDb->stringValue("Language"));
   m_ui->cbxLanguage->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   // fill buffer box with text in seconds, but data in msec ...
   for (QVector<float>::const_iterator cit = vBuffs.constBegin(); cit != vBuffs.constEnd(); cit ++)
   {
      m_ui->cbxBufferSeconds->insertItem(vBuffs.count(), QString("%1").arg(*cit), (int)((*cit) * 1000.0));
   }

   iIdx = m_ui->cbxBufferSeconds->findData(pDb->intValue("HttpCache"));
   m_ui->cbxBufferSeconds->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   iIdx = m_ui->cbxInterval->findText(pDb->stringValue("RefIntv"));
   m_ui->cbxInterval->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   m_ui->cbxLogLevel->setCurrentIndex((int)pDb->intValue("LogLevel"));

   if ((s = pDb->stringValue("PlayerModule")) == "")
   {
      // no player module chossen yet ...

#ifdef INCLUDE_LIBVLC
      // default module in version 2.xx is 5_libvlc.mod
      s = "5_libvlc.mod";
#else
      // default module in version 1.xx is 1_vlc-player.mod
      s = "1_vlc-player.mod";
#endif

   }

   iIdx = m_ui->cbxPlayerMod->findText(s);
   m_ui->cbxPlayerMod->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   // disable "minimize to tray" on mac because this isn't supported ...
#ifdef Q_OS_MAC
   m_ui->checkHideToSystray->setDisabled(true);
#endif // Q_OS_MAC

}

/* -----------------------------------------------------------------\
|  Method: changeEvent
|  Begin: 19.01.2010 / 15:46:10
|  Author: Jo2003
|  Description: catch event when language changes
|
|  Parameters: pointer to event
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
       {
          // save current index from comboboxes ...
          int iLanIdx = m_ui->cbxLanguage->currentIndex();
          int iLogIdx = m_ui->cbxLogLevel->currentIndex();
          int iBufIdx = m_ui->cbxBufferSeconds->currentIndex();
          int iModIdx = m_ui->cbxPlayerMod->currentIndex();
          int iIntIdx = m_ui->cbxInterval->currentIndex();

          m_ui->retranslateUi(this);

          // re-set index to comboboxes ...
          m_ui->cbxLanguage->setCurrentIndex(iLanIdx);
          m_ui->cbxLogLevel->setCurrentIndex(iLogIdx);
          m_ui->cbxBufferSeconds->setCurrentIndex(iBufIdx);
          m_ui->cbxPlayerMod->setCurrentIndex(iModIdx);
          m_ui->cbxInterval->setCurrentIndex(iIntIdx);

          // set company name ...
          QString s = m_ui->groupAccount->title();
          m_ui->groupAccount->setTitle(s.arg(COMPANY_NAME));
       }
       break;
    default:
        break;
    }
}

/* -----------------------------------------------------------------\
|  Method: slotEnableApiServer [slot]
|  Begin: 02.02.2011 / 12:00
|  Author: Jo2003
|  Description: enable / disable API server line
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::slotEnableApiServer()
{
#ifdef _IS_OEM
   if (!m_ui->lineApiServer->isVisible())
   {
      m_ui->lineApiServer->setVisible(true);
      m_ui->lineApiServer->setEnabled(true);
   }
   else
   {
      m_ui->lineApiServer->setEnabled(false);
      m_ui->lineApiServer->setVisible(false);
   }
#else
   if (m_ui->lineApiServer->isEnabled())
   {
      m_ui->lineApiServer->setEnabled(false);
   }
   else
   {
      m_ui->lineApiServer->setEnabled(true);
   }
#endif // _IS_OEM
}

/* -----------------------------------------------------------------\
|  Method: slotEnableVlcVerbLine [slot]
|  Begin: 08.02.2012
|  Author: Jo2003
|  Description: enable / disable libVLC verbose level server line
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::slotEnableVlcVerbLine()
{
   if (m_ui->lineVlcVerbose->isEnabled())
   {
      m_ui->lineVlcVerbose->setEnabled(false);
   }
   else
   {
      m_ui->lineVlcVerbose->setEnabled(true);
   }
}

/* -----------------------------------------------------------------\
|  Method: on_pushVLC_clicked
|  Begin: 19.01.2010 / 15:46:47
|  Author: Jo2003
|  Description: open file dialog to choose vlc binary
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_pushVLC_clicked()
{
#ifdef WIN32
   QString sFilter = tr("Executables (*.exe)");
#else
   QString sFilter = tr("Executables (*)");
#endif

   QString sVLCPath = QFileDialog::getOpenFileName(this, tr("VLC Media Player"),
                                                   m_ui->lineVLC->text(), sFilter);

#ifdef Q_OS_MAC
   // on mac get executable file name from application bundle ...
   QFileInfo fInfo (sVLCPath);

   // quick 'n' dirty check for application bundle ...
   if (fInfo.isDir() && (fInfo.suffix().toLower() == "app"))
   {
      QFile bundleInfo (sVLCPath + QString("/Contents/Info.plist"));
      if (bundleInfo.open(QIODevice::ReadOnly))
      {
         QString infoString = bundleInfo.readAll();
         QRegExp rx("<key>CFBundleExecutable</key>[^<]*<string>([^<]*)</string>");

         // use reg. expressions instead of xml stream parser ...
         if (rx.indexIn(infoString) > -1)
         {
            sVLCPath += QString("/Contents/MacOS/%1").arg(rx.cap(1));
         }
      }
   }
#endif // Q_OS_MAC

   m_ui->lineVLC->setText(sVLCPath);
}

/* -----------------------------------------------------------------\
|  Method: on_pushDir_clicked
|  Begin: 19.01.2010 / 15:47:19
|  Author: Jo2003
|  Description: open dir dialog to choose target dir for records
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_pushDir_clicked()
{
   QString sTargetDir = QFileDialog::getExistingDirectory(this, tr("Target Folder"),
                                                          m_ui->lineDir->text());

   m_ui->lineDir->setText(sTargetDir);
}

/* -----------------------------------------------------------------\
|  Method: on_pushSave_clicked
|  Begin: 19.01.2010 / 15:47:50
|  Author: Jo2003
|  Description: save all settings to ini file
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_pushSave_clicked()
{
   // line edits ...
   pDb->setValue("VLCPath", m_ui->lineVLC->text());
   pDb->setValue("User", m_ui->lineUsr->text());
   pDb->setValue("TargetDir", m_ui->lineDir->text());
   pDb->setValue("Passwd", m_ui->linePass->text());
   pDb->setValue("ErosPasswd", m_ui->lineErosPass->text());

   pDb->setValue("ProxyHost", m_ui->lineProxyHost->text());
   pDb->setValue("ProxyPort", m_ui->lineProxyPort->text());
   pDb->setValue("ProxyUser", m_ui->lineProxyUser->text());
   pDb->setValue("ProxyPasswd", m_ui->lineProxyPassword->text());
   pDb->setValue("ShutdwnCmd", m_ui->lineShutdown->text());
   pDb->setValue("APIServer", m_ui->lineApiServer->text());
   pDb->setValue("libVlcLogLevel", m_ui->lineVlcVerbose->text());

   // check boxes ...
   pDb->setValue("UseProxy", (int)m_ui->useProxy->checkState());
   pDb->setValue("AllowAdult", (int)m_ui->checkAdult->checkState());
   pDb->setValue("FixTime", (int)m_ui->checkFixTime->checkState());
   pDb->setValue("Refresh", (int)m_ui->checkRefresh->checkState());
   pDb->setValue("TrayHide", (int)m_ui->checkHideToSystray->checkState());
   pDb->setValue("AskRecFile", (int)m_ui->checkAskForName->checkState());
   pDb->setValue("TranslitRecFile", (int)m_ui->checkTranslit->checkState());
   pDb->setValue("DetachPlayer", (int)m_ui->checkDetach->checkState());
   pDb->setValue("ExtChanList", (int)m_ui->checkExtChanInfo->checkState());
   pDb->setValue("AdvSet", (int)m_ui->checkAdvanced->checkState());
   pDb->setValue("2ClickPlay", (int)m_ui->check2ClicksToPlay->checkState());
   pDb->setValue("UpdateCheck", (int)m_ui->checkUpdate->checkState());
   pDb->setValue("GPUAcc", (int)m_ui->checkGPUAcc->checkState());
   pDb->setValue("AdsEnabled", (int)m_ui->checkAds->checkState());

   // combo boxes ...
   pDb->setValue("Language", m_ui->cbxLanguage->currentText());
   pDb->setValue("HttpCache", m_ui->cbxBufferSeconds->itemData(m_ui->cbxBufferSeconds->currentIndex()).toInt());
   pDb->setValue("LogLevel", m_ui->cbxLogLevel->currentIndex());
   pDb->setValue("PlayerModule", m_ui->cbxPlayerMod->currentText());
   pDb->setValue("RefIntv", m_ui->cbxInterval->currentText());

   // short cuts ...
   CShortCutGrabber *pGrab;
   for (int i = 0; i < m_ui->tableShortCuts->rowCount(); i++)
   {
      pGrab = (CShortCutGrabber *)m_ui->tableShortCuts->cellWidget(i, 1);
      pDb->setShortCut(pGrab->target(), pGrab->slot(), pGrab->shortCutString());
   }
}

/* -----------------------------------------------------------------\
|  Method: on_checkAdvanced_clicked
|  Begin: 14.09.2011 / 08:45
|  Author: Jo2003
|  Description: enable advanced settings tab
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_checkAdvanced_clicked(bool checked)
{
   m_ui->tabWidget->setTabEnabled(2, checked);

   int iIdx = m_ui->cbxBufferSeconds->findData(500);

   if (checked)
   {
      if (iIdx < 0)
      {
         m_ui->cbxBufferSeconds->insertItem(-1, QString("%1").arg(0.5), 500);
      }
   }
   else
   {
      if (iIdx > -1)
      {
         m_ui->cbxBufferSeconds->removeItem(iIdx);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: on_pushDelLogos_clicked
|  Begin: 19.01.2010 / 15:48:39
|  Author: Jo2003
|  Description: delete all former downloaded channel logos
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_pushDelLogos_clicked()
{
   // get directory, where logos are located ...
   QDir logoDir(pFolders->getLogoDir());

   // get all files inside this dir ...
   QStringList fileList = logoDir.entryList(QDir::Files);

   // delete all files inside logo dir ...
   for (int i = 0; i < fileList.size(); i++)
   {
      QFile::remove(QString("%1/%2").arg(pFolders->getLogoDir()).arg(fileList[i]));
   }

   emit sigReloadLogos();
}

/* -----------------------------------------------------------------\
|  Method: SetStreamServerCbx
|  Begin: 28.02.2010 / 16:55:39
|  Author: Jo2003
|  Description: fill / mark combobox for stream server
|
|  Parameters: ref. to server list, act server
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::SetStreamServerCbx (const QVector<cparser::SSrv> &vSrvList, const QString &sActSrv)
{
   int iActIdx = 0;
   int iCount  = 0;
   QVector<cparser::SSrv>::const_iterator cit;


   m_ui->cbxStreamServer->clear();

   // add all servers ...
   for (cit = vSrvList.constBegin(); cit != vSrvList.constEnd(); cit++)
   {
      m_ui->cbxStreamServer->addItem((*cit).sName, QVariant((*cit).sIp));

      if ((*cit).sIp == sActSrv)
      {
         iActIdx = iCount;
      }

      iCount ++;
   }

   // mark active server ...
   m_ui->cbxStreamServer->setCurrentIndex(iActIdx);
}

/* -----------------------------------------------------------------\
|  Method: SetBitrateCbx
|  Begin: 14.01.2011 / 14:15
|  Author: Jo2003
|  Description: fill / mark combobox for bitrate
|
|  Parameters: ref. to bitrate vector, act bitrate
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::SetBitrateCbx (const QVector<int>& vValues, int iActrate)
{
   int iActIdx = 0;
   int iCount  = 0;
   QVector<int>::const_iterator cit;
   QString sName;


   m_ui->cbxBitRate->clear();

   // add all available bitrates ...
   for (cit = vValues.constBegin(); cit != vValues.constEnd(); cit++)
   {
      // build name ...
      switch (*cit)
      {
      case 320:
         sName = tr("Mobile");
         break;

      case 900:
         sName = tr("Eco");
         break;

      case 1500:
         sName = tr("Standard");
         break;

      case 2500:
         sName = tr("Premium");
         break;

      default:
         sName = tr("%1 Kbit/s").arg(*cit);
         break;
      }

      m_ui->cbxBitRate->addItem(sName, QVariant(*cit));

      if (*cit == iActrate)
      {
         iActIdx = iCount;
      }

      iCount ++;
   }

   // mark active rate ...
   m_ui->cbxBitRate->setCurrentIndex(iActIdx);
}

/* -----------------------------------------------------------------\
|  Method: fillTimeShiftCbx
|  Begin: 14.09.2011 / 09:30
|  Author: Jo2003
|  Description: fill / mark combobox for timeshift
|
|  Parameters: ref. to timeshift vector, act timeshift
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::fillTimeShiftCbx(const QVector<int> &vVals, int iAct)
{
   int iActIdx = 0;
   int iCount  = 0;
   QVector<int>::const_iterator cit;

   m_ui->cbxTimeShift->clear();

   // add all available timeshift values ...
   for (cit = vVals.constBegin(); cit != vVals.constEnd(); cit++)
   {
      m_ui->cbxTimeShift->addItem(QString::number(*cit), QVariant(*cit));

      if (*cit == iAct)
      {
         iActIdx = iCount;
      }

      iCount ++;
   }

   // mark active rate ...
   m_ui->cbxTimeShift->setCurrentIndex(iActIdx);
}

/* -----------------------------------------------------------------\
|  Method: on_cbxStreamServer_activated
|  Begin: 14.09.2011 / 09:40
|  Author: Jo2003
|  Description: signal set of stream server
|
|  Parameters: current index
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_cbxStreamServer_activated(int index)
{
   emit sigSetServer(m_ui->cbxStreamServer->itemData(index).toString());
}

/* -----------------------------------------------------------------\
|  Method: on_cbxBitRate_activated
|  Begin: 14.09.2011 / 09:40
|  Author: Jo2003
|  Description: set bitrate
|
|  Parameters: actual index
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_cbxBitRate_activated(int index)
{
   emit sigSetBitRate(m_ui->cbxBitRate->itemData(index).toInt());
}

/* -----------------------------------------------------------------\
|  Method: on_cbxTimeShift_activated
|  Begin: 14.09.2011 / 09:40
|  Author: Jo2003
|  Description: set timeshift
|
|  Parameters: actual index
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_cbxTimeShift_activated(int index)
{
   emit sigSetTimeShift(m_ui->cbxTimeShift->itemData(index).toInt());
}

/* -----------------------------------------------------------------\
|  Method: SaveWindowRect
|  Begin: 27.01.2010 / 11:22:39
|  Author: Jo2003
|  Description: save windows position in ini file
|
|  Parameters: windows position / size
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::SaveWindowRect (const QRect &wnd)
{
   QString sGeo = QString("%1;%2;%3;%4").arg(wnd.x()).arg(wnd.y())
                  .arg(wnd.width()).arg(wnd.height());

   pDb->setValue ("WndRect", sGeo);
}

/* -----------------------------------------------------------------\
|  Method: GetWindowRect
|  Begin: 27.01.2010 / 11:22:39
|  Author: Jo2003
|  Description: get windows position / size from ini file
|
|  Parameters: pointer to ok flag
|
|  Returns:  position, size of window
\----------------------------------------------------------------- */
QRect CSettingsDlg::GetWindowRect (bool *ok)
{
   QString sGeo = pDb->stringValue("WndRect");
   QRect   wnd;

   if (ok)
   {
      *ok = false;
   }

   if (sGeo.length() > 0)
   {
      QRegExp rx("^([0-9]*);([0-9]*);([0-9]*);([0-9]*).*$");

      if (rx.indexIn(sGeo) > -1)
      {
         wnd.setX(rx.cap(1).toInt());
         wnd.setY(rx.cap(2).toInt());
         wnd.setWidth(rx.cap(3).toInt());
         wnd.setHeight(rx.cap(4).toInt());

         if (ok)
         {
            *ok = true;
         }
      }
   }

   return wnd;
}

/* -----------------------------------------------------------------\
|  Method: SaveSplitterSizes
|  Begin: 18.02.2010 / 11:22:39
|  Author: Jo2003
|  Description: save chanlist size (splitter position)
|
|  Parameters: splitter size
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::SaveSplitterSizes (const QString &name, const QList<int> &sz)
{
   QString     sSz;
   QTextStream str(&sSz);

   for (int i = 0; i < sz.size(); i++)
   {
      str << sz[i] << ";";
   }

   pDb->setValue (name, sSz);
}

/* -----------------------------------------------------------------\
|  Method: SaveFavourites
|  Begin: 25.02.2010 / 14:22:39
|  Author: Jo2003
|  Description: save favourites
|
|  Parameters: list of favourites
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::SaveFavourites(const QList<int> &favList)
{
   QString     sFav;
   QTextStream str(&sFav);

   for (int i = 0; i < favList.size(); i++)
   {
      str << favList[i] << ";";
   }

   pDb->setValue ("Favorites", sFav);
}

/* -----------------------------------------------------------------\
|  Method: GetSplitterSizes
|  Begin: 18.02.2010 / 11:22:39
|  Author: Jo2003
|  Description: get chan list size
|
|  Parameters: pointer to ok flag
|
|  Returns:  size of chan list group box
\----------------------------------------------------------------- */
QList<int> CSettingsDlg::GetSplitterSizes(const QString &name, bool *ok)
{
   QString    sSz = pDb->stringValue(name);
   QList<int> sz;

   if (ok)
   {
      *ok = false;
   }

   if (sSz.length() > 0)
   {
      for (int i = 0; i < sSz.count(';'); i++)
      {
         sz << sSz.section(';', i, i).toInt();
      }

      if (ok)
      {
         if (sz.size() > 0)
         {
            *ok = true;
         }
      }
   }

   return sz;
}

/* -----------------------------------------------------------------\
|  Method: GetFavourites
|  Begin: 25.02.2010 / 14:22:39
|  Author: Jo2003
|  Description: get favourites
|
|  Parameters: pointer to ok flag
|
|  Returns:  list of favourites
\----------------------------------------------------------------- */
QList<int> CSettingsDlg::GetFavourites(bool *ok)
{
   QString    sFav = pDb->stringValue("Favorites");
   QList<int> lFav;

   if (ok)
   {
      *ok = false;
   }

   if (sFav.length() > 0)
   {
      for (int i = 0; i < sFav.count(';'); i++)
      {
         lFav << sFav.section(';', i, i).toInt();
      }

      if (ok)
      {
         if (lFav.size() > 0)
         {
            *ok = true;
         }
      }
   }

   return lFav;
}

/* -----------------------------------------------------------------\
|  Method: SetIsMaximized
|  Begin: 18.02.2010 / 11:22:39
|  Author: Jo2003
|  Description: store windows state (maximized or something else)
|
|  Parameters: maximized flag
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::SetIsMaximized(bool bMax)
{
   int iState = (bMax) ? 1 : 0;
   pDb->setValue("IsMaximized", iState);
}

/* -----------------------------------------------------------------\
|  Method: IsMaximized
|  Begin: 18.02.2010 / 11:22:39
|  Author: Jo2003
|  Description: get last windows state (maximized or something else)
|
|  Parameters: --
|
|  Returns:  true --> maximized
|           false --> not maximized
\----------------------------------------------------------------- */
bool CSettingsDlg::IsMaximized()
{
   return (pDb->intValue("IsMaximized")) ? true : false;
}

/* -----------------------------------------------------------------\
|  Method: SetCustFontSize
|  Begin: 18.02.2010 / 11:22:39
|  Author: Jo2003
|  Description: save customized font size
|
|  Parameters: font size change value
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::SetCustFontSize(int iSize)
{
   pDb->setValue("CustFontSz", iSize);
}

/* -----------------------------------------------------------------\
|  Method: GetCustFontSize
|  Begin: 18.02.2010 / 11:22:39
|  Author: Jo2003
|  Description: get customized font size
|
|  Parameters: --
|
|  Returns:  font size change value
\----------------------------------------------------------------- */
int CSettingsDlg::GetCustFontSize()
{
   return pDb->intValue("CustFontSz");
}

/* -----------------------------------------------------------------\
|  Method: GetCookie
|  Begin: 03.03.2010 / 12:22:39
|  Author: Jo2003
|  Description: get last used cookie
|
|  Parameters: --
|
|  Returns:  cookie string
\----------------------------------------------------------------- */
QString CSettingsDlg::GetCookie()
{
   return pDb->stringValue("LastCookie");
}

/* -----------------------------------------------------------------\
|  Method: SaveCookie
|  Begin: 03.03.2010 / 12:22:39
|  Author: Jo2003
|  Description: save cookie into ini file
|
|  Parameters: ref. cookie string
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::SaveCookie(const QString &str)
{
   pDb->setValue ("LastCookie", str);
}

/* -----------------------------------------------------------------\
|  Method: slotSplashStateChgd
|  Begin: 08.03.2010 / 13:22:39
|  Author: Jo2003
|  Description: store new splash state
|
|  Parameters: new splash state
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::slotSplashStateChgd(bool bChecked)
{
   pDb->setValue("NoSplash", bChecked ? 1 : 0);
}

//===================================================================
// return internal stored values ==>
//===================================================================
QString CSettingsDlg::GetVLCPath ()
{
   return m_ui->lineVLC->text();
}

QString CSettingsDlg::GetTargetDir ()
{
   return m_ui->lineDir->text();
}

QString CSettingsDlg::GetUser()
{
   return m_ui->lineUsr->text();
}

QString CSettingsDlg::GetPasswd()
{
   return m_ui->linePass->text();
}

QString CSettingsDlg::GetErosPasswd()
{
   return m_ui->lineErosPass->text();
}

QString CSettingsDlg::GetProxyHost ()
{
   return m_ui->lineProxyHost->text();
}

QString CSettingsDlg::GetProxyUser ()
{
   return m_ui->lineProxyUser->text();
}

QString CSettingsDlg::GetProxyPasswd ()
{
   return m_ui->lineProxyPassword->text();
}

QString CSettingsDlg::GetLanguage()
{
   return m_ui->cbxLanguage->currentText();
}

QString CSettingsDlg::GetPlayerModule()
{
   QString sPlayModule;

   if (m_ui->cbxPlayerMod->currentText() != "")
   {
      sPlayModule = QString("%1/%2").arg(pFolders->getModDir())
                    .arg(m_ui->cbxPlayerMod->currentText());
   }

   return sPlayModule;
}

bool CSettingsDlg::UseProxy ()
{
   return m_ui->useProxy->isChecked();
}

bool CSettingsDlg::AllowEros()
{
   return m_ui->checkAdult->isChecked();
}

bool CSettingsDlg::FixTime()
{
   return m_ui->checkFixTime->isChecked();
}

bool CSettingsDlg::DoRefresh()
{
   return m_ui->checkRefresh->isChecked();
}

bool CSettingsDlg::HideToSystray()
{
   return m_ui->checkHideToSystray->isChecked();
}

bool CSettingsDlg::AskForRecFile()
{
   return m_ui->checkAskForName->isChecked();
}

bool CSettingsDlg::TranslitRecFile()
{
   return m_ui->checkTranslit->isChecked();
}

bool CSettingsDlg::DetachPlayer()
{
   return m_ui->checkDetach->isChecked();
}

int CSettingsDlg::GetProxyPort ()
{
   return m_ui->lineProxyPort->text().toInt();
}

int CSettingsDlg::GetRefrInt()
{
   return m_ui->cbxInterval->currentText().toInt();;
}

vlclog::eLogLevel CSettingsDlg::GetLogLevel()
{
   return (vlclog::eLogLevel)m_ui->cbxLogLevel->currentIndex();
}

int CSettingsDlg::GetBufferTime()
{
   return m_ui->cbxBufferSeconds->itemData(m_ui->cbxBufferSeconds->currentIndex()).toInt();
}

QString CSettingsDlg::GetShutdownCmd()
{
   return m_ui->lineShutdown->text();
}

bool CSettingsDlg::DisableSplashScreen()
{
   return (pDb->intValue("NoSplash")) ? true : false;
}

int  CSettingsDlg::GetBitRate()
{
   return m_ui->cbxBitRate->currentText().toInt();
}

QString CSettingsDlg::GetAPIServer()
{
   return m_ui->lineApiServer->text();
}

bool CSettingsDlg::extChanList()
{
   return m_ui->checkExtChanInfo->isChecked();
}

bool CSettingsDlg::checkForUpdate()
{
   return m_ui->checkUpdate->isChecked();
}

int CSettingsDlg::getTimeShift()
{
   return m_ui->cbxTimeShift->itemData(m_ui->cbxTimeShift->currentIndex()).toInt();
}

void CSettingsDlg::saveChannel(int cid)
{
   pDb->setValue("lastChan", cid);
}

int CSettingsDlg::lastChannel()
{
   return pDb->intValue("lastChan");
}

void CSettingsDlg::saveEpgDay(const QString &dateString)
{
   pDb->setValue("epgDay", dateString);
}

QString CSettingsDlg::lastEpgDay()
{
   return pDb->stringValue("epgDay");
}

bool CSettingsDlg::doubleClickToPlay()
{
   return m_ui->check2ClicksToPlay->isChecked();
}

bool CSettingsDlg::useGpuAcc()
{
   return m_ui->checkGPUAcc->isChecked();
}

bool CSettingsDlg::showAds()
{
   return m_ui->checkAds->isChecked();
}

uint CSettingsDlg::libVlcVerboseLevel()
{
   return m_ui->lineVlcVerbose->text().toUInt();
}

//===================================================================
// <== return internal stored values
//===================================================================

/* -----------------------------------------------------------------\
|  Method: addShortCut
|  Begin: 15.03.2011 / 15:45
|  Author: Jo2003
|  Description: add a shortcut to shortcut table
|
|  Parameters: description, target, slot, shortcut
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::addShortCut(const QString &descr, const QString &target,
                              const QString &slot, const QString &keys)
{
   m_ui->tableShortCuts->addShortCut(descr, target, slot, keys);
}

/* -----------------------------------------------------------------\
|  Method: shortCut
|  Begin: 15.03.2011 / 15:45
|  Author: Jo2003
|  Description: get matching shortcut
|
|  Parameters: target and slot string
|
|  Returns:  shortcut string
\----------------------------------------------------------------- */
QString CSettingsDlg::shortCut(const QString &target, const QString &slot) const
{
   return m_ui->tableShortCuts->shortCut(target, slot);
}

/* -----------------------------------------------------------------\
|  Method: delShortCut
|  Begin: 18.03.2011 / 11:20
|  Author: Jo2003
|  Description: delete matching shortcut from shortcut table only
|
|  Parameters: target and slot string
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::delShortCut(const QString &target, const QString &slot)
{
   m_ui->tableShortCuts->delShortCut(target, slot);
}

/* -----------------------------------------------------------------\
|  Method: updateShortcutDescr
|  Begin: 25.03.2011 / 11:45
|  Author: Jo2003
|  Description: update description text
|
|  Parameters: description, target and slot
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::updateShortcutDescr (const QString &descr, const QString &target, const QString &slot)
{
   m_ui->tableShortCuts->updateShortcutDescr(descr, target, slot);
}

/* -----------------------------------------------------------------\
|  Method: on_btnResetShortcuts_clicked
|  Begin: 30.03.2011 / 16:45
|  Author: Jo2003
|  Description: reset shortcuts
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::on_btnResetShortcuts_clicked()
{
   m_ui->tableShortCuts->revert();
}

/* -----------------------------------------------------------------\
|  Method: shortCutCount
|  Begin: 30.03.2011 / 16:15
|  Author: Jo2003
|  Description: return number of shortcuts in shortcut table
|
|  Parameters: --
|
|  Returns:  number of shortcuts
\----------------------------------------------------------------- */
int CSettingsDlg::shortCutCount()
{
   return m_ui->tableShortCuts->rowCount();
}

/************************* History ***************************\
| $Log$
\*************************************************************/
