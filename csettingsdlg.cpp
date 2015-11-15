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
#include <QRadioButton>
#include <QMessageBox>
#include "externals_inc.h"


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
    m_ui(new Ui::CSettingsDlg), bSettingsRead(false)
{
   m_ui->setupUi(this);
   pAccountInfo    = NULL;
   pShortApiServer = new CShortcutEx(QKeySequence("CTRL+ALT+A"), this);
   pShortVerbLevel = new CShortcutEx(QKeySequence("CTRL+ALT+V"), this);
   m_pStrStdDlg    = new QStrStandardDlg(this, Qt::Tool);
   m_pSpdTestDlg   = new QSpeedTestDlg(this, Qt::Tool);
   m_ulChgdMask    = 0;

   if (pShortApiServer)
   {
      connect(pShortApiServer, SIGNAL(activated()), this, SLOT(slotEnableApiServer()));
   }

   if (pShortVerbLevel)
   {
      connect(pShortVerbLevel, SIGNAL(activated()), this, SLOT(slotEnableVlcVerbLine()));
   }

   m_iServerBitrate   = -1;
   m_iServerTimeShift = -1;

   // set company name for login data ...
   QString s = m_ui->groupAccount->title();
   m_ui->groupAccount->setTitle(s.arg(pCustomization->strVal("COMPANY_NAME")));

   // fill in buffer values ...
   vBuffs << 1.5 << 3 << 5 << 8 << 15 << 20 << 30 << 45 << 60 << 90;
   qSort(vBuffs);

#ifdef ENABLE_AD_SWITCH
   m_ui->checkAds->setEnabled(true);
#else
   m_ui->checkAds->hide();
#endif // ENABLE_AD_SWITCH

#ifndef _HAS_VOD_MANAGER
   /////////////////////////////////////////////////////////////////////////////
   //               Disable items if no VOD manager is supported              //
   /////////////////////////////////////////////////////////////////////////////
   m_ui->groupChanMan->hide();
   m_ui->groupVodMan->hide();
   m_ui->vlSetPage->setStretch(0, 0);  // hidden vod and channel manager
   m_ui->vlSetPage->setStretch(1, 10); // parent code and erotik settings
   m_ui->vlSetPage->setStretch(2, 5);  // vertical spacer
   m_ui->vlSetPage->setStretch(3, 0);  // save button

   // since parental manager is very reduced, there is no need
   // to secure it using parental code ...
   m_ui->stackedWidget->setCurrentIndex(1);
   m_ui->btnSaveExitManager->setDisabled(true);
   m_ui->btnSaveExitManager->hide();
#endif // _HAS_VOD_MANAGER

#ifdef _IS_OEM
   if (m_ui->lineApiServer->isVisible())
   {
      m_ui->lineApiServer->setVisible(false);
   }
#endif

#ifndef _EXT_EPG
   m_ui->checkExtEPG->setVisible(false);
#endif // _EXT_EPG

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
   delete m_ui;
}

