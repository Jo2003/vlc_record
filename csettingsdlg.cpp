/*********************** Information *************************\
| $HeadURL$
| 
| Author: Joerg Neubert
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
|  Author: Joerg Neubert
|  Description: construct dialog, init values
|
|  Parameters: pointer to translater, pointer to parent widget
|
|  Returns: --
\----------------------------------------------------------------- */
CSettingsDlg::CSettingsDlg(QTranslator *trans, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CSettingsDlg)
{
   m_ui->setupUi(this);
   pTranslator = trans;

   // set ini file name and open ini file ...
   IniFile.SetFileName(QString(INI_DIR).arg(getenv(APPDATA)).toLocal8Bit().data(), INI_FILE);
   IniFile.ReadIni();

   // fill in values ...

   // line edits ...
   m_ui->lineVLC->setText (IniFile.GetStringData("VLCPath"));
   m_ui->lineDir->setText (IniFile.GetStringData("TargetDir"));
   m_ui->lineUsr->setText (IniFile.GetStringData("User"));
   m_ui->linePass->setText (IniFile.GetStringData("Passwd"));

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

   // combo boxes ...
   int iIdx = m_ui->cbxLanguage->findText(IniFile.GetStringData("Language"));
   m_ui->cbxLanguage->setCurrentIndex((iIdx < 0) ? 0 : iIdx);
   m_ui->cbxLogLevel->setCurrentIndex((int)IniFile.GetIntData("LogLevel"));

   // set language as read ...
   pTranslator->load(QString("lang_%1").arg(m_ui->cbxLanguage->currentText()), QApplication::applicationDirPath());
}

/* -----------------------------------------------------------------\
|  Method: exec
|  Begin: 19.01.2010 / 15:44:46
|  Author: Joerg Neubert
|  Description: show dialog window, make sure right value in
|               log level is shown
|
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
int CSettingsDlg::exec ()
{
   m_ui->cbxLogLevel->setCurrentIndex((int)IniFile.GetIntData("LogLevel"));
   return QDialog::exec();
}

/* -----------------------------------------------------------------\
|  Method: ~CSettingsDlg / dstructor
|  Begin: 19.01.2010 / 15:45:48
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/* -----------------------------------------------------------------\
|  Method: on_pushVLC_clicked
|  Begin: 19.01.2010 / 15:46:47
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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

   IniFile.AddData("ProxyHost", m_ui->lineProxyHost->text());
   IniFile.AddData("ProxyPort", m_ui->lineProxyPort->text());
   IniFile.AddData("ProxyUser", m_ui->lineProxyUser->text());
   IniFile.AddData("ProxyPasswd", m_ui->lineProxyPassword->text());
   IniFile.AddData("RefIntv", m_ui->lineInterval->text());

   // check boxes ...
   IniFile.AddData("UseProxy", (int)m_ui->useProxy->checkState());
   IniFile.AddData("AllowAdult", (int)m_ui->checkAdult->checkState());
   IniFile.AddData("FixTime", (int)m_ui->checkFixTime->checkState());
   IniFile.AddData("Refresh", (int)m_ui->checkRefresh->checkState());

   // combo boxes ...
   IniFile.AddData("Language", m_ui->cbxLanguage->currentText());
   IniFile.AddData("LogLevel", m_ui->cbxLogLevel->currentIndex());

   IniFile.SaveIni();
}

/* -----------------------------------------------------------------\
|  Method: on_pushDelLogos_clicked
|  Begin: 19.01.2010 / 15:48:39
|  Author: Joerg Neubert
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

bool CSettingsDlg::UseProxy ()
{
   return (m_ui->useProxy->checkState() == Qt::Checked) ? true : false;
}

bool CSettingsDlg::AllowEros()
{
   return (m_ui->checkAdult->checkState() == Qt::Checked) ? true : false;
}

bool CSettingsDlg::FixTime()
{
   return (m_ui->checkFixTime->checkState() == Qt::Checked) ? true : false;
}

bool CSettingsDlg::DoRefresh()
{
   return (m_ui->checkRefresh->checkState() == Qt::Checked) ? true : false;
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

void CSettingsDlg::on_cbxLanguage_currentIndexChanged(QString str)
{
   pTranslator->load(QString("lang_%1").arg(str), QApplication::applicationDirPath());
}

vlclog::eLogLevel CSettingsDlg::GetLogLevel()
{
   return (vlclog::eLogLevel)m_ui->cbxLogLevel->currentIndex();
}

//===================================================================
// <== return internal stored values
//===================================================================

/************************* History ***************************\
| $Log$
\*************************************************************/

