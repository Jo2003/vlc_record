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
CAboutDialog::CAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAboutDialog)
{
   ui->setupUi(this);
   FillInfo();
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
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CAboutDialog::FillInfo()
{
   strAbout = "";
   QTextStream str(&strAbout);

   str << "<style type='text/css'>" << endl
         << "a:link, a:visited, a:active { text-decoration: underline; color: #0482FE;}" << endl
         << "</style>" << endl
         << "<span style='font-weight: bold; font-size: 16px;'>vlc-record</span><br />" << endl
         << "<table border='0' cellpadding='0' cellspacing='0'>" << endl
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'>%2</td></tr>").arg(tr("Version:")).arg(__MY__VERSION__) << endl
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'><a href='mailto:coujo@gmx.net'>Jo2003</a></td></tr>").arg(tr("Author:")) << endl
         << QString("<tr><td><b>%1</b></td><td style='padding-left: 15px;'>Olenka!</td></tr>").arg(tr("Inspired by:")) << endl
         << QString("<tr><td><b>SDK:</b></td><td style='padding-left: 15px;'>Qt %2 by <a href='http://qt.nokia.com'>Nokia</a></td></tr>").arg(qVersion()) << endl
         << "<tr><td><b>Icons:</b></td><td style='padding-left: 15px;'>by <a href='http://dryicons.com'>dryicons.com</a></td></tr>" << endl
         << "</table><br /> <br />" << endl
         << tr("<b>vlc-record</b> - a tool to <b>view / record program streams</b> sent from %1.").arg(COMPANY_LINK) << endl
         << tr("To do this it will authenticate you with username and password at %1.").arg(COMPANY_NAME) << endl
         << tr("It then will start the %1 to display (and record) the selected stream.").arg("<a href='http://www.videolan.org'>vlc media player</a>") << endl
         << QString("%1<br />").arg(tr("It supports <b>EPG and TimeShift</b>.")) << endl
         << tr("For News and Updates please visit my site at %1.").arg("<a href='http://vlc-record.coujo.de'>http://vlc-record.coujo.de</a>") << endl
         << "<br /> <br />" << tr("<b>vlc-record is free software!</b>") << endl
         << tr("Nevertheless ... if you like this software, please support me: ") << endl
         << "<a href='https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=11286909'>"
         << tr("Donate some $ and / or &euro; at Paypal for my work.") << "</a><br /> <br />" << endl
         << tr("Thank you,") << "<br />&nbsp;&nbsp;&nbsp;&nbsp;J&ouml;rg" << endl;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