/* -----------------------------------------------------------------\
|  Method: setAccountInfo
|  Begin: 06.06.2012
|  Author: Jo2003
|  Description: set account info struct
|
|  Parameters: pointer to account info struct
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::setAccountInfo(const cparser::SAccountInfo *pInfo)
{
   pAccountInfo = pInfo;
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
   bool        bUpdCase = false;

   ////////////////////////////////////////////////////////////////////////////////
   // update case: Encrypted passwords ---->
   // We added a simply password encryption. In case there is an update,
   // we must make sure that the unencrypted password is converted
   // into an encrypted one. To make our lives easier we'll use new
   // keys for the encrypted passwords ...
   if (((s = pDb->stringValue("Passwd")) != "")
       && (pDb->stringValue("PasswdEnc") == ""))
   {
      bUpdCase = true;
      pDb->setPassword("PasswdEnc", s);
   }

   if (((s = pDb->stringValue("ErosPasswd")) != "")
       && (pDb->stringValue("ErosPasswdEnc") == ""))
   {
      pDb->setPassword("ErosPasswdEnc", s);
   }

   if (((s = pDb->stringValue("ProxyPasswd")) != "")
       && (pDb->stringValue("ProxyPasswdEnc") == ""))
   {
      pDb->setPassword("ProxyPasswdEnc", s);
   }

   // remove unused settings ...
   if (bUpdCase)
   {
      pDb->removeSetting("Passwd");
      pDb->removeSetting("ErosPasswd");
      pDb->removeSetting("ProxyPasswd");
   }
   // update case: Encrypted passwords <----
   ////////////////////////////////////////////////////////////////////////////////


   // line edits ...
   m_ui->lineVLC->setText (pDb->stringValue("VLCPath"));
   m_ui->lineDir->setText (pDb->stringValue("TargetDir"));
   m_ui->lineUsr->setText (pDb->stringValue("User"));
   m_ui->linePass->setText (pDb->password("PasswdEnc"));
   m_ui->lineErosPass->setText(pDb->password("ErosPasswdEnc"));
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
      m_ui->lineApiServer->setText (pCustomization->strVal("API_SERVER"));
   }

   // in case we use customization we should hide the API server name stuff ...
#ifdef _IS_OEM
   m_ui->lineApiServer->setVisible(false);
#endif // _IS_OEM

   m_ui->lineProxyHost->setText(pDb->stringValue("ProxyHost"));
   m_ui->lineProxyPort->setText(pDb->stringValue("ProxyPort"));
   m_ui->lineProxyUser->setText(pDb->stringValue("ProxyUser"));
   m_ui->lineProxyPassword->setText(pDb->password("ProxyPasswdEnc"));

   // check boxes ...
   m_ui->useProxy->setCheckState((Qt::CheckState)pDb->intValue("UseProxy"));
   m_ui->checkAdult->setCheckState((Qt::CheckState)pDb->intValue("AllowAdult"));
   m_ui->checkHideToSystray->setCheckState((Qt::CheckState)pDb->intValue("TrayHide"));
   m_ui->checkAskForName->setCheckState((Qt::CheckState)pDb->intValue("AskRecFile"));
   m_ui->checkTranslit->setCheckState((Qt::CheckState)pDb->intValue("TranslitRecFile"));
   m_ui->checkDetach->setCheckState((Qt::CheckState)pDb->intValue("DetachPlayer"));
   m_ui->checkAdvanced->setCheckState((Qt::CheckState)pDb->intValue("AdvSet"));
   m_ui->checkGPUAcc->setCheckState((Qt::CheckState)pDb->intValue("GPUAcc"));
   m_ui->chkStrSrvAuto->setCheckState((Qt::CheckState)pDb->intValue("AutoStrSrv", &iErr));

   // value doesn't exist in database ...
   if (iErr)
   {
      // enable by default ...
      m_ui->chkStrSrvAuto->setCheckState(Qt::Checked);
   }

   m_ui->checkAds->setCheckState((Qt::CheckState)pDb->intValue("AdsEnabled", &iErr));

   // value doesn't exist in database ...
   if (iErr)
   {
      // enable by default ...
      m_ui->checkAds->setCheckState(Qt::Checked);
   }

#ifdef _EXT_EPG
   m_ui->checkExtEPG->setCheckState((Qt::CheckState)pDb->intValue("ExtEPG", &iErr));

   // value doesn't exist in database ...
   if (iErr)
   {
      // enable by default ...
      m_ui->checkExtEPG->setCheckState(Qt::Checked);
   }
#endif // _EXT_EPG

   m_ui->check2ClicksToPlay->setCheckState((Qt::CheckState)pDb->intValue("2ClickPlay", &iErr));

   // value doesn't exist in database ...
   if (iErr)
   {
      // enable by default ...
      m_ui->check2ClicksToPlay->setCheckState(Qt::Checked);
   }

   m_ui->tabWidget->setTabEnabled(2, pDb->intValue("AdvSet") ? true : false);

   if (m_ui->checkAdvanced->isChecked())
   {
      vBuffs.prepend(0.5);
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

   m_ui->cbxLogLevel->setCurrentIndex((int)pDb->intValue("LogLevel"));

   // deinterlace mode ...
   if ((s = pDb->stringValue("DeintlMode")) != "")
   {
      if ((iIdx = m_ui->cbxDeintlMode->findText(s)) != -1)
      {
         m_ui->cbxDeintlMode->setCurrentIndex(iIdx);
      }
   }

   if ((s = pDb->stringValue("PlayerModule")) == "")
   {
      // no player module chosen yet ...

      // default module in version 2.xx is 5_libvlc.mod
      s = "5_libvlc.mod";
   }

   iIdx = m_ui->cbxPlayerMod->findText(s);
   m_ui->cbxPlayerMod->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

#ifndef Q_OS_MAC
   // check if hide to systray is supported ...
   // always disable on mac!
   if (!QSystemTrayIcon::isSystemTrayAvailable())
#endif
   {
      m_ui->checkHideToSystray->setDisabled(true);
   }

   // font size ...
   m_ui->spinBoxFontDelta->setValue(pDb->intValue("CustFontSz"));

   // mark settings as read ...
   bSettingsRead = true;
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
            // take care for combo boxes filled from
            // out side ..
            int         i;
            QVariantMap::ConstIterator cit;
            QVariantMap vmRates;
            QVariantMap vmServers;
            QVariant    vCurSrv;
            QVariant    vCurBr;

            for (i = 0; i < m_ui->cbxBitRate->count(); i++)
            {
                vmRates.insert(m_ui->cbxBitRate->itemText(i), m_ui->cbxBitRate->itemData(i));

                if (i == m_ui->cbxBitRate->currentIndex())
                {
                    vCurBr = m_ui->cbxBitRate->itemData(i);
                }
            }

            for (i = 0; i < m_ui->cbxStreamServer->count(); i++)
            {
                vmServers.insert(m_ui->cbxStreamServer->itemText(i), m_ui->cbxStreamServer->itemData(i));

                if (i == m_ui->cbxStreamServer->currentIndex())
                {
                    vCurSrv = m_ui->cbxStreamServer->itemData(i);
                }
            }

            // save current index from comboboxes ...
            int iLanIdx = m_ui->cbxLanguage->currentIndex();
            int iLogIdx = m_ui->cbxLogLevel->currentIndex();
            int iBufIdx = m_ui->cbxBufferSeconds->currentIndex();
            int iModIdx = m_ui->cbxPlayerMod->currentIndex();
            int iDeiIdx = m_ui->cbxDeintlMode->currentIndex();

            m_ui->retranslateUi(this);

            // clear content of stream server and bitrates ...
            m_ui->cbxBitRate->clear();
            m_ui->cbxStreamServer->clear();

            // re-fill bitrates and stream servers ...
            for (cit = vmRates.constBegin(); cit != vmRates.constEnd(); cit ++)
            {
                m_ui->cbxBitRate->addItem(cit.key(), cit.value());
            }

            for (cit = vmServers.constBegin(); cit != vmServers.constEnd(); cit ++)
            {
                m_ui->cbxStreamServer->addItem(cit.key(), cit.value());
            }

            // re-set index to comboboxes ...
            m_ui->cbxLanguage->setCurrentIndex(iLanIdx);
            m_ui->cbxLogLevel->setCurrentIndex(iLogIdx);
            m_ui->cbxBufferSeconds->setCurrentIndex(iBufIdx);
            m_ui->cbxPlayerMod->setCurrentIndex(iModIdx);
            m_ui->cbxDeintlMode->setCurrentIndex(iDeiIdx);

            m_ui->cbxBitRate->setCurrentIndex(m_ui->cbxBitRate->findData(vCurBr));
            m_ui->cbxStreamServer->setCurrentIndex(m_ui->cbxStreamServer->findData(vCurSrv));

            // set company name ...
            QString s = m_ui->groupAccount->title();
            m_ui->groupAccount->setTitle(s.arg(pCustomization->strVal("COMPANY_NAME")));

            // stream standard
            handleStrStdDeps();
        }
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------
//
//! \brief   set some data triggered by show event
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \param   e [in] (QShowEvent*) event pointer
//---------------------------------------------------------------------------
void CSettingsDlg::showEvent(QShowEvent *e)
{
    handleStrStdDeps();

    QDialog::showEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   check if chosen bitrate and timeshift makes sende
//
//! \author  Jo2003
//! \date    14.09.2014
//
//! \param   iBitRate [in] (int) chosen bitrate
//! \param   iTimeShift [in] (int) chosen timeshift
//! \param   what [in] (const QString &) what to check
//
//! \return  true -> emit changed signal; false -> don't emit
//---------------------------------------------------------------------------
bool CSettingsDlg::checkBitrateAndTimeShift(int iBitRate, int iTimeShift, const QString &what)
{
   bool bSendSig = true;

   if ((showInfo.playState() == IncPlay::PS_PLAY)
       && (showInfo.showType() == ShowInfo::Archive))
   {
      // get informaion about the channel from showinfo
      int     cid = showInfo.channelId();
      bool    bArchHasTs = false;
      QString msg;
      QMap<int, QString> brMap;

      getBrMap(brMap);

      if (pChanMap->contains(cid))
      {
         cparser::SChan chan = pChanMap->value(cid);

         for (int i = 0; ((i < chan.vTs.count()) && !bArchHasTs); i ++)
         {
            if ((chan.vTs.at(i).iTimeShift == iTimeShift)
                && (chan.vTs.at(i).iBitRate == iBitRate))
            {
               bArchHasTs = true;
            }
         }

         if (!bArchHasTs)
         {
            QMessageBox::StandardButton btn;
            msg  = tr("The archive for channel '%1' isn't available in your combination of bitrate (%2) and timeshift (%3)!")
                  .arg(showInfo.chanName()).arg(brMap[iBitRate]).arg(iTimeShift);
            msg += "<br>";
            msg += tr("Following table shows the available combinations:");
            msg += pHtml->createBitrateTsTable(chan.vTs);
            msg += "<br> <br>";
            msg += pHtml->htmlTag("b", tr("Do you want to activate the new %1? The player will switch to 'Live' then.").arg(what));

            btn = QMessageBox::information(this, tr("Information"), msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            if (btn == QMessageBox::No)
            {
               bSendSig = false;
            }
            else if (btn == QMessageBox::Yes)
            {
               // emulate Live show ...
               showInfo.setShowType(ShowInfo::Live);
            }
         }
      }
   }

   return bSendSig;
}

//---------------------------------------------------------------------------
//
//! \brief   enable / disable bitrate and buffer settings dependend on
//!          stream standard
//
//! \author  Jo2003
//! \date    04.11.2015
//
//---------------------------------------------------------------------------
void CSettingsDlg::handleStrStdDeps()
{
    // update button text
    m_ui->pushStrStd->setText(m_pStrStdDlg->getCurrName());

    // enable / disable different dialog items dependend on new stream standard...
    QString ssVal = m_pStrStdDlg->getCurrVal();

    // make fuzzy compare since in stream standard values might be some typos ...
    if (ssVal.contains("dash", Qt::CaseInsensitive) || ssVal.contains("hls", Qt::CaseInsensitive))
    {
        m_ui->cbxBitRate->setEnabled(false);
        m_ui->cbxBufferSeconds->setEnabled(false);
    }
    else
    {
        m_ui->cbxBitRate->setEnabled(true);
        m_ui->cbxBufferSeconds->setEnabled(true);
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
   checkChanges();

   // line edits ...
   pDb->setValue("VLCPath", m_ui->lineVLC->text());
   pDb->setValue("User", m_ui->lineUsr->text());
   pDb->setValue("TargetDir", m_ui->lineDir->text());
   pDb->setPassword("PasswdEnc", m_ui->linePass->text());

   pDb->setValue("ProxyHost", m_ui->lineProxyHost->text());
   pDb->setValue("ProxyPort", m_ui->lineProxyPort->text());
   pDb->setValue("ProxyUser", m_ui->lineProxyUser->text());
   pDb->setPassword("ProxyPasswdEnc", m_ui->lineProxyPassword->text());
   pDb->setValue("ShutdwnCmd", m_ui->lineShutdown->text());
   pDb->setValue("APIServer", m_ui->lineApiServer->text());
   pDb->setValue("libVlcLogLevel", m_ui->lineVlcVerbose->text());

   // check boxes ...
   pDb->setValue("UseProxy", (int)m_ui->useProxy->checkState());
   pDb->setValue("TrayHide", (int)m_ui->checkHideToSystray->checkState());
   pDb->setValue("AskRecFile", (int)m_ui->checkAskForName->checkState());
   pDb->setValue("TranslitRecFile", (int)m_ui->checkTranslit->checkState());
   pDb->setValue("DetachPlayer", (int)m_ui->checkDetach->checkState());
   pDb->setValue("AdvSet", (int)m_ui->checkAdvanced->checkState());
   pDb->setValue("2ClickPlay", (int)m_ui->check2ClicksToPlay->checkState());
   pDb->setValue("GPUAcc", (int)m_ui->checkGPUAcc->checkState());
   pDb->setValue("AdsEnabled", (int)m_ui->checkAds->checkState());
   pDb->setValue("ExtEPG", (int)m_ui->checkExtEPG->checkState());
   pDb->setValue("AutoStrSrv", (int)m_ui->chkStrSrvAuto->checkState());

#ifndef _HAS_VOD_MANAGER
   /////////////////////////////////////////////////////////////////////////////
   //                      no VOD manager is supported                        //
   /////////////////////////////////////////////////////////////////////////////
   // These values are normally stored when leaving parental manager.
   // Since this button is disabled here we save it using the normal
   // save button.
   pDb->setValue("AllowAdult", (int)m_ui->checkAdult->checkState());
   pDb->setPassword("ErosPasswdEnc", m_ui->lineErosPass->text());
#endif // _HAS_VOD_MANAGER

   // combo boxes ...
   pDb->setValue("Language", m_ui->cbxLanguage->currentText());
   int idx = m_ui->cbxBufferSeconds->currentIndex();
   pDb->setValue("HttpCache", m_ui->cbxBufferSeconds->itemData((idx == -1) ? 0 : idx).toInt());
   pDb->setValue("LogLevel", m_ui->cbxLogLevel->currentIndex());
   pDb->setValue("PlayerModule", m_ui->cbxPlayerMod->currentText());
   pDb->setValue("DeintlMode", m_ui->cbxDeintlMode->currentText());

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

   if (!vSrvList.isEmpty())
   {
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

   // make sure the box isn't touched if there is only one entry ...
   if ((vSrvList.count() < 2) || m_ui->chkStrSrvAuto->isChecked())
   {
      m_ui->cbxStreamServer->setDisabled(true);
   }
   else
   {
      m_ui->cbxStreamServer->setEnabled(true);
   }
}

/* -----------------------------------------------------------------\
|  Method: SetBitrateCbx
|  Begin: 14.01.2011 / 14:15
|  Author: Jo2003
|  Description: fill / mark combobox for bitrate
|
|  Parameters: ref. to bitrate map, act bitrate
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::SetBitrateCbx (const cparser::QBitratesMap &mBitRates, int iActrate)
{
   int iActIdx = 0;
   int iCount  = 0;
   cparser::QBitratesMap::const_iterator cit;

   // backup bitrate ...
   m_iServerBitrate = iActrate;

   if (!mBitRates.isEmpty())
   {
      m_ui->cbxBitRate->clear();

      // add all available bitrates ...
      for (cit = mBitRates.constBegin(); cit != mBitRates.constEnd(); cit++)
      {
          m_ui->cbxBitRate->addItem(cit.value(), cit.key());

          if (cit.key() == iActrate)
          {
              iActIdx = iCount;
          }

          iCount ++;
      }

      // mark active rate ...
      m_ui->cbxBitRate->setCurrentIndex(iActIdx);
   }

   // make sure the box isn't touched if there is only one entry ...
   if (mBitRates.count() < 2)
   {
      m_ui->cbxBitRate->setDisabled(true);
   }
   else
   {
      m_ui->cbxBitRate->setEnabled(true);
   }
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

   m_iServerTimeShift = iAct;

   if (!vVals.isEmpty())
   {
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

   // make sure the box isn't touched if there is only one entry ...
   if (vVals.count() < 2)
   {
      m_ui->cbxTimeShift->setDisabled(true);
   }
   else
   {
      m_ui->cbxTimeShift->setEnabled(true);
   }
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
   int  br       = m_ui->cbxBitRate->itemData(index).toInt();
   int  i;
   bool bSendSig = true;

   if ((showInfo.playState() == IncPlay::PS_PLAY)
       && (showInfo.showType() == ShowInfo::Archive))
   {
      // get informaion about the channel from showinfo
      int     cid = showInfo.channelId();
      bool    bArchHasBitrate = false;
      QString msg;
      QVector<int> brAvail;
      QMap<int, QString> brMap;

      // create map from values in cbx ...
      getBrMap(brMap);

      if (pChanMap->contains(cid))
      {
         cparser::SChan chan = pChanMap->value(cid);

         for (i = 0; i < chan.vTs.count(); i ++)
         {
            if (chan.vTs.at(i).iTimeShift  == 100) // API hack!
            {
               brAvail.append(chan.vTs.at(i).iBitRate);

               if (chan.vTs.at(i).iBitRate == br)
               {
                  bArchHasBitrate = true;
               }
            }
         }

         if (!bArchHasBitrate)
         {
            QMessageBox::StandardButton btn;
            msg  = tr("The archive for channel '%1' isn't available in the bitrate '%2'!")
                  .arg(showInfo.chanName()).arg(brMap[br]);
            msg += "<br> <br>";
            msg += tr("Following bitrates are available:");
            msg += "<br>";

            for (i = 0; i < brAvail.count(); i++)
            {
               msg += QString("%1, ").arg(brMap.value(brAvail.at(i)));
            }

            msg += "<br> <br>";
            msg += pHtml->htmlTag("b", tr("Do you want to activate the new bitrate? The player will switch to 'Live' then."));

            btn = QMessageBox::information(this, tr("Information"), msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            if (btn == QMessageBox::No)
            {
               bSendSig = false;
            }
            else if (btn == QMessageBox::Yes)
            {
               // emulate Live show ...
               showInfo.setShowType(ShowInfo::Live);
            }
         }
      }
   }

   if (!bSendSig)
   {
      // return to the default bitrate ...
      if ((index = m_ui->cbxBitRate->findData(m_iServerBitrate)) != -1)
      {
         m_ui->cbxBitRate->setCurrentIndex(index);
      }
   }
   else
   {
      m_iServerBitrate = br;
      emit sigSetBitRate(br);
   }
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
   int  ts    = m_ui->cbxTimeShift->itemData(index).toInt();
   /*
    * Maybe later we will add this check here as well ...
    *
   int  br    = m_ui->cbxBitRate->itemData(m_ui->cbxBitRate->currentIndex()).toInt();
   bool bSend = checkBitrateAndTimeShift(br, ts, tr("timeshift"));

   if (!bSend)
   {
      // return to the default timeshift ...
      if ((index = m_ui->cbxTimeShift->findData(m_iServerTimeShift)) != -1)
      {
         m_ui->cbxTimeShift->setCurrentIndex(index);
      }
   }
   else
   {
      m_iServerTimeShift = ts;

      // store global ...
      tmSync.setTimeShift(ts);
      emit sigSetTimeShift(ts);
   }
   */

   m_iServerTimeShift = ts;

   // store global ...
   tmSync.setTimeShift(ts);
   emit sigSetTimeShift(ts);
}

