/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/caboutdialog.cpp $
| 
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 16:13:41
| 
| Last edited by: $Author: joergn $
| 
| $Id: caboutdialog.cpp 175 2010-01-19 14:22:13Z joergn $
\*************************************************************/
#include "caboutdialog.h"
#include "ui_caboutdialog.h"

/* -----------------------------------------------------------------\
|  Method: CAboutDialog / constructor
|  Begin: 18.01.2010 / 16:14:06
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Method: changeEvent
|  Begin: 18.01.2010 / 16:15:15
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
|  Description: fill about info into string
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CAboutDialog::FillInfo()
{
   strAbout = tr(
      "<style type='text/css'>\n"
      "  a:link, a:visited, a:active { text-decoration: underline; color: #0482FE;}\n"
      "</style>\n"
      "<span style='font-weight: bold; font-size: 16px;'>vlc-record</span><br />\n"
      "<table border='0' cellpadding='0' cellspacing='0'>\n"
      "<tr><td><b>Version:</b></td><td style='padding-left: 15px;'>%1</td></tr>\n"
      "<tr><td><b>Author:</b></td><td style='padding-left: 15px;'><a href='mailto:coujo@gmx.net'>J&ouml;rg Neubert</a></td></tr>\n"
      "<tr><td><b>Inspired by:</b></td><td style='padding-left: 15px;'>Olga!</td></tr>\n"
      "<tr><td><b>SDK:</b></td><td style='padding-left: 15px;'>Qt 4.6 by <a href='http://qt.nokia.com'>Nokia</a></td></tr>\n"
      "<tr><td><b>Icons:</b></td><td style='padding-left: 15px;'>by <a href='http://dryicons.com'>dryicons.com</a></td></tr>\n"
      "</table>\n"
      "<br /> <br />\n"
      "<b>vlc-record</b> - a tool to <b>view / record program streams</b> sent from "
      "<a href='http://www.kartina.tv'>Kartina.tv</a>. "
      "To do this it will authenticate you with username and password at kartina.tv. "
      "It then will start the <a href='http://www.videolan.org'>vlc media player</a> "
      "to display (and record) the selected stream. It supports <b>EPG and TimeShift</b>.<br />\n"
      "For News and Updates please visit my site at <a href='http://www.coujo.de'>http://www.coujo.de</a>.\n"
      "<br /> <br />\n"
      "<b>vlc-record is free software!</b> \n"
      "Nevertheless ... if you like this software, please support me: \n"
      "<a href='https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=11286909'>"
      "Donate some $ and / or &euro;</a> at PayPal for my work.\n"
      "<br /> <br />\n"
      "Thank you,<br />\n"
      "&nbsp;&nbsp;&nbsp;&nbsp;J&ouml;rg"
   ).arg(__MY__VERSION__);
}

/************************* History ***************************\
| $Log$
\*************************************************************/

