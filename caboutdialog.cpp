/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/caboutdialog.cpp $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:13:41
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: caboutdialog.cpp 1265 2013-12-13 14:40:16Z Olenka.Joerg $
\*************************************************************/
#include "caboutdialog.h"
#include "ui_caboutdialog.h"

#include "qcustparser.h"
#include "chtmlwriter.h"
#include "templates.h"

// global customization class ...
extern QCustParser *pCustomization;

// global html writer ...
extern CHtmlWriter *pHtml;

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
CAboutDialog::CAboutDialog(QWidget *parent, const cparser::SAccountInfo& aInfo) :
    QDialog(parent),
    ui(new Ui::CAboutDialog)
{
   ui->setupUi(this);
   FillInfo(aInfo);
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
   switch (e->type())
   {
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
|  Parameters: aInfo ref. to account info
|
|  Returns: --
\----------------------------------------------------------------- */
void CAboutDialog::FillInfo(const cparser::SAccountInfo& aInfo)
{
   strAbout = "";
   QString td, rows, link;
   const char* linkcss = "text-decoration: underline; color: #0482FE;";

   strAbout = pHtml->span(pCustomization->strVal("APP_NAME"), "font-weight: bold; font-size: 16px;") + "<br />";
   // version ...
   td    = pHtml->tableCell(tr("Version:")         , "font-weight: bold;");
   td   += pHtml->tableCell(__MY__VERSION__        , "padding-left: 7px;");
   rows  = pHtml->tableRow(td);

   // libvlc version ...
   link  = pHtml->link("http://www.videolan.org", "VideoLAN.org", "", linkcss);
   link  = QString("%1 by %2").arg(libvlc_get_version()).arg(link);
   td    = pHtml->tableCell(tr("libVLC:")          , "font-weight: bold;");
   td   += pHtml->tableCell(link                   , "padding-left: 7px;");
   rows += pHtml->tableRow(td);

   // author ...
   link  = pHtml->link("mailto:coujo@gmx.net", "Jo2003", "", linkcss);
   td    = pHtml->tableCell(tr("Author:")          , "font-weight: bold;");
   td   += pHtml->tableCell(link                   , "padding-left: 7px;");
   rows += pHtml->tableRow(td);
#ifndef _IS_OEM
   // project site ...
   link  = pHtml->link("http://vlc-record.coujo.de", "vlc-record.coujo.de", "", linkcss);
   td    = pHtml->tableCell(tr("Project Site:" )   , "font-weight: bold;");
   td   += pHtml->tableCell(link                   , "padding-left: 7px;");
   rows += pHtml->tableRow(td);
#endif // _IS_OEM
   // inspiration ...
   td    = pHtml->tableCell(tr("Inspired by:")     , "font-weight: bold;");
   td   += pHtml->tableCell("Olenka!"              , "padding-left: 7px;");
   rows += pHtml->tableRow(td);
   // qt version ...
   link  = pHtml->link("http://qt-project.org", "Qt Project", "", linkcss);
   link  = QString("Qt %1 by %2").arg(qVersion()).arg(link);
   td    = pHtml->tableCell("SDK:"                 , "font-weight: bold;");
   td   += pHtml->tableCell(link                   , "padding-left: 7px;");
   rows += pHtml->tableRow(td);
   // icons ...
   link  = pHtml->link("http://dryicons.com", "dryicons.com", "", linkcss);
   td    = pHtml->tableCell("Most icons:"          , "font-weight: bold;");
   td   += pHtml->tableCell(link                   , "padding-left: 7px;");
   rows += pHtml->tableRow(td);

   // row
   td    = pHtml->tableCell("<hr />", "", 2);
   rows += pHtml->tableRow(td);

   // spacer
   td    = pHtml->tableCell("", "", 2);
   rows += pHtml->tableRow(td);

   td    = pHtml->tableCell(tr("Made for %1").arg(pCustomization->strVal("APP_NAME")), "font-weight: bold; color: #800;", 2, "center");
   rows += pHtml->tableRow(td);

   // spacer
   td    = pHtml->tableCell("", "", 2);
   rows += pHtml->tableRow(td);

   td    = pHtml->tableCell(tr("Account Info:"), "font-weight: bold;", 2);
   rows += pHtml->tableRow(td);

   td    = pHtml->tableCell(tr("User"));
   td   += pHtml->tableCell(aInfo.sName, "padding-left: 7px;");
   rows += pHtml->tableRow(td);

   td    = pHtml->tableCell(tr("E-Mail"));
   td   += pHtml->tableCell(aInfo.sMail, "padding-left: 7px;");
   rows += pHtml->tableRow(td);

   // spacer
   td    = pHtml->tableCell("", "", 2);
   rows += pHtml->tableRow(td);

   td    = pHtml->tableCell(tr("Packets:"), "font-weight: bold;", 2);
   rows += pHtml->tableRow(td);

   // Services ...
   foreach (const QString& key, aInfo.services.keys())
   {
       td    = pHtml->tableCell(key);
       td   += pHtml->tableCell(aInfo.services.value(key), "padding-left: 7px;");
       rows += pHtml->tableRow(td);
   }



   // wrap into table ...
   strAbout += pHtml->htmlTag("table", rows) + "<br /> <br />";

   strAbout += pHtml->htmlTag("b", tr("This program is free software!")) + " ";
   strAbout += tr("Nevertheless ... if you like this software, please support me: ");
   strAbout += pHtml->link("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=11286909",
                           tr("Donate some $ and / or &euro; at Paypal for my work."), "", linkcss) + "<br /> <br />";
   strAbout += tr("Thank you,") + "<br />&nbsp;&nbsp;&nbsp;J&ouml;rg";
   strAbout  = pHtml->htmlPage(strAbout);
}

/************************* History ***************************\
| $Log$
\*************************************************************/