/* -----------------------------------------------------------------\
|  Method: on_cbxLanguage_currentIndexChanged
|  Begin: 24.07.2013
|  Author: Jo2003
|  Description: set and activate language
|
|  Parameters: new language shortcut
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_cbxLanguage_currentIndexChanged(const QString &lng)
{
   // set language as read ...
   pAppTransl->load(QString("lang_%1").arg(lng), pFolders->getLangDir());
   pQtTransl->load(QString("qt_%1").arg(lng), pFolders->getQtLangDir());
}

//---------------------------------------------------------------------------
//
//! \brief   change / save language change
//
//! \author  Jo2003
//! \date    26.07.2013
//
//! \param   lng (const QString&) language code to set
//
//! \return  --
//---------------------------------------------------------------------------
void CSettingsDlg::on_cbxLanguage_activated(const QString &lng)
{
#ifdef _TASTE_IPTV_RECORD
   // save language as read ...
   pApiClient->queueRequest(CIptvDefs::REQ_SET_LANGUAGE, lng);
#else
   Q_UNUSED(lng)
#endif
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
   QStringList sl;

   for (int i = 0; i < sz.size(); i++)
   {
      sl << QString::number(sz[i]);
   }

   pDb->setValue (name, sl.join(";"));
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
   int         err;
   QString     s   = pDb->stringValue(name, &err);
   QList<int>  sz;

   if (ok)
   {
      *ok = false;
   }

   if ((s.size() > 0) && !err)
   {
      QStringList sl  = s.split(";");

      for (int i = 0; i < sl.count(); i++)
      {
         sz << sl.at(i).toInt();
      }

      if (ok)
      {
         *ok = true;
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
|  Method: setGeometry
|  Begin: 11.07.2012
|  Author: Jo2003
|  Description: save a geometry
|
|  Parameters: ref. byte array
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::setGeometry(const QByteArray &ba)
{
   pDb->setBlob("WndGeometry", ba);
}

/* -----------------------------------------------------------------\
|  Method: getGeometry
|  Begin: 11.07.2012
|  Author: Jo2003
|  Description: get stored geometry
|
|  Parameters: --
|
|  Returns:  byte array with geometry data
\----------------------------------------------------------------- */
QByteArray CSettingsDlg::getGeometry()
{
   return pDb->blobValue("WndGeometry");
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

QString CSettingsDlg::getDeinlMode()
{
   return m_ui->cbxDeintlMode->currentText();
}

QString CSettingsDlg::getStreamServer()
{
   int idx = m_ui->cbxStreamServer->currentIndex();
   return m_ui->cbxStreamServer->itemData((idx == -1) ? 0 : idx).toString();
}

bool CSettingsDlg::UseProxy ()
{
   return m_ui->useProxy->isChecked();
}

bool CSettingsDlg::AllowEros()
{
   return m_ui->checkAdult->isChecked();
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

vlclog::eLogLevel CSettingsDlg::GetLogLevel()
{
   return (vlclog::eLogLevel)m_ui->cbxLogLevel->currentIndex();
}

int CSettingsDlg::GetBufferTime()
{
   int idx = m_ui->cbxBufferSeconds->currentIndex();
   return m_ui->cbxBufferSeconds->itemData((idx == -1) ? 0 : idx).toInt();
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
   int idx = m_ui->cbxBitRate->currentIndex();
   return m_ui->cbxBitRate->itemData((idx == -1) ? 0 : idx).toInt();
}

QString CSettingsDlg::GetAPIServer()
{
   return m_ui->lineApiServer->text();
}

int CSettingsDlg::getTimeShift()
{
   int idx = m_ui->cbxTimeShift->currentIndex();
   return m_ui->cbxTimeShift->itemData((idx == -1) ? 0 : idx).toInt();
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

bool CSettingsDlg::extEpg()
{
    return m_ui->checkExtEPG->isChecked();
}

bool CSettingsDlg::autoStrSrv()
{
    return m_ui->chkStrSrvAuto->isChecked();
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

/* -----------------------------------------------------------------\
|  Method: slotLockParentalManager [slot]
|  Begin: 14.05.2012
|  Author: Jo2003
|  Description: lock parental manager
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::slotLockParentalManager()
{
#ifdef _HAS_VOD_MANAGER
   sTempPasswd = "";
   m_ui->stackedWidget->setCurrentIndex(0);
   m_ui->tabWidget->setTabIcon(3, QIcon(":/access/locked"));
#endif // _HAS_VOD_MANAGER
}

/* -----------------------------------------------------------------\
|  Method: slotBuildChanManager [slot]
|  Begin: 14.05.2012
|  Author: Jo2003
|  Description: fill channel manager with data
|
|  Parameters: XML data
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::slotBuildChanManager(const QString &str)
{
   QString          sLogo;
   QListWidgetItem *pItem;
   QFileInfo        fInfo;

   channelVector.clear();

   if (!pApiParser->parseChannelList(str, channelVector))
   {
      m_ui->listHide->clear();

      for (int i = 0; i < channelVector.count(); i++)
      {
         if (!channelVector[i].bIsGroup)
         {
            fInfo.setFile(channelVector[i].sIcon);
            sLogo = QString("%1/%2").arg(pFolders->getLogoDir()).arg(fInfo.fileName());
            pItem = new QListWidgetItem (QIcon(sLogo), channelVector[i].sName);

            // save channel id in list item ...
            pItem->setData(Qt::UserRole, channelVector[i].iId);

            m_ui->listHide->addItem(pItem);

            if (channelVector[i].bIsHidden)
            {
               pItem->setSelected(true);
            }
         }
      }

      if (pAccountInfo->bHasVOD)
      {
         // request vod manager data ...
         pApiClient->queueRequest(CIptvDefs::REQ_GET_VOD_MANAGER, sTempPasswd);
      }
      else
      {
         // show manager widget ...
         m_ui->stackedWidget->setCurrentIndex(1);
         m_ui->tabWidget->setTabIcon(m_ui->tabWidget->currentIndex(), QIcon(":/access/unlocked"));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotBuildVodManager [slot]
|  Begin: 23.05.2012
|  Author: Jo2003
|  Description: fill VOD manager with data
|
|  Parameters: XML data
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::slotBuildVodManager(const QString &str)
{
   vodRatesVector.clear();

   QLayout      *pLayout = m_ui->scrollAreaContents->layout();
   QVBoxLayout  *pVMainLayout, *pVChildLayout;
   QLabel       *pTitle;
   QFrame       *pChildWidget;
   QRadioButton *pRadHide, *pRadShow, *pRadPass;
   QString       sLabel;
   QFont         font;
   QMap<QString, QString> transMap;

   // help translate the vod manager strings ...
   transMap.insert("blood",    tr("blood"));
   transMap.insert("violence", tr("violence"));
   transMap.insert("obscene",  tr("obscene"));
   transMap.insert("obsence",  tr("obscene")); // fix a small typo in API ...
   transMap.insert("porn",     tr("porn"));
   transMap.insert("horror",   tr("horror"));

   // clear layout ...
   if (pLayout)
   {
      QLayoutItem *child;
      while ((child = pLayout->takeAt(0)) != 0)
      {
         child->widget()->deleteLater();
         delete child;
      }
      delete pLayout;
   }

   if (!pApiParser->parseVodManager(str, vodRatesVector))
   {
      pVMainLayout = new QVBoxLayout();

      // make forms for every rate ...
      for (int i = 0; i < vodRatesVector.count(); i++)
      {
         // translate label ...
         sLabel    = transMap.value(vodRatesVector[i].sGenre, vodRatesVector[i].sGenre);

         // make label better looking ...
         sLabel[0] = sLabel[0].toUpper();
         sLabel   += ":";

         // create the whole bunch new widgets needed in this form ...
         pChildWidget  = new QFrame();
         pVChildLayout = new QVBoxLayout();
         pTitle        = new QLabel(sLabel);
         pRadHide      = new QRadioButton (tr("hide"));
         pRadShow      = new QRadioButton (tr("show"));
         pRadPass      = new QRadioButton (tr("password protected"));

         // store the name of the form so we can check it later ...
         pChildWidget->setObjectName(vodRatesVector[i].sGenre);

         // make label bold ...
         font = pTitle->font();
         font.setBold(true);
         pTitle->setFont(font);

         // set spacing ...
         pVChildLayout->setSpacing(2);

         // add all widgets to layout ...
         pVChildLayout->addWidget(pTitle);
         pVChildLayout->addWidget(pRadShow);
         pVChildLayout->addWidget(pRadHide);
         pVChildLayout->addWidget(pRadPass);

         // set layout to form ...
         pChildWidget->setLayout(pVChildLayout);

         // mark radio button as needed ...
         if (vodRatesVector[i].sAccess == "hide")
         {
            pRadHide->setChecked(true);
         }
         else if (vodRatesVector[i].sAccess == "show")
         {
            pRadShow->setChecked(true);
         }
         else if (vodRatesVector[i].sAccess == "pass")
         {
            pRadPass->setChecked(true);
         }

         // add form to main layout ...
         pVMainLayout->addWidget(pChildWidget);
      }

      // show what we've done ...
      m_ui->scrollAreaContents->setLayout(pVMainLayout);
      m_ui->stackedWidget->setCurrentIndex(1);
      m_ui->tabWidget->setTabIcon(m_ui->tabWidget->currentIndex(), QIcon(":/access/unlocked"));
   }
}

/* -----------------------------------------------------------------\
|  Method: on_btnSaveExitManager_clicked [slot]
|  Begin: 23.05.2012
|  Author: Jo2003
|  Description: save all changes, request reload where needed
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::on_btnSaveExitManager_clicked()
{
   //////////////////////////////////////////////////
   // Adult Channels (Live) ...
   //////////////////////////////////////////////////
   pDb->setValue("AllowAdult", (int)m_ui->checkAdult->checkState());
   pDb->setPassword("ErosPasswdEnc", m_ui->lineErosPass->text());


   //////////////////////////////////////////////////
   // Channel Manager ...
   //////////////////////////////////////////////////

   // find differences ...
   QVector<int>     toShow, toHide, nowShow;
   QListWidgetItem *pItem;
   QStringList      ids;
   int              i, cid;

   // build show vector with actual values ...
   for (i = 0; i < channelVector.count(); i++)
   {
      if (!channelVector[i].bIsGroup)
      {
         if (!channelVector[i].bIsHidden)
         {
            nowShow.append(channelVector[i].iId);
         }
      }
   }

   // create hide and show vector with changed values only ...
   for (i = 0; i < m_ui->listHide->count(); i++)
   {
      pItem = m_ui->listHide->item(i);
      cid   = qvariant_cast<int>(pItem->data(Qt::UserRole));

      if (pItem->isSelected())
      {
         // means hide ...
         if (nowShow.contains(cid))
         {
            toHide.append(cid);
         }
      }
      else
      {
         // means show ...
         if (!nowShow.contains(cid))
         {
            toShow.append(cid);
         }
      }
   }

   // enqueue channel list changes ...
   if (toHide.count())
   {
      for (i = 0; i < toHide.count(); i++)
      {
         ids << QString::number(toHide[i]);
      }
      pApiClient->queueRequest(CIptvDefs::REQ_SETCHAN_HIDE, ids.join(","), sTempPasswd);
   }

   ids.clear();
   if (toShow.count())
   {
      for (i = 0; i < toShow.count(); i++)
      {
         ids << QString::number(toShow[i]);
      }
      pApiClient->queueRequest(CIptvDefs::REQ_SETCHAN_SHOW, ids.join(","), sTempPasswd);
   }

   if (toHide.count() || toShow.count())
   {
      // request new channel list ...
      pApiClient->queueRequest(CIptvDefs::REQ_CHANNELLIST);
   }

   //////////////////////////////////////////////////
   // VOD Manager ...
   //////////////////////////////////////////////////
   if (pAccountInfo->bHasVOD)
   {
      QVBoxLayout  *pMainLayout, *pChildLayout;
      QLayoutItem  *child;
      QRadioButton *pRadShow, *pRadHide, *pRadPass;
      QString       sRules, sAccess;
      pMainLayout = (QVBoxLayout *)m_ui->scrollAreaContents->layout();

      for (i = 0; i < pMainLayout->count(); i++)
      {
         // layout was buildt from rates vector so we can
         // assume that the index of the layout is equal
         // to the index in the rates vector ...
         if ((child = pMainLayout->itemAt(i)) != 0)
         {
            // simple check ...
            if (vodRatesVector[i].sGenre == child->widget()->objectName())
            {
               sAccess      = "";
               pChildLayout = (QVBoxLayout *)child->widget()->layout();

               // assume the order as created ...
               pRadShow = (QRadioButton *)pChildLayout->itemAt(1)->widget();
               pRadHide = (QRadioButton *)pChildLayout->itemAt(2)->widget();
               pRadPass = (QRadioButton *)pChildLayout->itemAt(3)->widget();

               if (pRadShow->isChecked())
               {
                  sAccess = "show";
               }
               else if (pRadHide->isChecked())
               {
                  sAccess = "hide";
               }
               else if (pRadPass->isChecked())
               {
                  sAccess = "pass";
               }

               if ((sAccess != "") && (sAccess != vodRatesVector[i].sAccess))
               {
                  sRules += QString("&%1=%2")
                        .arg(vodRatesVector[i].sGenre)
                        .arg(sAccess);
               }
            }
         }
      }

      if (sRules != "")
      {
         mInfo(tr("Changed VOD Rate: %1").arg(sRules));
         pApiClient->queueRequest(CIptvDefs::REQ_SET_VOD_MANAGER, sRules, sTempPasswd);
      }
   }

   QTimer::singleShot(1000, this, SLOT(slotLockParentalManager()));
}

/* -----------------------------------------------------------------\
|  Method: on_btnEnterManager_clicked [slot]
|  Begin: 22.05.2012
|  Author: Jo2003
|  Description: request channel list for settings used in channel
|               manager
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::on_btnEnterManager_clicked()
{
   sTempPasswd = m_ui->linePasswd->text();
   m_ui->linePasswd->clear();
   pApiClient->queueRequest(CIptvDefs::REQ_CHANLIST_ALL, sTempPasswd);
}

/* -----------------------------------------------------------------\
|  Method: on_linePasswd_returnPressed [slot]
|  Begin: 22.05.2012
|  Author: Jo2003
|  Description: request channel list for settings used in channel
|               manager
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::on_linePasswd_returnPressed()
{
   on_btnEnterManager_clicked();
}

/* -----------------------------------------------------------------\
|  Method: on_btnChgPCode_clicked [slot]
|  Begin: 01.06.2012
|  Author: Jo2003
|  Description: save new pcode ...
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::on_btnChgPCode_clicked()
{
   // precheck ...
   QRegExp rx("[^0-9]+");
   QString sOldPCode = m_ui->lineOldPCode->text();
   QString sNewPCode = m_ui->lineNewPCode->text();
   QString sConPCode = m_ui->lineConfirmPCode->text();


#ifndef _HAS_VOD_MANAGER
   if ((!sOldPCode.isEmpty())                         // old password is set
#else
   if ((sOldPCode == sTempPasswd)                     // old password is ok
#endif // _HAS_VOD_MANAGER
      && (sNewPCode == sConPCode)                     // new and confirm are equal
      && (sNewPCode.count() > 0)                      // there is a new password at all
      && ((sOldPCode.indexOf(rx) == -1)               // there are only numbers
          && (sNewPCode.indexOf(rx) == -1)))
   {
      // disable dialog items while we're settings ...
      m_ui->lineOldPCode->setDisabled(true);
      m_ui->lineNewPCode->setDisabled(true);
      m_ui->lineConfirmPCode->setDisabled(true);
      m_ui->btnChgPCode->setDisabled(true);

      pApiClient->queueRequest(CIptvDefs::REQ_SET_PCODE, sOldPCode, sNewPCode);
   }
   else
   {
      QMessageBox::critical(this, tr("Error!"),
                            tr("<b>Please check the data entered.</b>\n"
                               "<br /> <br />\n"
                               "To change the parent code make sure:\n"
                               "<ul>\n"
                               "<li>The old parent code is correct.</li>\n"
                               "<li>The new code and the confirm code are equal.</li>\n"
                               "<li>The new code isn't empty.</li>\n"
                               "<li>The new code contains <b style='color: red;'>numbers only</b>.</li>\n"
                               "</ul>\n"));

      // clear password forms ...
      m_ui->lineOldPCode->clear();
      m_ui->lineNewPCode->clear();
      m_ui->lineConfirmPCode->clear();
   }
}

/* -----------------------------------------------------------------\
|  Method: slotNewPCodeSet [slot]
|  Begin: 01.06.2012
|  Author: Jo2003
|  Description: got pcode change response
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::slotNewPCodeSet()
{
   QString oldPw;

   // internally store changed pcode ...
   sTempPasswd = m_ui->lineNewPCode->text();

   // Was there a code stored for adult channels ... ?
   // To be sure ask database, but not text field.
   if ((oldPw = pDb->password("ErosPasswdEnc")) != "")
   {
      // there was an adult code stored ...
      if (sTempPasswd != oldPw)
      {
         // update eros passwd if set ...
         m_ui->lineErosPass->setText(sTempPasswd);

         // save to database ...
         pDb->setPassword("ErosPasswdEnc", sTempPasswd);
      }
   }

   slotEnablePCodeForm();

   QMessageBox::information(this, tr("Information"), tr("Parent Code successfully changed."));
   mInfo(tr("Parent Code successfully changed."));

   // on error we'll get a message box from the Recorder ...
}

/* -----------------------------------------------------------------\
|  Method: slotEnablePCodeForm [slot]
|  Begin: 05.06.2012
|  Author: Jo2003
|  Description: enable dialog items
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CSettingsDlg::slotEnablePCodeForm()
{
   // clear form ...
   m_ui->lineOldPCode->clear();
   m_ui->lineNewPCode->clear();
   m_ui->lineConfirmPCode->clear();

   // enable items ...
   m_ui->lineOldPCode->setEnabled(true);
   m_ui->lineNewPCode->setEnabled(true);
   m_ui->lineConfirmPCode->setEnabled(true);
   m_ui->btnChgPCode->setEnabled(true);
}

//---------------------------------------------------------------------------
//
//! \brief   we've got speed test data -> ctreate dialog!
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \param   spdData [in] (QSpeedDataVector) speed test data
//---------------------------------------------------------------------------
void CSettingsDlg::slotSpeedTestData(QSpeedDataVector spdData)
{
    m_pSpdTestDlg->setData(spdData);

    QPoint p = mapToGlobal(m_ui->pushSpeedTest->pos());
    m_pSpdTestDlg->move(p.x(), p.y());

    if (m_pSpdTestDlg->exec() == QDialog::Accepted)
    {
        if (!m_pSpdTestDlg->chosenOne().isEmpty())
        {
            m_ui->chkStrSrvAuto->setChecked(false);
            autoSetStreamServer(m_pSpdTestDlg->chosenOne());
        }

        if (m_pSpdTestDlg->nominalBr() > 0)
        {
            mInfo(tr("Set new nominal bitrate: %1").arg(m_pSpdTestDlg->nominalBr()));
            int idx = m_ui->cbxBitRate->findData(m_pSpdTestDlg->nominalBr());

            if ((idx != -1) && (idx != m_ui->cbxBitRate->currentIndex()))
            {
                m_ui->cbxBitRate->setCurrentIndex(idx);
                on_cbxBitRate_activated(idx);
            }
        }
    }

    // in case there is something running ...
    m_pSpdTestDlg->on_btnStop_clicked();
}

//---------------------------------------------------------------------------
//
//! \brief   change language from outsite
//
//! \author  Jo2003
//! \date    24.07.2013
//
//! \param   lng (const QString&) language code to set
//
//! \return  0 --> ok; -1 --> language not available
//---------------------------------------------------------------------------
int CSettingsDlg::setLanguage(const QString &lng)
{
   int idx = -1;

   // language available ... ?
   if ((idx = m_ui->cbxLanguage->findText(lng)) != -1)
   {
      // different from current language ... ?
      if (idx != m_ui->cbxLanguage->currentIndex())
      {
         // set new language ...
         m_ui->cbxLanguage->setCurrentIndex(idx);

         // save it internal too ...
         pDb->setValue("Language", lng);
      }
   }

   return (idx > -1) ? 0 : idx;
}

//---------------------------------------------------------------------------
//
//! \brief   set username from outsite
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   str (const QString&) new username
//
//! \return  --
//---------------------------------------------------------------------------
void CSettingsDlg::setUser(const QString& str)
{
   m_ui->lineUsr->setText(str);
}

//---------------------------------------------------------------------------
//
//! \brief   set password from outsite
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   str (const QString&) new password
//
//---------------------------------------------------------------------------
void CSettingsDlg::setPasswd(const QString& str)
{
   m_ui->linePass->setText(str);
}

//---------------------------------------------------------------------------
//
//! \brief   set API server from outsite
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   str [in] (const QString&) new server
//
//---------------------------------------------------------------------------
void CSettingsDlg::setApiSrv(const QString &str)
{
   m_ui->lineApiServer->setText(str);
}

//---------------------------------------------------------------------------
//
//! \brief   set active stream server
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   str [in] (const QString&) new server
//
//---------------------------------------------------------------------------
void CSettingsDlg::setActiveStreamServer(const QString &str)
{
   int idx = m_ui->cbxStreamServer->findData(str);

   if (idx > -1)
   {
      m_ui->cbxStreamServer->setCurrentIndex(idx);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set active time shift
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   val [in] (int) new timeshift
//
//---------------------------------------------------------------------------
void CSettingsDlg::setActiveTimeshift(int val)
{
   int idx = m_ui->cbxTimeShift->findData(val);

   if (idx > -1)
   {
      m_ui->cbxTimeShift->setCurrentIndex(idx);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set active bitrate
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   val [in] (int) new bitrate
//
//---------------------------------------------------------------------------
void CSettingsDlg::setActiveBitrate(int val)
{
   int idx = m_ui->cbxBitRate->findData(val) ;

   if (idx > -1)
   {
      m_ui->cbxBitRate->setCurrentIndex(idx);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set active buffer time
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   val [in] (int) new time
//
//---------------------------------------------------------------------------
void CSettingsDlg::setActiveBuffer(int val)
{
   int idx = m_ui->cbxBufferSeconds->findData(val);

   if (idx > -1)
   {
      m_ui->cbxBufferSeconds->setCurrentIndex(idx);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get font size delta
//
//! \author  Jo2003
//! \date    03.06.2014
//
//! \param   --
//
//! \return  font size delta
//---------------------------------------------------------------------------
int CSettingsDlg::getFontDelta()
{
   return m_ui->spinBoxFontDelta->value();
}

//---------------------------------------------------------------------------
//
//! \brief   set font size delta
//
//! \author  Jo2003
//! \date    03.06.2014
//
//! \param   i [in] (int) new size difference
//
//! \return  --
//---------------------------------------------------------------------------
void CSettingsDlg::setFontDelta(int i)
{
    m_ui->spinBoxFontDelta->setValue(i);
}

//---------------------------------------------------------------------------
//
//! \brief   set stream standard data to dialog
//
//! \author  Jo2003
//! \date    31.08.2015
//
//! \param   data [in] (const cparser::QStrStdMap&) stream standard data
//! \param   curr [in] (const QString&) current stream standard
//
//! \return  --
//---------------------------------------------------------------------------
void CSettingsDlg::setStrStdData(const cparser::QStrStdMap &data, const QString &curr)
{
    m_pStrStdDlg->setStrStdData(data, curr);
}

//---------------------------------------------------------------------------
//
//! \brief   set stream server to value in GUI, emit signal
//
//! \author  Jo2003
//! \date    14.09.2015
//
//! \param   curr [in] (const QString&) auto stream server
//
//! \return  --
//---------------------------------------------------------------------------
void CSettingsDlg::autoSetStreamServer(const QString &curr)
{
    int idx = m_ui->cbxStreamServer->findData(curr);

    if (idx != -1)
    {
        m_ui->cbxStreamServer->setCurrentIndex(idx);
        emit sigSetServer(curr);
    }
}

//---------------------------------------------------------------------------
//
//! \brief   get bitrate map from bitrate cbx
//
//! \author  Jo2003
//! \date    15.11.2015
//
//! \param   brMap [out] (QMap<int, QString> &) map to fill
//
//! \return  --
//---------------------------------------------------------------------------
void CSettingsDlg::getBrMap(QMap<int, QString> &brMap) const
{
    int      val;
    QString  text;
    brMap.clear();

    for (int i = 0; i < m_ui->cbxBitRate->count(); i++)
    {
        val  = m_ui->cbxBitRate->itemData(i).toInt();
        text = m_ui->cbxBitRate->itemText(i);

        brMap[val] = text;
    }

}

//---------------------------------------------------------------------------
//
//! \brief   do we need a re-login?
//
//! \author  Jo2003
//! \date    15.11.2015
//
//! \return  changed mask
//---------------------------------------------------------------------------
ulong CSettingsDlg::savedChanges() const
{
    return m_ulChgdMask;
}

//---------------------------------------------------------------------------
//
//! \brief   compare values before save to find out if re-login is needed
//
//! \author  Jo2003
//! \date    15.11.2015
//
//---------------------------------------------------------------------------
void CSettingsDlg::checkChanges()
{
    QString sCmp;
    int     iErr, iState;

    m_ulChgdMask = 0;

    // user credentials
    if ((m_ui->lineUsr->text() != pDb->stringValue("User"))
        || (m_ui->linePass->text() != pDb->password("PasswdEnc")))
    {
        mInfo(tr("Trigger Re-Login. Reason: %1").arg(tr("Credentials changed")));
        m_ulChgdMask |= CREDENTIALS_CHG;
    }

    // api server
    sCmp = pDb->stringValue("APIServer").isEmpty() ? pCustomization->strVal("API_SERVER") : pDb->stringValue("APIServer");
    if (sCmp != m_ui->lineApiServer->text())
    {
        mInfo(tr("Trigger Re-Login. Reason: %1").arg(tr("API server changed")));
        m_ulChgdMask |= API_SERVER_CHG;
    }

    // auto stream server
    if (m_ui->chkStrSrvAuto->isChecked())
    {
        iState = pDb->intValue("AutoStrSrv", &iErr);
        if (iErr)
        {
            iState = (int)Qt::Checked;
        }

        if (iState != (int)m_ui->chkStrSrvAuto->checkState())
        {
            mInfo(tr("Trigger Re-Login. Reason: %1").arg(tr("Stream Server 'AUTO' setting activated")));
            m_ulChgdMask |= STREAM_SERVER_CHG;
        }
    }

    // proxy user / password
    if ((m_ui->lineProxyHost->text()        != pDb->stringValue("ProxyHost"))
        || (m_ui->lineProxyPort->text()     != pDb->stringValue("ProxyPort"))
        || (m_ui->lineProxyUser->text()     != pDb->stringValue("ProxyUser"))
        || (m_ui->lineProxyPassword->text() != pDb->password("ProxyPasswdEnc"))
        || (m_ui->useProxy->checkState()    != (Qt::CheckState)pDb->intValue("UseProxy")))
    {
        mInfo(tr("Trigger Re-Login. Reason: %1").arg(tr("Proxy settings changed")));
        m_ulChgdMask |= PROXY_CHG;
    }

    // language change ...
    if (m_ui->cbxLanguage->currentText() != pDb->stringValue("Language"))
    {
        mInfo(tr("Trigger Re-Login. Reason: %1").arg(tr("Language changed")));
        m_ulChgdMask |= LANG_CHG;
    }

    // player module
    sCmp = pDb->stringValue("PlayerModule").isEmpty() ? "5_libvlc.mod" : pDb->stringValue("PlayerModule");
    if (m_ui->cbxPlayerMod->currentText() != sCmp)
    {
        mInfo(tr("Trigger player module change"));
        m_ulChgdMask |= PLAY_MOD_CHG;
    }
}

//---------------------------------------------------------------------------
//
//! \brief   font delta was changed -> tell about
//
//! \author  Jo2003
//! \date    03.06.2014
//
//! \param   arg1 [in] (int) new size difference
//
//! \return  --
//---------------------------------------------------------------------------
void CSettingsDlg::on_spinBoxFontDelta_valueChanged (int arg1)
{
   if (bSettingsRead)
   {
      // get delta ...
      int delta = arg1 - pDb->intValue("CustFontSz");

      // font size ...
      pDb->setValue("CustFontSz", arg1);

      emit sigFontDeltaChgd(delta);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   open stream standard dialog
//
//! \author  Jo2003
//! \date    31.08.2015
//
//---------------------------------------------------------------------------
void CSettingsDlg::on_pushStrStd_clicked()
{
    QPoint p = mapToGlobal(m_ui->pushStrStd->pos());
    m_pStrStdDlg->move(p.x(), p.y());

    if (m_pStrStdDlg->exec() == QDialog::Accepted)
    {
        // any change in name ... ?
        if (m_ui->pushStrStd->text() != m_pStrStdDlg->getCurrName())
        {
            handleStrStdDeps();

            emit sigSetStrStd(m_pStrStdDlg->getCurrVal());
        }
    }
    else
    {
        // revert any change ...
        m_pStrStdDlg->setCurrName(m_ui->pushStrStd->text());
    }
}

//---------------------------------------------------------------------------
//
//! \brief   request speed test data
//
//! \author  Jo2003
//! \date    14.09.2015
//
//---------------------------------------------------------------------------
void CSettingsDlg::on_pushSpeedTest_clicked()
{
    QSpeedDataVector spdData = m_pSpdTestDlg->data();

    if (spdData.isEmpty())
    {
        // only request data if not already there
        emit sigReqSpeedData();
    }
    else
    {
        // open dialog
        slotSpeedTestData(spdData);
    }
}

/************************* History ***************************\
| $Log$
\*************************************************************/
