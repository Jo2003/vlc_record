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
#include "externals_inc.h"


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
#ifdef __INFO_WINDOW_CONTENT
   addOrderInfo();
#endif // __INFO_WINDOW_CONTENT
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
|  Parameters: sExpires - string with date and time
|
|  Returns: --
\----------------------------------------------------------------- */
void CAboutDialog::FillInfo(QString sExpires)
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
   // expires ...
   td    = pHtml->tableCell(tr("Account expires:") , "font-weight: bold;");
   td   += pHtml->tableCell(sExpires               , "padding-left: 7px;");
   rows += pHtml->tableRow(td);
   // icons ...
   link  = pHtml->link("http://dryicons.com", "dryicons.com", "", linkcss);
   td    = pHtml->tableCell("Most icons:"          , "font-weight: bold;");
   td   += pHtml->tableCell(link                   , "padding-left: 7px;");
   rows += pHtml->tableRow(td);

   // wrap into table ...
   strAbout += pHtml->htmlTag("table", rows) + "<br /> <br />";

   strAbout += pHtml->htmlTag("b", tr("This program is free software!")) + " ";
   strAbout += tr("Nevertheless ... if you like this software, please support me: ");
   strAbout += pHtml->link("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=11286909",
                           tr("Donate some $ and / or &euro; at Paypal for my work."), "", linkcss) + "<br /> <br />";
   strAbout += tr("Thank you,") + "<br />&nbsp;&nbsp;&nbsp;J&ouml;rg";
   strAbout  = pHtml->htmlPage(strAbout);
}

#ifdef __INFO_WINDOW_CONTENT
//---------------------------------------------------------------------------
//
//! \brief   add order information in first tab
//
//! \author  Jo2003
//! \date    27.05.2015
//
//! \return  --
//---------------------------------------------------------------------------
void CAboutDialog::addOrderInfo()
{
   QWidget*      pOrderWidgetTab = new QWidget;
   QHBoxLayout*  pLayout         = new QHBoxLayout;
   QTextBrowser* pContent        = new QTextBrowser;
   QString       sContent        = pAppTransl->translate("infoWindowContent", __INFO_WINDOW_CONTENT);
   QString       sCss;

   if (sContent.isEmpty())
   {
      sContent = __INFO_WINDOW_CONTENT;
   }

   // CSS
   sCss   = "body{background-color: rgb(255, 254, 212); color: #151515;}";
   sCss  += "a{text-decoration: underline; color: #0482FE;}";

   // table width ...
   sContent = sContent.arg("98%");

   sContent = pHtml->htmlPage(sContent, tr("Order Info"), sCss);

   // add order content ...
   pContent->setHtml(sContent);

   // make sure browser can be opened ...
   pContent->setOpenExternalLinks(true);

   // add widget to layout and create new tab ...
   pLayout->addWidget(pContent);
   pOrderWidgetTab->setLayout(pLayout);
   ui->tabWidget->insertTab(0, pOrderWidgetTab, tr("Order Info"));

   // set active tab
   ui->tabWidget->setCurrentIndex(0);
}
#endif // __INFO_WINDOW_CONTENT

/************************* History ***************************\
| $Log$
\*************************************************************/

