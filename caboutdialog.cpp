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
   ui->textShortcuts->setHtml(sShortCuts);
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
         << tr("<b>vlc-record</b> - a tool to <b>view / record program streams</b> from %1%2").arg(COMPANY_LINK)
#ifdef _IS_OEM
              .arg(tr(" (click to buy a subscription).")) << endl
#else
              .arg(".") << endl
#endif
         << tr("To do this you will need to authorise using username and password provided by %1.").arg(COMPANY_NAME) << endl
         << tr("It then will start the %1 to display (and record) the selected stream.").arg("<a href='http://www.videolan.org'>vlc media player</a>") << endl
         << QString("%1<br />").arg(tr("It supports <b>EPG and TimeShift</b>.")) << endl
         << tr("For News and Updates please visit my site at %1.").arg("<a href='http://vlc-record.coujo.de'>http://vlc-record.coujo.de</a>") << endl
         << "<br /> <br />" << tr("<b>vlc-record is free software!</b>") << endl
         << tr("Nevertheless ... if you like this software, please support me: ") << endl
         << "<a href='https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=11286909'>"
         << tr("Donate some $ and / or &euro; at Paypal for my work.") << "</a><br /> <br />" << endl
         << tr("Thank you,") << "<br />&nbsp;&nbsp;&nbsp;&nbsp;J&ouml;rg" << endl;

   sShortCuts = "";
   str.setString(&sShortCuts);
   str << "<style type='text/css'>" << endl
         << ".tabhead {backgound-color: #036; color: white; font-weight: bold;}" << endl
         << ".tabrow {background-color: white; color: #036;}" << endl
         << ".small {font-size: 9px; color: #800;}"
         << "</style>" << endl
         << "<div align='center'><br />" << endl
         << "<table width='95%' cellpadding='3' cellspacing='1' border='0' bgcolor='#036'>" << endl
         << "<tr><th class='tabhead'>" << tr("Function") << "</th><th class='tabhead'>" << tr("Shortcut") << "</th></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Play") << "</td><td class='tabrow'>ALT+P</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Stop") << "</td><td class='tabrow'>ALT+S</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Record") << "</td><td class='tabrow'>ALT+R</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Timer Record") << "</td><td class='tabrow'>ALT+T</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Favourites 0 ... 9") << "</td><td class='tabrow'>ALT+0 ... 9</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Enlarge Font Size") << "</td><td class='tabrow'>ALT++</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Reduce Font Size") << "</td><td class='tabrow'>ALT+-</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Open Info Dialog") << "</td><td class='tabrow'>ALT+I</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Open Settings Dialog") << "</td><td class='tabrow'>ALT+S</td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Quit VLC Record") << "</td><td class='tabrow'>ALT+Q</td></tr>" << endl
#ifdef INCLUDE_LIBVLC
         << "<tr><td class='tabrow'>" << tr("Toggle Fullscreen") << "</td><td class='tabrow'>ALT+F<span class='small'>*</span></td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Switch Aspect Ratio") << "</td><td class='tabrow'>ALT+A<span class='small'>*</span></td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Switch Crop Geometry") << "</td><td class='tabrow'>ALT+C<span class='small'>*</span></td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Jump ~2 minutes forward") << "</td><td class='tabrow'>CTRL+ALT+F<span class='small'>**</span></td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Jump ~2 minutes backward") << "</td><td class='tabrow'>CTRL+ALT+B<span class='small'>**</span></td></tr>" << endl
         << "<tr><td class='tabrow'>" << tr("Pause") << "</td><td class='tabrow'>CTRL+ALT+P<span class='small'>**</span></td></tr>" << endl
#endif // INCLUDE_LIBVLC
         << "</table></div><br />" << endl
#ifdef INCLUDE_LIBVLC
         << "<span class='small'>*" << tr("Only works in version 2.xx with activated libVLC.") << "</span><br />" << endl
         << "<span class='small'>**" << tr("Only works on archive play.") << "</span><br />" << endl
         << "<span class='small'>" << tr("Shortcuts don't work on Linux in fullscreen mode. Double click with mouse to change to windowed mode.") << "</span>"
#endif // INCLUDE_LIBVLC
         << endl;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

