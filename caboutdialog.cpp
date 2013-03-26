/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:13:41
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "caboutdialog.h"
#include "ui_caboutdialog.h"

#include "qcustparser.h"

// global customization class ...
extern QCustParser *pCustomization;

/* -----------------------------------------------------------------\
|  Method: CAboutDialog / constructor
|  Begin: 18.01.2010 / 16:14:06
|  Author: Jo2003
|  Description: constructs about dialog class
|
|  Parameters: pointer to parent window
|
|  Returns: --
\----------------------------------------------------------------- */
CAboutDialog::CAboutDialog(QWidget *parent, QString sExpires) :
    QDialog(parent),
    ui(new Ui::CAboutDialog)
{
   ui->setupUi(this);
   FillInfo(sExpires);
   ui->textBrowser->setHtml(strAbout);
}

/* -----------------------------------------------------------------\
|  Method: ~CAboutDialog / destructor
|  Begin: 18.01.2010 / 16:14:44
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CAboutDialog::~CAboutDialog()
{
    delete ui;
}

/* -----------------------------------------------------------------\
|  Method: ConnectSettings
|  Begin: 08.03.2010 / 14:14:44
|  Author: Jo2003
|  Description: connect settings with splash options ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CAboutDialog::ConnectSettings(CSettingsDlg *pSet)
{
   ui->checkDontShowOnStartup->setChecked(pSet->DisableSplashScreen());
   connect (ui->checkDontShowOnStartup, SIGNAL(clicked(bool)), pSet, SLOT(slotSplashStateChgd(bool)));
}

/* -----------------------------------------------------------------\
|  Method: changeEvent
|  Begin: 18.01.2010 / 16:15:15
|  Author: Jo2003
|  Description: catch language change event
|
|  Parameters: pointer to event
|
|  Returns: --
\----------------------------------------------------------------- */
void CAboutDialog::changeEvent(QEvent *e)
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
|  Method: FillInfo
|  Begin: 18.01.2010 / 16:15:50
|  Author: Jo2003
|  Description: fill about info into string
|
|  Parameters: sExpires - string with date and time
|
|  Returns: --
\----------------------------------------------------------------- */
void CAboutDialog::FillInfo(QString sExpires)
{
   strAbout = "";
   QTextStream str(&strAbout);

   str << "<style type='text/css'>" << endl
         << "a:link, a:visited, a:active { text-decoration: underline; color: #0482FE;}" << endl
         << "</style>" << endl
         << "<span style='font-weight: bold; font-size: 16px;'>" << pCustomization->strVal("APP_NAME") << "</span><br />" << endl
         << "<table border='0' cellpadding='0' cellspacing='0'>" << endl
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'>%2</td></tr>").arg(tr("Version:")).arg(__MY__VERSION__) << endl
#ifdef INCLUDE_LIBVLC
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'>%2</td></tr>").arg(tr("libVLC:"))
             .arg(QString("%1").arg(libvlc_get_version())) << endl
#endif
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'><a href='mailto:coujo@gmx.net'>Jo2003</a></td></tr>").arg(tr("Author:")) << endl
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'><a href='http://vlc-record.coujo.de'>vlc-record.coujo.de</a></td></tr>").arg(tr("Project Site:")) << endl
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'>Olenka!</td></tr>").arg(tr("Inspired by:")) << endl
         << QString("<tr><td><b>SDK:</b></td><td style='padding-left: 15px;'>Qt %2 by <a href='http://qt.digia.com'>Digia</a></td></tr>").arg(qVersion()) << endl
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'>%2</td></tr>").arg(tr("Account expires:")).arg(sExpires) << endl
         << "<tr><td><b>Most icons:</b></td><td style='padding-left: 15px;'>by <a href='http://dryicons.com'>dryicons.com</a></td></tr>" << endl
         << "</table><br /> <br />" << endl
         << tr("<b>This program is free software!</b>") << endl
         << tr("Nevertheless ... if you like this software, please support me: ") << endl
         << "<a href='https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=11286909'>"
         << tr("Donate some $ and / or &euro; at Paypal for my work.") << "</a><br /> <br />" << endl
         << tr("Thank you,") << "<br />&nbsp;&nbsp;&nbsp;&nbsp;J&ouml;rg" << endl;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

