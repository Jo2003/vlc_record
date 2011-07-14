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

   if (pShortApiServer)
   {
      connect(pShortApiServer, SIGNAL(activated()), this, SLOT(slotEnableApiServer()));
   }

   // set company name for login data ...
   QString s = m_ui->groupAccount->title();
   m_ui->groupAccount->setTitle(s.arg(COMPANY_NAME));

   // fill in values ...

   // line edits ...
   m_ui->lineVLC->setText (pDb->stringValue("VLCPath"));
   m_ui->lineDir->setText (pDb->stringValue("TargetDir"));
   m_ui->lineUsr->setText (pDb->stringValue("User"));
   m_ui->linePass->setText (pDb->stringValue("Passwd"));
   m_ui->lineErosPass->setText(pDb->stringValue("ErosPasswd"));
   m_ui->lineShutdown->setText(pDb->stringValue("ShutdwnCmd"));
   m_ui->lineUser->setText(pDb->stringValue ("RegUser"));
   m_ui->lineRegData->setText(pDb->stringValue ("RegData"));
   m_ui->lineApiServer->setText(pDb->stringValue ("APIServer"));

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

   // on update the password for adult channels may not be given ...
   if (m_ui->checkAdult->isChecked() && (m_ui->lineErosPass->text() == ""))
   {
      m_ui->lineErosPass->setText(m_ui->linePass->text());
   }

   // fill player module box with available modules ...
   QDir modDir(pFolders->getModDir());
   m_ui->cbxPlayerMod->addItems(modDir.entryList(QStringList("*.mod"), QDir::Files, QDir::Name));

   // combo boxes ...
   int iIdx;
   iIdx = m_ui->cbxLanguage->findText(pDb->stringValue("Language"));
   m_ui->cbxLanguage->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   iIdx = m_ui->cbxBufferSeconds->findText(pDb->stringValue("HttpCache"));
   m_ui->cbxBufferSeconds->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   iIdx = m_ui->cbxInterval->findText(pDb->stringValue("RefIntv"));
   m_ui->cbxInterval->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   m_ui->cbxLogLevel->setCurrentIndex((int)pDb->intValue("LogLevel"));

   iIdx = m_ui->cbxPlayerMod->findText(pDb->stringValue("PlayerModule"));
   m_ui->cbxPlayerMod->setCurrentIndex((iIdx < 0) ? 0 : iIdx);
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
    switch (e->type()) {
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

   // combo boxes ...
   pDb->setValue("Language", m_ui->cbxLanguage->currentText());
   pDb->setValue("HttpCache", m_ui->cbxBufferSeconds->currentText());
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


   m_ui->cbxBitRate->clear();

   // add all available bitrates ...
   for (cit = vValues.constBegin(); cit != vValues.constEnd(); cit++)
   {
      m_ui->cbxBitRate->addItem(QString::number(*cit), QVariant(*cit));

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
|  Method: on_btnSaveStreamServer_clicked
|  Begin: 21.01.2010 / 11:22:39
|  Author: Jo2003
|  Description: signal set of stream server
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_btnSaveStreamServer_clicked()
{
   // which server was choosed ... ?
   int iSrv  = m_ui->cbxStreamServer->currentIndex();

   emit sigSetServer(m_ui->cbxStreamServer->itemData(iSrv).toString());
}

/* -----------------------------------------------------------------\
|  Method: on_btnSaveBitrate_clicked
|  Begin: 14.01.2011 / 14:45
|  Author: Jo2003
|  Description: button set bitrate pressed
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CSettingsDlg::on_btnSaveBitrate_clicked()
{
   // which bitrate was used ... ?
   int iRate = m_ui->cbxBitRate->currentIndex();

   emit sigSetBitRate(m_ui->cbxBitRate->itemData(iRate).toInt());
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
|  Method: SaveOtherSettings
|  Begin: 18.02.2010 / 11:22:39
|  Author: Jo2003
|  Description: write ini file to disk
|
|  Parameters: --
|
|  Returns:  0 --> ok
|           -1 --> any error
\----------------------------------------------------------------- */
int CSettingsDlg::SaveOtherSettings()
{
   return 0; /*IniFile.SaveIni(); */
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
   return m_ui->cbxBufferSeconds->currentText().toInt();
}

QString CSettingsDlg::GetShutdownCmd()
{
   return m_ui->lineShutdown->text();
}

bool CSettingsDlg::DisableSplashScreen()
{
   return (pDb->intValue("NoSplash")) ? true : false;
}

bool CSettingsDlg::regOk()
{
   return (hsah(m_ui->lineUser->text()) == m_ui->lineRegData->text()) ? true : false;
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

//===================================================================
// <== return internal stored values
//===================================================================

/* -----------------------------------------------------------------\
|  Method: hsah (hash reversed ;-) )
|  Begin: 23.12.2010 / 11:45
|  Author: Jo2003
|  Description: make a hash code
|
|  Parameters: string to hash
|
|  Returns:  hash
\----------------------------------------------------------------- */
QString CSettingsDlg::hsah (const QString &str)
{
   int i = 0;
   QByteArray arr = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
   QStringList list;
   QString sTmp(arr.toHex());

   while (i < sTmp.length())
   {
      list.push_back(sTmp.mid(i, 8));
      i += 8;
   }

   sTmp.clear();

   for (i = 0; i <  list.count(); i++)
   {
      sTmp += (i ? QString("-") : QString("")) + reverse(list[i]);
   }

   return QString(QCryptographicHash::hash(sTmp.toUtf8(), QCryptographicHash::Sha1).toHex());
}

/* -----------------------------------------------------------------\
|  Method: reverse
|  Begin: 23.12.2010 / 11:45
|  Author: Jo2003
|  Description: reverse a string
|
|  Parameters: ref. to string
|
|  Returns:  ref. to string
\----------------------------------------------------------------- */
QString& CSettingsDlg::reverse(QString &str)
{
   int i, j = 0;
   QString sTmp = str;

   str.clear();

   for (i = sTmp.count() - 1; i >= 0; i--)
   {
      str[j++] = sTmp[i];
   }

   return str;
}

/* -----------------------------------------------------------------\
|  Method: reverse
|  Begin: 23.12.2010 / 11:45
|  Author: Jo2003
|  Description: reverse a string
|
|  Parameters: ref. to string
|
|  Returns:  ref. to string
\----------------------------------------------------------------- */
void CSettingsDlg::on_pushDoRegister_clicked()
{
   if (hsah(m_ui->lineUser->text()) != m_ui->lineRegData->text())
   {
      if (hsah(m_ui->lineRegData->text()) == MASTER_HASH)
      {
         QMessageBox::information(this, tr("Reg Info"), hsah(m_ui->lineUser->text()));
      }
   }

   pDb->setValue ("RegUser", m_ui->lineUser->text());
   pDb->setValue ("RegData", m_ui->lineRegData->text());
}

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

