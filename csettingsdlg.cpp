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

   // set ini file name and open ini file ...
   IniFile.SetFileName(QString(INI_DIR).arg(getenv(APPDATA)).toLocal8Bit().data(), INI_FILE);

   IniFile.ReadIni();

   // fill in values ...

   // line edits ...
   m_ui->lineVLC->setText (IniFile.GetStringData("VLCPath"));
   m_ui->lineDir->setText (IniFile.GetStringData("TargetDir"));
   m_ui->lineUsr->setText (IniFile.GetStringData("User"));
   m_ui->linePass->setText (IniFile.GetStringData("Passwd"));
   m_ui->lineErosPass->setText(IniFile.GetStringData("ErosPasswd"));
   m_ui->lineShutdown->setText(IniFile.GetStringData("ShutdwnCmd"));

#ifdef Q_OS_WIN32
   if (m_ui->lineShutdown->text() == "")
   {
      m_ui->lineShutdown->setText ("shutdown.exe -s -f -t 5");
   }
#endif

   m_ui->lineProxyHost->setText(IniFile.GetStringData("ProxyHost"));
   m_ui->lineProxyPort->setText(IniFile.GetStringData("ProxyPort"));
   m_ui->lineProxyUser->setText(IniFile.GetStringData("ProxyUser"));
   m_ui->lineProxyPassword->setText(IniFile.GetStringData("ProxyPasswd"));
   m_ui->lineInterval->setText(IniFile.GetStringData("RefIntv"));

   // check boxes ...
   m_ui->useProxy->setCheckState((Qt::CheckState)IniFile.GetIntData("UseProxy"));
   m_ui->checkAdult->setCheckState((Qt::CheckState)IniFile.GetIntData("AllowAdult"));
   m_ui->checkFixTime->setCheckState((Qt::CheckState)IniFile.GetIntData("FixTime"));
   m_ui->checkRefresh->setCheckState((Qt::CheckState)IniFile.GetIntData("Refresh"));
   m_ui->checkHideToSystray->setCheckState((Qt::CheckState)IniFile.GetIntData("TrayHide"));
   m_ui->checkAskForName->setCheckState((Qt::CheckState)IniFile.GetIntData("AskRecFile"));
   m_ui->checkTranslit->setCheckState((Qt::CheckState)IniFile.GetIntData("TranslitRecFile"));
   m_ui->checkDetach->setCheckState((Qt::CheckState)IniFile.GetIntData("DetachPlayer"));

   // on update the password for adult channels may not be given ...
   if (m_ui->checkAdult->isChecked() && (m_ui->lineErosPass->text() == ""))
   {
      m_ui->lineErosPass->setText(m_ui->linePass->text());
   }

   // fill player module box with available modules ...
   QDir appDir (QApplication::applicationDirPath());
   m_ui->cbxPlayerMod->addItems(appDir.entryList(QStringList("*.mod"), QDir::Files, QDir::Name));

   // combo boxes ...
   int iIdx;
   iIdx = m_ui->cbxLanguage->findText(IniFile.GetStringData("Language"));
   m_ui->cbxLanguage->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   iIdx = m_ui->cbxBufferSeconds->findText(IniFile.GetStringData("HttpCache"));
   m_ui->cbxBufferSeconds->setCurrentIndex((iIdx < 0) ? 0 : iIdx);

   m_ui->cbxLogLevel->setCurrentIndex((int)IniFile.GetIntData("LogLevel"));

   iIdx = m_ui->cbxPlayerMod->findText(IniFile.GetStringData("PlayerModule"));
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

          m_ui->retranslateUi(this);

          // re-set index to comboboxes ...
          m_ui->cbxLanguage->setCurrentIndex(iLanIdx);
          m_ui->cbxLogLevel->setCurrentIndex(iLogIdx);
          m_ui->cbxBufferSeconds->setCurrentIndex(iBufIdx);
          m_ui->cbxPlayerMod->setCurrentIndex(iModIdx);

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
   IniFile.AddData("VLCPath", m_ui->lineVLC->text());
   IniFile.AddData("User", m_ui->lineUsr->text());
   IniFile.AddData("TargetDir", m_ui->lineDir->text());
   IniFile.AddData("Passwd", m_ui->linePass->text());
   IniFile.AddData("ErosPasswd", m_ui->lineErosPass->text());

   IniFile.AddData("ProxyHost", m_ui->lineProxyHost->text());
   IniFile.AddData("ProxyPort", m_ui->lineProxyPort->text());
   IniFile.AddData("ProxyUser", m_ui->lineProxyUser->text());
   IniFile.AddData("ProxyPasswd", m_ui->lineProxyPassword->text());
   IniFile.AddData("RefIntv", m_ui->lineInterval->text());
   IniFile.AddData("ShutdwnCmd", m_ui->lineShutdown->text());

   // check boxes ...
   IniFile.AddData("UseProxy", (int)m_ui->useProxy->checkState());
   IniFile.AddData("AllowAdult", (int)m_ui->checkAdult->checkState());
   IniFile.AddData("FixTime", (int)m_ui->checkFixTime->checkState());
   IniFile.AddData("Refresh", (int)m_ui->checkRefresh->checkState());
   IniFile.AddData("TrayHide", (int)m_ui->checkHideToSystray->checkState());
   IniFile.AddData("AskRecFile", (int)m_ui->checkAskForName->checkState());
   IniFile.AddData("TranslitRecFile", (int)m_ui->checkTranslit->checkState());
   IniFile.AddData("DetachPlayer", (int)m_ui->checkDetach->checkState());

   // combo boxes ...
   IniFile.AddData("Language", m_ui->cbxLanguage->currentText());
   IniFile.AddData("HttpCache", m_ui->cbxBufferSeconds->currentText());
   IniFile.AddData("LogLevel", m_ui->cbxLogLevel->currentIndex());
   IniFile.AddData("PlayerModule", m_ui->cbxPlayerMod->currentText());

   IniFile.SaveIni();
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
   // create logo path ...
   QString sPath = QString(LOGO_DIR).arg(getenv(APPDATA)).toLocal8Bit().data();

   // get directory, where logos are located ...
   QDir logoDir(sPath);

   // get all files inside this dir ...
   QStringList fileList = logoDir.entryList(QDir::Files);

   // delete all files inside logo dir ...
   for (int i = 0; i < fileList.size(); i++)
   {
      QFile::remove(QString("%1/%2").arg(sPath).arg(fileList[i]));
   }

   emit sigReloadLogos();
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
   // available servers: 1 and 3!
   // index 0 --> server 1
   // index 1 --> server 3
   int iSrv = m_ui->cbxStreamServer->currentIndex();

   if (iSrv == 0)
   {
      iSrv = 1;
   }
   else if (iSrv == 1)
   {
      iSrv = 3;
   }
   else
   {
      iSrv = -1;
   }

   if (iSrv != -1)
   {
      emit sigSetServer(iSrv);
   }
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

   IniFile.AddData ("WndRect", sGeo);

   IniFile.SaveIni();
}

/* -----------------------------------------------------------------\
|  Method: GetWindowRect
|  Begin: 27.01.2010 / 11:22:39
|  Author: Jo2003
|  Description: get windows position / size from ini file
|
|  Parameters: ref. to QRect
|
|  Returns:  0 ==> ok
|           -1 ==> any error
\----------------------------------------------------------------- */
QRect CSettingsDlg::GetWindowRect (bool *ok)
{
   QString sGeo = IniFile.GetStringData("WndRect");
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
   return QString("%1/%2").arg(QApplication::applicationDirPath())
         .arg(m_ui->cbxPlayerMod->currentText());
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
   int i = m_ui->lineInterval->text().toInt();

   // make sure the interfal is not 0 !!!
   if (i < 1)
   {
      i = 1;
   }

   return i;
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

//===================================================================
// <== return internal stored values
//===================================================================

/************************* History ***************************\
| $Log$
\*************************************************************/

